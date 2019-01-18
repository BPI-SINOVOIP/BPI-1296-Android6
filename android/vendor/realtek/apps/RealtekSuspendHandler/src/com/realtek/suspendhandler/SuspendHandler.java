package com.realtek.suspendhandler;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
//import android.util.Log;

import android.content.pm.PackageManager;
import android.app.ActivityManager;
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import android.content.ComponentName;
import android.util.Slog;
import android.os.AsyncTask;
import android.os.SystemProperties;
import android.os.SystemClock;
import android.os.storage.StorageManager;
import android.os.storage.StorageVolume;
import android.os.storage.IMountService;
import android.os.ServiceManager;
import android.os.storage.VolumeInfo;
import android.os.storage.DiskInfo;
import android.os.Environment;
import android.os.IBinder;
import android.os.UserHandle;
import android.content.IntentFilter;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.ActivityManager.RunningTaskInfo;

import java.util.GregorianCalendar;
import java.util.Calendar;
import java.util.Date;
import java.util.ArrayList;
import java.util.List;

/**
 * Realtek system suspend/resume handler
 * TODO: should be able to remount device under case that user cancel suspend flow.
 */
public class SuspendHandler extends BroadcastReceiver {

    private static final boolean DEBUG = true;
    private static final String TAG= "RealtekSuspend" ;

    private static final String RED="\033[0;31;31m";
    private static final String YELLOW="\033[0;33;33m";
    private static final String END="\033[m";

    private Context mContext;
	//private int mUnmountCounter = 0;

    // use service to manage wakeLock
    private SuspendService mService;

    public SuspendHandler(SuspendService service){
        mService = service;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        mContext = context;
        String action = intent.getAction();

        // get screen off policy
        int mode = mService.getPlatformScreenOffMode();
        String modeString = (mode==SuspendService.SCREEN_OFF_SUSPEND_ON)?"SCREEN_OFF_SUSPEND_ON":"SCREEN_OFF_SUSPEND_OFF";
        Slog.d(TAG, "onReceive \""+action+"\" mode: \""+modeString+"\"");

        if(action.equals(Intent.ACTION_SCREEN_OFF)){
            if(mode == SuspendService.SCREEN_OFF_SUSPEND_ON){
                onReceiveScreenOff(true);
            }
            //else
            //    onReceiveScreenOff(true);

            // update kernel suspend context
            long suspendContext = mService.getKernelSuspendContext();
            mService.setSuspendContext(suspendContext);

        }else if(action.equals("rtk.intent.action.POWER_SUSPEND")){
            // shutdown process, do not unmount external storages
            onReceiveScreenOff(true);
            long suspendContext = mService.getKernelSuspendContext();
            mService.setSuspendContext(suspendContext);
        }else if(action.equals(Intent.ACTION_SCREEN_ON)){
            // it is good to always process screen on.
            onReceiveScreenOn();
        }
        //else if(action.equals(SuspendService.UNMOUNT_TIMEOUT_BROADCAST)){
        //    onReceiveUnmountTimeout();
        //}
    }

    /*
    private void onReceiveUnmountTimeout(){
        Slog.d(TAG, "process unmount timeout status: mIsSuspendMode:"+mService.mIsSuspendMode+" mUnmountCounter:"+mService.mUnmountCounter);
        if(mService.mIsSuspendMode){
            Slog.d(TAG, "receive unmount timeout during suspend, release wake_lock, and leave suspend mode");
            mService.releaseWakeLock();
            mService.mIsSuspendMode = false;
        }
    }
    */

    private void onReceiveScreenOn(){
        Slog.d(TAG, "process SCREEN_ON");
        mService.acquireWakeLock();
        // remove delayMessage
        long tid = SuspendService.getCurrentThreadID();
        Slog.d(TAG, "tid=("+tid+") remove delay message");
        mService.removeDelayMessage();
        mService.remountSDCards();
        // clear in release wake_lock
        //mService.setWakeUpTimerWithChecking(false);
        // send another broadcast
        sendPlatformOnBroadcast();
    }

    private void sendPlatformOnBroadcast(){
        Intent intent = new Intent();
        intent.setAction("android.intent.action.PLATFORM_SCREEN_ON");
        mContext.sendBroadcast(intent);
    }

    private void sendPlatformOffBroadcast(){
        Intent intent = new Intent();
        intent.setAction("android.intent.action.PLATFORM_SCREEN_OFF");
        mContext.sendBroadcast(intent);
    }

    private void onReceiveScreenOff(boolean bUnmount){

        // clear the list first
        mService.mSDCardList.clear();
        mService.mUnmountedSDCardList.clear();

        sendPlatformOffBroadcast();
        Slog.d(TAG, "process SCREEN_OFF need unmount: "+bUnmount);
        if(mService.mIsSuspendMode){
            Slog.w(TAG, "Warning, mIsSuspendMode is already been set to true");
        }
        mService.mIsSuspendMode = true;

        mService.dumpWakeLockStatus();

        // back to home first
        Slog.d(TAG, "back to home");
        backToHome();
        Slog.d(TAG, "kill Activities");

        killHistoryActivities();
        for(int i=0;i<20;i++){
            if(isAtHomeActivity() == true) { break; }
            SystemClock.sleep(100);
        }

        int unmountCount = 0;

        if(bUnmount){
            Slog.d(TAG, "unmount USB");
            unmountCount = unmountUSB2();
            Slog.d(TAG, "total unmount "+unmountCount+" partitions");
        }

        // if no external storages need to be unmounted, just release wake_lock
        if(unmountCount<=0){
            Slog.d(TAG, "no external storage needs to be unmount, release wake_lock and leave SECRRN_OFF");
            //mService.mIsSuspendMode = false;
            //mService.releaseWakeLock();
            mService.releaseWakeLockDelay(1500);
        }else{
            // set unmount counter to service
            mService.mUnmountCounter = unmountCount;
            // book a timeout handle to remove wake_lock
            bookUnmountTimeoutHandle();
        }
        Slog.d(TAG, "leave SCREEN_OFF");
    }

    /**
     * 
     */
    private void bookUnmountTimeoutHandle(){
        long tid = SuspendService.getCurrentThreadID();
        Slog.d(TAG, "tid=("+tid+") bookUnmountTimeoutHandle");
        // post handler to do the delay job
        mService.postDelayedMessage();

        // PendingIntent would let AlarmManager acquire wake_lock and block the suspend flow, so SCREEN_OFF receiver can not 
        /*
        long trigger_time = 2 * 1000;  // milliseconds - delay several seconds
        AlarmManager am = (AlarmManager) mContext.getSystemService(Context.ALARM_SERVICE);
        long triggerTime = SystemClock.elapsedRealtime() + trigger_time;
        am.set(AlarmManager.ELAPSED_REALTIME, triggerTime, mService.mPi);
        */
    }


    /*
    private void onReceiveUnmounted(){
        Slog.d(TAG, "ACTION_MEDIA_UNMOUNTED");
    }
    */

    /*
    private void wakeLockHandling(){
        Slog.d(TAG, "Process wakeLockHandling");
        
    }
    */

    private void backToHome(){
        Intent homeIntent =  new Intent(Intent.ACTION_MAIN, null);
        homeIntent.addCategory(Intent.CATEGORY_HOME);
        homeIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
        mContext.startActivityAsUser(homeIntent, UserHandle.CURRENT);
    }

    private boolean isAtHomeActivity(){

        final PackageManager pm = mContext.getPackageManager();
        ActivityInfo homeInfo = new Intent(Intent.ACTION_MAIN).addCategory(Intent.CATEGORY_HOME).resolveActivityInfo(pm, 0);

        String homePkgName = homeInfo.packageName;
        String homeClassName = homeInfo.name;

        ActivityManager am = (ActivityManager) mContext.getSystemService(Context.ACTIVITY_SERVICE);
        List<RunningTaskInfo> taskInfo = am.getRunningTasks(1);
        ComponentName componentInfo = taskInfo.get(0).topActivity;
        Slog.d(TAG, "homeInfo pkgName:"+homePkgName+ " ClassName:"+homeClassName);
        Slog.d(TAG, "CURRENT Activity :: Package Name :"+componentInfo.getPackageName()+ "ClassName:"+taskInfo.get(0).topActivity.getClassName());
        if(homePkgName.equals(componentInfo.getPackageName()) && homeClassName.equals(taskInfo.get(0).topActivity.getClassName())){
            return true;
        }else{
            return true;
        }
    }

    // copied from recent task panel in system UI
    // kill history activities
    private void killHistoryActivities(){
        final int DISPLAY_TASKS = 20;
        final int MAX_TASKS = DISPLAY_TASKS + 1; // allow extra for non-apps
        // 1. get Activity list
        final PackageManager pm = mContext.getPackageManager();
        final ActivityManager am = (ActivityManager) mContext.getSystemService(Context.ACTIVITY_SERVICE);
        final List<ActivityManager.RecentTaskInfo> recentTasks = am.getRecentTasks(MAX_TASKS, ActivityManager.RECENT_IGNORE_UNAVAILABLE);
        int numTasks = recentTasks.size();
        ActivityInfo homeInfo = new Intent(Intent.ACTION_MAIN).addCategory(Intent.CATEGORY_HOME).resolveActivityInfo(pm, 0);
        ArrayList<TaskDescription> tasks = new ArrayList<TaskDescription>();
        final int first = 0;
        // for safty, use a stupid way, first, create list, then use a second loop to kill process
        for (int i = first, index = 0; i < numTasks && (index < MAX_TASKS); ++i) {

            final ActivityManager.RecentTaskInfo recentInfo = recentTasks.get(i);
            Intent intent = new Intent(recentInfo.baseIntent);
            if (recentInfo.origActivity != null) {
                intent.setComponent(recentInfo.origActivity);
            }

            // Don't load the current home activity.
            if (isCurrentHomeActivity(intent.getComponent(), homeInfo)) {
                continue;
            }

            // Don't load ourselves
            if (intent.getComponent().getPackageName().equals(mContext.getPackageName())) {
                continue;
            }

            TaskDescription item = createTaskDescription(recentInfo.id,
                            recentInfo.persistentId, recentInfo.baseIntent,
                            recentInfo.origActivity, recentInfo.description);

            if(item!=null){
                tasks.add(item);
                Slog.d(TAG,RED+"TaskItem:"+item+END);
                ++index;
            }
        }
        // loop tasks and kill them.
        for(int i=0;i<tasks.size();i++){
            TaskDescription ad = tasks.get(i);
            boolean rst = am.removeTask(ad.persistentTaskId /*, ActivityManager.REMOVE_TASK_KILL_PROCESS */ );
            Slog.d(TAG, "remove task rst:"+rst);
        }
    }

    TaskDescription createTaskDescription(int taskId, int persistentTaskId, Intent baseIntent,
            ComponentName origActivity, CharSequence description) {
        Intent intent = new Intent(baseIntent);
        if (origActivity != null) {
            intent.setComponent(origActivity);
        }
        final PackageManager pm = mContext.getPackageManager();
        intent.setFlags((intent.getFlags()&~Intent.FLAG_ACTIVITY_RESET_TASK_IF_NEEDED)
                | Intent.FLAG_ACTIVITY_NEW_TASK);
        final ResolveInfo resolveInfo = pm.resolveActivity(intent, 0);
        if (resolveInfo != null) {
            final ActivityInfo info = resolveInfo.activityInfo;
            final String title = info.loadLabel(pm).toString();

            if (title != null && title.length() > 0) {
                if (DEBUG) Slog.v(TAG, "creating activity desc for id="
                        + persistentTaskId + ", label=" + title);

                TaskDescription item = new TaskDescription(taskId,
                        persistentTaskId, resolveInfo, baseIntent, info.packageName,
                        description);
                return item;
            } else {
                if (DEBUG) Slog.v(TAG, "SKIPPING item " + persistentTaskId);
            }
        }
        return null;
    }

    private boolean isCurrentHomeActivity(ComponentName component, ActivityInfo homeInfo) {
        if (homeInfo == null) {
            final PackageManager pm = mContext.getPackageManager();
            homeInfo = new Intent(Intent.ACTION_MAIN).addCategory(Intent.CATEGORY_HOME)
                .resolveActivityInfo(pm, 0);
        }
        return homeInfo != null
            && homeInfo.packageName.equals(component.getPackageName())
            && homeInfo.name.equals(component.getClassName());
    }

    // helper class to keep task description
    class TaskDescription {
        final ResolveInfo resolveInfo;
        final int taskId; // application task id for curating apps
        final int persistentTaskId; // persistent id
        final Intent intent; // launch intent for application
        final String packageName; // used to override animations (see onClick())
        final CharSequence description;

        public TaskDescription(int _taskId, int _persistentTaskId,
            ResolveInfo _resolveInfo, Intent _intent,
            String _packageName, CharSequence _description) {

            resolveInfo = _resolveInfo;
            intent = _intent;
            taskId = _taskId;
            persistentTaskId = _persistentTaskId;

            description = _description;
            packageName = _packageName;
        }

        public String toString(){
            return "[TaskDescription]:"+packageName;
        }
    }

    class UnmountTaskParam {
        List<String> mList;
    }

    class UnmountTask extends AsyncTask<UnmountTaskParam, Void, Boolean> {
        @Override
        protected Boolean doInBackground(UnmountTaskParam... params){
            List<String> list = params[0].mList;
            IBinder service = ServiceManager.getService("mount");

            if(mService.mIsSuspendMode == false){
                return true;
            }

            if (service != null) {
                IMountService mountService = IMountService.Stub.asInterface(service);

                if(list.size() == 0){
                    return true;
                }

                for(int i=0;i<list.size();i++){
                    if(mService.mIsSuspendMode == false){
                        return true;
                    }

                    //Slog.d(TAG+"-Vold",RED+"unmount path:"+list.get(i)+END);
                    try{
                        Slog.d(TAG+"-Vold","unmount path:"+list.get(i)+" start");
                        mountService.unmountVolume(list.get(i), true, false);
                        Slog.d(TAG+"-Vold","unmount path:"+list.get(i)+" done");
                    }catch(android.os.RemoteException e){
                        Slog.e(TAG+"-Vold",RED+"unmount "+list.get(i)+" failed"+END);
                        //mContext.unregisterReceiver(br);
                        return false;
                    }catch(IllegalStateException e){
                        Slog.e(TAG+"-Vold",RED+"unmount "+list.get(i)+" failed (2)"+END);
                        return false;
                    }
                }

                return true;

            }else{
                return false;
            }
        }
        @Override
        protected void onPostExecute(Boolean result){
            Slog.d(TAG, "UnmountTask onPostExecute");
        }
    }

    private int unmountUSB2(){

        Slog.d(TAG, "unmountUSB2");

        StorageManager manager = (StorageManager) mContext.getSystemService(Context.STORAGE_SERVICE);

        // try to get VolumeInfo
        List<VolumeInfo> volumeInfoList = manager.getVolumes();

        int mUnmountCounter=0;
        List<String> list = new ArrayList<String>();

        // for debug, print size of SDCard list
        Slog.d(TAG, "mSDCardList:"+mService.mSDCardList.size()+
                    " mUnmountedSDCardList:"+mService.mUnmountedSDCardList.size());

        for(VolumeInfo v : volumeInfoList){
            DiskInfo d = v.disk;
            if(d!=null){
                Slog.d(TAG, "Path:"+v.path+" SD:"+d.isSd());
                String path = v.path;
                list.add(path);
                if(d.isSd()){
                    mService.mSDCardList.add(path);
                }
            }
        }

        if(list.size()==0){
            Slog.d(TAG, "--- no external device need to unmount");
            return 0;
        }

        mUnmountCounter = list.size();
        Slog.d(TAG+"-Vold",RED+mUnmountCounter+" path to be unmounted"+END);

        // unmount USB in background
        UnmountTaskParam param = new UnmountTaskParam();
        param.mList = list;
        new UnmountTask().execute(param,param,param);

        return mUnmountCounter;
    }

    /**
     * @Deprecated
     * Unmount USB
     * return value nums of partition is unmounted 
     * old implemenation, use unmountUSB2 instead
     */
    private int unmountUSB(){
        StorageManager manager = (StorageManager) mContext.getSystemService(Context.STORAGE_SERVICE);

        // try to get VolumeInfo
        List<VolumeInfo> volumeInfoList = manager.getVolumes();
        for(VolumeInfo v : volumeInfoList){
            DiskInfo d = v.disk;
            if(d!=null){
                Slog.d(TAG, "Path:"+v.path+" SD:"+d.isSd());
            }
        }

        final StorageVolume[] storageVolumes = manager.getVolumeList();
        IBinder service = ServiceManager.getService("mount");
        int mUnmountCounter=0;
        List<String> list = new ArrayList<String>();
        if (service != null) {
            IMountService mountService = IMountService.Stub.asInterface(service);
            // create list
            for(int i=0;i<storageVolumes.length;i++){
                //Slog.d(TAG+"-Vold",RED+"volume info:"+storageVolumes[i]+END);
                if(shouldUnmount(storageVolumes[i])){
                    //Slog.d(TAG+"-Vold",RED+"volume info:"+storageVolumes[i]+END);
                    list.add(storageVolumes[i].getPath());
                }
            }

            if(list.size()==0){
                Slog.d(TAG, "--- no external device need to unmount");
                //SystemClock.sleep(3000);
                return 0;
            }

            mUnmountCounter = list.size();
            Slog.d(TAG+"-Vold",RED+mUnmountCounter+" path to be unmounted"+END);

            /*
            // do unmount
            for(int i=0;i<list.size();i++){
                //Slog.d(TAG+"-Vold",RED+"unmount path:"+list.get(i)+END);
                try{
                    Slog.d(TAG+"-Vold","unmount path:"+list.get(i)+" start");
                    mountService.unmountVolume(list.get(i), true, false);
                    Slog.d(TAG+"-Vold","unmount path:"+list.get(i)+" done");
                }catch(android.os.RemoteException e){
                    Slog.e(TAG+"-Vold",RED+"unmount "+list.get(i)+" failed"+END);
                    //mContext.unregisterReceiver(br);
                    return -1;
                }
            }
            */

            // unmount USB in background
            UnmountTaskParam param = new UnmountTaskParam();
            param.mList = list;
            new UnmountTask().execute(param,param,param);
        }
        return mUnmountCounter;
    }

	private boolean shouldUnmount(StorageVolume v){
        // is interanl
        if(v.isEmulated())
            return false;
        // is mounted
        if(!v.getState().equals(Environment.MEDIA_MOUNTED))
            return false;
        return true;
    }
}
