package com.rtk.debug;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemProperties;
import android.os.storage.StorageManager;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.util.Log;
import android.widget.Toast;
import com.rtk.debug.util.Utils;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by archerho on 2015/10/1.
 */
public class SettingsFrag extends PreferenceFragment implements Preference.OnPreferenceChangeListener, Preference.OnPreferenceClickListener {
    private static final String TAG = "SettingsFrag";
    private CheckBoxPreference mDumpLogPref;
    private CheckBoxPreference mDumpKernelLogPref;
    private CheckBoxPreference mStartRecordPref;
    private Preference mSavePathPref;
    private CheckBoxPreference mDumpiQiYiPref;
    private CheckBoxPreference mDumpStreamPref;
    private CheckBoxPreference mDumpNetInfoPref;
    private CheckBoxPreference mDumpVideoInfoPref;
    private CheckBoxPreference mDumpBufferInfoPref;
    private CheckBoxPreference mDumpSystemInfoPref;
    private CheckBoxPreference mDumpDvdplayerPref;
    private Preference mDumpAudioPref;
    private List<Preference> prefSwitchList = new ArrayList<Preference>();
    private static boolean sIsRecording = false;
    private Handler mHandler = new Handler();
    public static final String KEY_DUMP_PATH = "rtk.debug.dump_path";
    private boolean mIsRefreshState = false;
    private Preference mStopAllPref;
    private Preference mStartAllPref;
    private Preference mUnmountPref;
    private String mMaxSpaceVolId;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.settings);

        setUpPreferences();
    }

    private void setUpPreferences() {
        Preference titlePref = findPreference(getString(R.string.pref_cat_appearance_key));
        if(titlePref!=null)
            titlePref.setTitle("version:"+Utils.getApkVersion(getActivity()));

        mSavePathPref = (Preference)findPreference(getString(R.string.pref_save_path));
        mStartAllPref = findPreference(getString(R.string.pref_start_all));
        if(mStartAllPref!=null){
            mStartAllPref.setOnPreferenceClickListener(this);
            prefSwitchList.add(mStartAllPref);
        }
        mStopAllPref = findPreference(getString(R.string.pref_stop_all));
        if(mStopAllPref!=null){
            mStopAllPref.setOnPreferenceClickListener(this);
            prefSwitchList.add(mStopAllPref);
        }
        mUnmountPref = findPreference(getString(R.string.pref_unmount_device));
        if(mUnmountPref!=null){
            mUnmountPref.setOnPreferenceClickListener(this);
            prefSwitchList.add(mUnmountPref);
        }
        mDumpLogPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dump_log));
        if(mDumpLogPref!=null) {
            mDumpLogPref.setOnPreferenceChangeListener(this);
            prefSwitchList.add(mDumpLogPref);
        }
        mDumpKernelLogPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dump_kernel_log));
        if(mDumpKernelLogPref!=null) {
            mDumpKernelLogPref.setOnPreferenceChangeListener(this);
            prefSwitchList.add(mDumpKernelLogPref);
        }
        mDumpStreamPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dump_stream));
        if(mDumpStreamPref!=null) {
            mDumpStreamPref.setOnPreferenceChangeListener(this);
            prefSwitchList.add(mDumpStreamPref);
        }
        mDumpNetInfoPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dump_network_info));
        if(mDumpNetInfoPref!=null) {
            mDumpNetInfoPref.setOnPreferenceChangeListener(this);
            prefSwitchList.add(mDumpNetInfoPref);
        }
        mDumpiQiYiPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dump_iQiYi));
        if(mDumpiQiYiPref!=null) {
            mDumpiQiYiPref.setOnPreferenceChangeListener(this);
            prefSwitchList.add(mDumpiQiYiPref);
        }
        mDumpBufferInfoPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dump_buffer_info));
        if(mDumpBufferInfoPref!=null) {
            mDumpBufferInfoPref.setOnPreferenceChangeListener(this);
            prefSwitchList.add(mDumpBufferInfoPref);
        }
        mDumpVideoInfoPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dump_video_info));
        if(mDumpVideoInfoPref!=null) {
            mDumpVideoInfoPref.setOnPreferenceChangeListener(this);
            prefSwitchList.add(mDumpVideoInfoPref);
        }
        mDumpSystemInfoPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dumpsys_meminfo));
        if(mDumpSystemInfoPref !=null) {
            mDumpSystemInfoPref.setOnPreferenceChangeListener(this);
            prefSwitchList.add(mDumpSystemInfoPref);
        }
        mStartRecordPref = (CheckBoxPreference) findPreference(getString(R.string.pref_start_record));
        if(mStartRecordPref!=null) {
            mStartRecordPref.setOnPreferenceChangeListener(this);
        }
        mDumpDvdplayerPref = (CheckBoxPreference) findPreference(getString(R.string.pref_dump_dvdplayer));
        if(mDumpDvdplayerPref!=null) {
            mDumpDvdplayerPref.setOnPreferenceChangeListener(this);
        }
        mDumpAudioPref = findPreference(getString(R.string.pref_dump_audio_log));
        if(mDumpAudioPref!=null) {
            mDumpAudioPref.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
                @Override
                public boolean onPreferenceClick(Preference preference) {
                    log("mDumpAudioPref onPreferenceClick");
                    dumpAudioPref();
                    return false;
                }
            });
            prefSwitchList.add(mDumpAudioPref);
        }

        checkStorageAndInitLogPath();

        DumpDmesg.getInstance(getActivity()).on();
        refreshWorkingStatus();
    }

    private void checkStorageAndInitLogPath() {
        mMaxSpaceVolId = Utils.getMostFreeSpaceVolumeId(getActivity());
        String logRootPath = Utils.getSavedLogsDirectory(getActivity());
        if(!logRootPath.startsWith("/")){
            //save log path as property
            SystemProperties.set(KEY_DUMP_PATH, "");
            if(mSavePathPref!=null)
                mSavePathPref.setTitle("Please plug an external storage");
            for(Preference pref : prefSwitchList){
                pref.setEnabled(false);
            }
        }else {
            //save log path as property
            SystemProperties.set(KEY_DUMP_PATH, logRootPath+"/");
            if(mSavePathPref!=null)
                mSavePathPref.setTitle("Saved path: " + logRootPath+"/");
            for(Preference pref : prefSwitchList){
                pref.setEnabled(true);
            }
        }
    }

    private void refreshWorkingStatus() {
        mIsRefreshState = true;
        //reset
        for(Preference pref : prefSwitchList){
            if(pref instanceof CheckBoxPreference) {
                ((CheckBoxPreference)pref).setChecked(false);
            }
        }

        if(DumpLog.getInstance(getActivity()).isOn()){
            if(mDumpLogPref!=null){
                mDumpLogPref.setChecked(true);
            }
        }
        if(DumpKernelLog.getInstance(getActivity()).isOn()){
            if(mDumpKernelLogPref !=null){
                mDumpKernelLogPref.setChecked(true);
            }
        }
        if(DumpStream.getInstance(getActivity()).isOn()){
            if(mDumpStreamPref!=null)
                mDumpStreamPref.setChecked(true);
        }
        if(DumpNetInfo.getInstance(getActivity()).isOn()){
            if(mDumpNetInfoPref!=null)
                mDumpNetInfoPref.setChecked(true);
        }
        if(DumpVideoInfo.getInstance(getActivity()).isOn()){
            if(mDumpVideoInfoPref!=null)
                mDumpVideoInfoPref.setChecked(true);
        }
        if(DumpiQiYi.getInstance(getActivity()).isOn()){
            if(mDumpiQiYiPref!=null)
                mDumpiQiYiPref.setChecked(true);
        }
        if(DumpBufferInfo.getInstance(getActivity()).isOn()){
            if(mDumpBufferInfoPref!=null)
                mDumpBufferInfoPref.setChecked(true);
        }
        if(DumpSystemInfo.getInstance(getActivity()).isOn()){
            if(mDumpSystemInfoPref!=null)
                mDumpSystemInfoPref.setChecked(true);
        }
        if(DumpDvdplayer.getInstance(getActivity()).isOn()){
            if(mDumpDvdplayerPref!=null)
                mDumpDvdplayerPref.setChecked(true);
        }
        mIsRefreshState = false;
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object o) {
        if (preference == null) return true;
        if (mIsRefreshState) return true;
        log("onPreferenceChange");
        if (preference == mDumpLogPref) {
            dumpLog((Boolean) o);
        } else if (preference == mDumpKernelLogPref) {
            dumpKernel((Boolean) o);
        } else if (preference == mDumpStreamPref) {
            dumpStream((Boolean) o);
        } else if (preference == mDumpNetInfoPref) {
            dumpNetInfo((Boolean) o);
        } else if (preference == mDumpVideoInfoPref) {
            dumpVideoInfo((Boolean) o);
        } else if (preference == mDumpiQiYiPref) {
            dumpiQiYi((Boolean) o);
        } else if (preference == mDumpBufferInfoPref) {
            dumpBufferInfo((Boolean) o);
        } else if (preference == mDumpSystemInfoPref) {
            dumpSystemInfo((Boolean) o);
        } else if (preference == mDumpDvdplayerPref) {
            dumpDvdplayer((Boolean) o);
        }
        return true;
    }

    private void dumpDvdplayer(boolean b) {
        if(b){
            DumpDvdplayer.getInstance(getActivity()).on();
        }else {
            DumpDvdplayer.getInstance(getActivity()).off();
        }
    }

    private void dumpBufferInfo(boolean b) {
        if(b){
            DumpBufferInfo.getInstance(getActivity()).on();
        }else {
            DumpBufferInfo.getInstance(getActivity()).off();
        }
    }

    private void dumpStream(boolean b) {
        if(b){
            DumpStream.getInstance(getActivity()).on();
        }else {
            DumpStream.getInstance(getActivity()).off();
        }
    }

    private void dumpLog(boolean on){
        if(on){
            DumpLog.getInstance(getActivity()).on();
        }else{
            DumpLog.getInstance(getActivity()).off();
            String logFolder = SystemProperties.get(SettingsFrag.KEY_DUMP_PATH);
            try {
                Utils.copyDirectory(new File("/data/anr"), new File(logFolder+"anr"));
            } catch (IOException e) {
                log(e.getMessage());
            }
            try {
                Utils.copyDirectory(new File("/data/tombstones"), new File(logFolder+"tombstones"));
            } catch (IOException e) {
                log(e.getMessage());
            }
        }
    }

    private void dumpKernel(boolean on) {
        if(on){
            DumpKernelLog.getInstance(getActivity()).on();
        }else{
            DumpKernelLog.getInstance(getActivity()).off();
        }
    }

    private void dumpNetInfo(boolean on){
        if(on){
            DumpNetInfo.getInstance(getActivity()).on();
        }else{
            DumpNetInfo.getInstance(getActivity()).off();
        }
    }

    private void dumpVideoInfo(boolean on){
        if(on){
            DumpVideoInfo.getInstance(getActivity()).on();
        }else{
            DumpVideoInfo.getInstance(getActivity()).off();
        }
    }

    private void dumpiQiYi(boolean on){
        if(on){
            DumpiQiYi.getInstance(getActivity()).on();
        }else{
            DumpiQiYi.getInstance(getActivity()).off();
        }
    }

    private void dumpSystemInfo(boolean on){
        log("dumpSystemInfo "+on);
        if(on){
            DumpSystemInfo.getInstance(getActivity()).on();
        }else{
            DumpSystemInfo.getInstance(getActivity()).off();
        }
    }

    private void dumpAudioPref() {
        String msg;
        if(!DumpAudio.getInstance(getActivity()).isOn()){
            msg="fw log is not enable yet";
        }else {
            msg="fw log is enabled";
        }
        Toast.makeText(getActivity(), msg, Toast.LENGTH_LONG).show();
    }

    private void log(String s) {
        Log.d(TAG, s);
    }

    @Override
    public boolean onPreferenceClick(Preference preference) {
        if(preference==mStopAllPref) {
            for (Preference pref : prefSwitchList) {
                if (pref instanceof CheckBoxPreference) {
                    CheckBoxPreference cb = (CheckBoxPreference) pref;
                    onPreferenceChange(cb, false);
                    cb.setChecked(false);
                }
            }
            return true;
        } else if(preference==mStartAllPref){
            for (Preference pref : prefSwitchList) {
                if (pref instanceof CheckBoxPreference) {
                    CheckBoxPreference cb = (CheckBoxPreference) pref;
                    onPreferenceChange(cb, true);
                    cb.setChecked(true);
                }
            }
        } else if(preference==mUnmountPref){
            if(mMaxSpaceVolId==null) return true;
            onPreferenceClick(mStopAllPref);
            new UnmountTask(getContext(), mMaxSpaceVolId).execute();
        }
        return false;
    }

    class UnmountTask extends AsyncTask<Void, Void, Exception> {
        private final Context mContext;
        private final StorageManager mStorageManager;
        private final String mVolumeId;

        public UnmountTask(Context context, String volId) {
            mContext = context.getApplicationContext();
            mStorageManager = mContext.getSystemService(StorageManager.class);
            mVolumeId = volId;
        }

        @Override
        protected Exception doInBackground(Void... params) {
            try {
                mStorageManager.unmount(mVolumeId);
                return null;
            } catch (Exception e) {
                return e;
            }
        }

        @Override
        protected void onPostExecute(Exception e) {
            if (e == null) {
                Toast.makeText(mContext, mContext.getString(R.string.storage_unmount_success,
                        mVolumeId), Toast.LENGTH_SHORT).show();
            } else {
                Log.e(TAG, "Failed to unmount " + mVolumeId, e);
                Toast.makeText(mContext, mContext.getString(R.string.storage_unmount_failure,
                        mVolumeId), Toast.LENGTH_SHORT).show();
            }
        }
    }
}
