package com.realtek.record;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;

import java.util.*;

import com.realtek.hardware.RtkPIPRecordManager;

public class SettingPanel
{
    public static final String TAG = "SettingPanel";
    
    private static final String START_PIP_RECORD = "Start PIP Record";
    private static final String STOP_PIP_RECORD = "Stop PIP Record";
    
    private String[] mTrack = {"Video + Audio", "VideoOnly", "AudioOnly"};
    private String[] mSource = {"Mixer1", "V1"};
    private String[] mOutputFmt = {"MPEG2TS", "MP4"};
    private String[] mResolution = {"default", "720P", "1080P"};
    private String[] mFR = {"24", "30", "60", "10", "12", "15", "20", "25", "50"};
    private String[] mBR = {"3000000", "5000000", "6000000", "9000000"};
    private String[] mFI = {"1", "0", "3", "5", "10"};
    private String[] mSR = {"48000", "96000", "88200", "64000", "44100", "32000", "24000", "22050", "16000", "12000", "11025", "8000"};
    private String[] mChannel = {"2", "3", "4", "5", "6", "1"};
    
    private Spinner spnTrack = null;
    private Spinner spnSource = null;
    private Spinner spnFormat = null;
    private Spinner spnStoragePath = null;
    private Spinner spnResolution = null;
    private Spinner spnFrameRate = null;
    private Spinner spnBitRate = null;
    private Spinner spnFrameInterval = null;
    private Spinner spnSampleRate = null;
    private Spinner spnChannelCount = null;   
    
    private ArrayAdapter<String> mTrackList;
    private ArrayAdapter<String> mSourceList;
    private ArrayAdapter<String> mFormatList;
    private ArrayAdapter<String> mStoragePathList;
    private ArrayAdapter<String> mResolutionList;
    private ArrayAdapter<String> mFrameRateList;
    private ArrayAdapter<String> mBitRateList;
    private ArrayAdapter<String> mFrameIntervalList;
    private ArrayAdapter<String> mSampleRateList;
    private ArrayAdapter<String> mChannelCountList;
    
    private Button mPIPRecordButton = null;
    private EditText mWidthEt = null;
    private EditText mHeightEt = null;
    private EditText mStreamIpEt = null;
    private EditText mStreanPortEt = null;
    
    private int mTrackOption = RtkPIPRecordManager.TRACK_VIDEO_AND_AUDIO;
    private int mSourceType = RtkPIPRecordManager.TRANSCODED_TYPE_MIXER1;
    private int mOutputFormat = RtkPIPRecordManager.OUTPUT_FILE_FORMAT_TS;
    private int mWidth = 640;
    private int mHeight = 480;
    private int mFrameRate = 24;
    private int mBitRate = 3000000;
    private int mFrameInterval = 1;
    private int mSampleRate = 48000;
    private int mChannelCount = 2;
    private int mStreamPort = 7890;
    private String mStreamIp = "239.0.0.1";
    private String mStoragePath = Environment.getExternalStorageDirectory().toString();
    
    private View mSettingPanel = null;
    private LinearLayout mSettingLayout = null;
    private Context mContext = null;
    private AlertDialog mDialog = null;
    public SettingPanel(View view, AlertDialog dialog) {
        Log.d(TAG, "construct SettingPanel instance");
        mSettingPanel = view;
        mContext = dialog.getContext();
        mDialog = dialog;
        mSettingLayout = (LinearLayout)mSettingPanel.findViewById(R.id.setting);
    }
    
    protected void initPanel() {
        Log.d(TAG, "initPanel");
        
        mWidthEt = (EditText)mSettingLayout.findViewById(R.id.widthText);
        mHeightEt = (EditText)mSettingLayout.findViewById(R.id.heightText);
        mStreamIpEt = (EditText)mSettingLayout.findViewById(R.id.ipText);
        mStreanPortEt = (EditText)mSettingLayout.findViewById(R.id.portText);
        
        // TrackOption
        spnTrack = (Spinner)mSettingLayout.findViewById(R.id.spnTrack);
        mTrackList = new ArrayAdapter<String>(mContext, R.layout.spinner, mTrack);
        spnTrack.setAdapter(mTrackList);
        spnTrack.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                String track = adapterView.getSelectedItem().toString();
                switch(track) {
                    case "Video + Audio":
                        mTrackOption = RtkPIPRecordManager.TRACK_VIDEO_AND_AUDIO;
                        break;
                    case "AudioOnly":
                        mTrackOption = RtkPIPRecordManager.TRACK_AUDIO_ONLY;
                        break;
                    case "VideoOnly":
                        mTrackOption = RtkPIPRecordManager.TRACK_VIDEO_ONLY;
                        break;
                    default:
                        mTrackOption = RtkPIPRecordManager.TRACK_VIDEO_AND_AUDIO; 
                        break;
                }
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });
        spnTrack.setEnabled(false);
        // SourceType
        spnSource = (Spinner)mSettingLayout.findViewById(R.id.spnSourceType);
        mSourceList = new ArrayAdapter<String>(mContext, R.layout.spinner, mSource);
        spnSource.setAdapter(mSourceList);
        spnSource.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                String type = adapterView.getSelectedItem().toString();
                switch(type) {
                    case "Mixer1":
                        mSourceType = RtkPIPRecordManager.TRANSCODED_TYPE_MIXER1;
                        break;
                    case "V1":
                        mSourceType = RtkPIPRecordManager.TRANSCODED_TYPE_V1;
                        break;
                    default:
                        mSourceType = RtkPIPRecordManager.TRANSCODED_TYPE_MIXER1;
                        break;
                }
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });
        // OutputFormat
        spnFormat = (Spinner)mSettingLayout.findViewById(R.id.spnFormat);
        mFormatList = new ArrayAdapter<String>(mContext, R.layout.spinner, mOutputFmt);
        spnFormat.setAdapter(mFormatList);
        spnFormat.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                String format = adapterView.getSelectedItem().toString();
                switch(format) {
                    case "MPEG2TS":
                        mOutputFormat = RtkPIPRecordManager.OUTPUT_FILE_FORMAT_TS;
                        break;
                    case "MP4":
                        mOutputFormat = RtkPIPRecordManager.OUTPUT_FILE_FORMAT_MP4;
                        break;
                    default:
                        mOutputFormat = RtkPIPRecordManager.OUTPUT_FILE_FORMAT_TS;    
                        break;
                }
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });        
        // StoragePath
        spnStoragePath = (Spinner)mSettingLayout.findViewById(R.id.spnStoragePath);
        ArrayList<String> pathList = new ArrayList<String>();
        getStoragePathList(pathList);
        pathList.add("UDP");
        mStoragePathList = new ArrayAdapter<String>(mContext, R.layout.spinner, pathList);
        spnStoragePath.setAdapter(mStoragePathList);
        spnStoragePath.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                mStoragePath = adapterView.getSelectedItem().toString();
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });
        // Resolution
        spnResolution = (Spinner)mSettingLayout.findViewById(R.id.spnResolution);
        mResolutionList = new ArrayAdapter<String>(mContext, R.layout.spinner, mResolution);
        spnResolution.setAdapter(mResolutionList);
        spnResolution.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                String resolution = adapterView.getSelectedItem().toString();
                switch(resolution)
                {
                    case "Original":
                        mWidthEt.setText("640");
                        mHeightEt.setText("480");
                        break;
                    case "1080P":
                        mWidthEt.setText("1920");
                        mHeightEt.setText("1080");
                        break;
                    case "720P":
                        mWidthEt.setText("1280");
                        mHeightEt.setText("720");
                        break;
                    default:
                        mWidthEt.setText("640");
                        mHeightEt.setText("480");
                        break;
                }
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });        
        // FrameRate
        spnFrameRate = (Spinner)mSettingLayout.findViewById(R.id.spnFrameRate);
        mFrameRateList = new ArrayAdapter<String>(mContext, R.layout.spinner, mFR);
        spnFrameRate.setAdapter(mFrameRateList);
        spnFrameRate.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                mFrameRate = Integer.parseInt(adapterView.getSelectedItem().toString());
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });
        // BitRate
        spnBitRate = (Spinner)mSettingLayout.findViewById(R.id.spnBR);
        mBitRateList = new ArrayAdapter<String>(mContext, R.layout.spinner, mBR);
        spnBitRate.setAdapter(mBitRateList);
        spnBitRate.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                mBitRate = Integer.parseInt(adapterView.getSelectedItem().toString());
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });        
        // I-FrameInterval
        spnFrameInterval = (Spinner)mSettingLayout.findViewById(R.id.spnFI);
        mFrameIntervalList = new ArrayAdapter<String>(mContext, R.layout.spinner, mFI);
        spnFrameInterval.setAdapter(mFrameIntervalList); 
        spnFrameInterval.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                mFrameInterval = Integer.parseInt(adapterView.getSelectedItem().toString());
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });          
        // SampleRate
        spnSampleRate = (Spinner)mSettingLayout.findViewById(R.id.spnSampleRate);
        mSampleRateList = new ArrayAdapter<String>(mContext, R.layout.spinner, mSR);
        spnSampleRate.setAdapter(mSampleRateList); 
        spnSampleRate.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                mSampleRate = Integer.parseInt(adapterView.getSelectedItem().toString());
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        });         
        // ChannelCount
        spnChannelCount = (Spinner)mSettingLayout.findViewById(R.id.spnChannel);
        mChannelCountList = new ArrayAdapter<String>(mContext, R.layout.spinner, mChannel);
        spnChannelCount.setAdapter(mChannelCountList);
        spnChannelCount.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView adapterView, View view, int position, long id) {
                mChannelCount = Integer.parseInt(adapterView.getSelectedItem().toString());
            }
            public void onNothingSelected(AdapterView arg0) {

            }
        }); 
        // set button listener
        mPIPRecordButton = (Button)mSettingLayout.findViewById(R.id.pip_record_button);
        if(!PIPRecordService.mServiceIsAlive) {
            mPIPRecordButton.setText(START_PIP_RECORD);
        } else {
            mPIPRecordButton.setText(STOP_PIP_RECORD);
        }
        mPIPRecordButton.setOnClickListener(new ButtonClickListener());       
    }
    
    /**
     * PIP Record button listener
     */
    class ButtonClickListener implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            Log.d(TAG, "click pip_record_button");
            if(!PIPRecordService.mServiceIsAlive) {
                // start service
                Intent serviceIntent = new Intent(mContext, com.realtek.record.PIPRecordService.class);
                
                mStreamIp = mStreamIpEt.getText().toString();
                mStreamPort = Integer.parseInt(mStreanPortEt.getText().toString());
                mWidth = Integer.parseInt(mWidthEt.getText().toString());
                mHeight = Integer.parseInt(mHeightEt.getText().toString());
                
                serviceIntent.putExtra("track-option", mTrackOption);
                serviceIntent.putExtra("source-type", mSourceType);
                serviceIntent.putExtra("output-format", mOutputFormat);
                serviceIntent.putExtra("storage-path", mStoragePath);
                serviceIntent.putExtra("width", mWidth);
                serviceIntent.putExtra("height", mHeight);
                serviceIntent.putExtra("frame-rate", mFrameRate);
                serviceIntent.putExtra("bit-rate", mBitRate);
                serviceIntent.putExtra("i-frame-interval", mFrameInterval);
                serviceIntent.putExtra("sample-rate", mSampleRate);
                serviceIntent.putExtra("channel-count", mChannelCount);
                serviceIntent.putExtra("stream-ip", mStreamIp);
                serviceIntent.putExtra("stream-port", mStreamPort);
                
                mContext.startService(serviceIntent);
                mPIPRecordButton.setText(STOP_PIP_RECORD);
                mDialog.dismiss();
            } 
            else {
                //stop service
                Intent serviceIntent = new Intent(mContext, com.realtek.record.PIPRecordService.class);
                mContext.stopService(serviceIntent);
                mPIPRecordButton.setText(START_PIP_RECORD);
            }         
        }
    }
    
    private void getStoragePathList(ArrayList<String> pathList) {
    	pathList.clear();
        List<StorageUtils.StorageInfo> storageList = StorageUtils.getStorageList(mContext);
        for(StorageUtils.StorageInfo info : storageList) {
            pathList.add(info.path);
            Log.d(TAG, "storage path : "+info.path);    
        }
    }
}