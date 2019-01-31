package com.realtek.transcode;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Bundle;
import android.os.Handler;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.Message;
import android.os.Messenger;
import android.media.MediaMuxer;
import android.util.Log;

import java.util.*;
import java.text.DecimalFormat;
import java.io.File;

import com.realtek.decode.AsyncAudioDecodeThread;
import com.realtek.decode.AsyncVideoDecodeThread;

public class RealtekTranscodeService extends Service {

    public static final String TAG="RealtekTranscodeService";
    private static final int ENCODER_INSTANCES_LIMIT = 4;
    private static int TranscodeInstanceIdx[] = {0, 0, 0, 0};
    private static AsyncVideoDecodeThread mPlayer_async[] = new AsyncVideoDecodeThread[ENCODER_INSTANCES_LIMIT];
    private static AsyncAudioDecodeThread mAudioPlayer_async[] = new AsyncAudioDecodeThread[ENCODER_INSTANCES_LIMIT];
    private static muxerMonitorThread mMuxerMonitorThread[] = new muxerMonitorThread[ENCODER_INSTANCES_LIMIT];
    private static decodeThread mDecodeThread[] = new decodeThread[ENCODER_INSTANCES_LIMIT];
    private static MediaMuxer mMuxer[] = new MediaMuxer[ENCODER_INSTANCES_LIMIT];
    private static boolean mMuxerActive[] = {false, false, false, false};
    private static int mHasVideo[] = new int[ENCODER_INSTANCES_LIMIT];
    private static int mHasAudio[] = new int[ENCODER_INSTANCES_LIMIT];
    private String mFormat[] = new String[ENCODER_INSTANCES_LIMIT];
    private String mAudioMode[] = new String[ENCODER_INSTANCES_LIMIT];
    private String mResolution[] = new String[ENCODER_INSTANCES_LIMIT];
    private int mSampleRate[] = new int[ENCODER_INSTANCES_LIMIT];
    private int mAudioChannel[] = new int[ENCODER_INSTANCES_LIMIT];
    private boolean mBypass[] = new boolean[ENCODER_INSTANCES_LIMIT];
    private String mFrameRate[] = new String[ENCODER_INSTANCES_LIMIT];
    private int mBitRate[] = new int[ENCODER_INSTANCES_LIMIT];
    private int mIFrameInterval[] = new int[ENCODER_INSTANCES_LIMIT];
    private int mDeinterlace[] = new int[ENCODER_INSTANCES_LIMIT];
    private int mBitrateMode[] = new int[ENCODER_INSTANCES_LIMIT];
    private int mProfile[] = new int[ENCODER_INSTANCES_LIMIT];
    private int mLevel[] = new int[ENCODER_INSTANCES_LIMIT];
    private String mSourcePath[] = new String[ENCODER_INSTANCES_LIMIT];
    private String mTargetPath[] = new String[ENCODER_INSTANCES_LIMIT];
    private static int mTrancecodeState[] = new int[ENCODER_INSTANCES_LIMIT];
    private Messenger mMessenger = null;
    private final Handler mHandler = new Handler();
    private int mLastActivatedNode = -1;
    private int mTranscodeQueue = 0;
    public static RealtekTranscodeService sInstance;
    
    public static RealtekTranscodeService getInstance(){
        return sInstance;
    }

    @Override
    public void onCreate(){
        super.onCreate();
        Log.d(TAG, "onCreate");
        sInstance = this;
        //ServiceManager.addService("RealtekTranscode", mBinder);
    }

    @Override
    public void onDestroy(){
        super.onDestroy();
        mHandler.removeCallbacksAndMessages(null);
        Log.d(TAG, "onDestroy");
    }

    @Override
    public IBinder onBind(Intent intent){
        Log.d(TAG, "onBind !!!");
        Bundle extras = intent.getExtras();
        if(extras != null)
            mMessenger = (Messenger)extras.get("MSG_HANDLER");
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent){
        Log.d(TAG, "onUnBind !!!");
        return super.onUnbind(intent);
    }
	//John
	@Override
    public int onStartCommand(Intent intent,int flags,int startId){
		int retVal = super.onStartCommand(intent, flags, startId);
        Log.d(TAG, "onStartCommand and do something--->intent ="+intent+" startId ="+startId);
        return retVal;
    }
    
    private final IRealtekTranscodeService.Stub mBinder = new IRealtekTranscodeService.Stub(){
        public int getTranscodeInstanceIdx()
        {
            int i;
            synchronized (mBinder) {
            for(i=0; i<ENCODER_INSTANCES_LIMIT; i++)
                if(TranscodeInstanceIdx[i] == 0)
                {
                   TranscodeInstanceIdx[i] = 1;
                   mTrancecodeState[i] = 0;
                   mDecodeThread[i] = new decodeThread(i);
                   mMuxerMonitorThread[i] = new muxerMonitorThread(i);
                   Log.d(TAG, "getTranscodeInstanceIdx " + i);
                   return i;
                }
            if(i >= ENCODER_INSTANCES_LIMIT)
                return -1;
            }
            return -1;
        }

        public void setDataSource(int idx, String source, String target, String output, int track)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return;

            Log.d(TAG, "Enter setDataSource");
            mSourcePath[idx] = source;
            mTargetPath[idx] = target;
            mFormat[idx] = output;
            
            switch(track)
            {
                case 0:
                    mHasAudio[idx] = 0;
                    mHasVideo[idx] = 1;
                    break;
                case 1:
                    mHasAudio[idx] = 1;
                    mHasVideo[idx] = 0;
                    break;
                default:
                    mHasAudio[idx] = 1;
                    mHasVideo[idx] = 1;
                    break;
            }
        }

        public void setVideoParameter(int idx, String resolution, String frameRate, int bitRate, int iFrame, int enableDeinterlace, int bitrateMode, int profile, int level)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return;

            Log.d(TAG, "Enter setVideoParameter idx " + idx);
            mResolution[idx] = resolution;
            mFrameRate[idx] = frameRate;
            mBitRate[idx] = bitRate;
            mIFrameInterval[idx] = iFrame;
            mDeinterlace[idx] = enableDeinterlace;
            mBitrateMode[idx] = bitrateMode;
            mProfile[idx] = profile;
            mLevel[idx] = level;
        }

        public void setAudioParameter(int idx, String audioMode, int sampleRate, int channel)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return;

            Log.d(TAG, "Enter setAudioParameter idx " + idx);
            mAudioMode[idx] = audioMode;
            mSampleRate[idx] = sampleRate;
            mAudioChannel[idx] = channel;
            if(mAudioMode[idx].compareTo("ByPass") == 0)
                mBypass[idx] = true;
        }

        public int startTranscode(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return -1;

            Log.d(TAG, "Enter startTranscode idx " + idx);
            if(mLastActivatedNode == -1)
            {
                mHandler.post(mDecodeThread[idx]);
                mLastActivatedNode = idx;
                return 0;
            }

            if(TranscodeInstanceIdx[mLastActivatedNode] == 1 && mMuxerActive[mLastActivatedNode] == false)
            {
                mTranscodeQueue++;
                mHandler.postDelayed(mDecodeThread[idx], 3000*mTranscodeQueue);
            }
            else
            {
                mTranscodeQueue = 0;
                mHandler.post(mDecodeThread[idx]);
            }

            mLastActivatedNode = idx;
            return 0;
        }

        public int stopTranscode(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return -1;

            ForceStopTranscodeTask(idx);
            return 0;
        }

        public boolean isVideoTranscodeAlive(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return false;

            if(mHasVideo[idx] != 0 && mPlayer_async[idx] != null && mPlayer_async[idx].isAlive())
                return true;
            else
                return false;
        }

        public boolean isAudioTranscodeAlive(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return false;
            
            if(mHasAudio[idx] != 0 && mAudioPlayer_async[idx] != null && mAudioPlayer_async[idx].isAlive())
                return true;
            else
                return false;
        }

        public int getTranscodeState(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return 0;
            else
                return mTrancecodeState[idx];
        }
        
        public long getCurrentPTS(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return 0;

            if(mHasVideo[idx] != 0 && isVideoTranscodeAlive(idx))
                return mPlayer_async[idx].curTimeStamp/1000000;
            else if(mHasAudio[idx] != 0 && isAudioTranscodeAlive(idx))
                return mAudioPlayer_async[idx].curTimeStamp/1000000;
            else
                return 0;
        }

        public double getDecodeAverageFPS(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return 0;

            if(mHasVideo[idx] != 0 && isVideoTranscodeAlive(idx))
                return mPlayer_async[idx].mDecodeFPS;
            else if(mHasAudio[idx] != 0 && isAudioTranscodeAlive(idx))
                return mAudioPlayer_async[idx].mDecodeFPS;
            else
                return 0;
        }

        public double getEncodeAverageFPS(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return 0;

            if(mHasVideo[idx] != 0 && isVideoTranscodeAlive(idx))
                return mPlayer_async[idx].mEncodeFPS;
            else if(mHasAudio[idx] != 0 && isAudioTranscodeAlive(idx))
                return mAudioPlayer_async[idx].mEncodeFPS;
            else
                return 0;
        }

        public double getVideoDecodeCurrentFPS(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return 0;

            if(mHasVideo[idx] != 0 && isVideoTranscodeAlive(idx))
                return mPlayer_async[idx].mInstantDecodeframeCount;
            else
                return 0;
        }

        public double getVideoEncodeCurrentFPS(int idx)
        {
            if(TranscodeInstanceIdx[idx] == 0)
                return 0;

            if(mHasVideo[idx] != 0 && isVideoTranscodeAlive(idx))
            {
                double frameCount = mPlayer_async[idx].mInstantEncodeframeCount;
                mPlayer_async[idx].resetInstantFrameCount();
                return frameCount;
            }
            else
                return 0;
        }
    };

    private void createMediaMuxer(int idx)
    {
        File tmpFile = new File(mTargetPath[idx]);
        if(tmpFile.exists())
        {
            Log.d(TAG, "Detected target file " + mTargetPath[idx] + " exists, delete it");
            tmpFile.delete();
        }

        try {
            if(mFormat[idx].compareTo("MPEG2TS") == 0) {
                Log.d(TAG, "Create MediaMuxer with MUXER_OUTPUT_MPEG2TS idx " + idx);
                mMuxer[idx] = new MediaMuxer(mTargetPath[idx], MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG2TS);
            }
            else {
                Log.d(TAG, "Create MediaMuxer with MUXER_OUTPUT_MPEG_4 idx " + idx);
                mMuxer[idx] = new MediaMuxer(mTargetPath[idx], MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            }
        }  catch (java.io.IOException e) {
            Log.e(TAG, "Create muxer ERROR idx " + idx + " !!!");
        }
    }

    private void ForceStopTranscodeTask(int idx)
    {
        Log.d(TAG, "Enter stopTranscode idx " + idx);
        ForceStopMuxerMonitorThread(idx);
        ForceStopAllDecodeThread(idx);

        synchronized (mBinder) {
        TranscodeInstanceIdx[idx] = 0;
        Log.d(TAG, "release TranscodeInstanceIdx " + idx);
        }
    }

    private void ForceStopMuxerMonitorThread(int idx){
        if(mMuxerMonitorThread[idx] != null)
        {
            if(mMuxerMonitorThread[idx].isAlive())
            {
                Log.d(TAG, "Force stop ForceStopMuxerMonitorThread idx " + idx);
                mMuxerMonitorThread[idx].stopThread();
                int timeout = 3;
                while(mMuxerMonitorThread[idx].isAlive())
                {
                    try {
                        Thread.sleep(1000);
                        timeout--;
                        if(timeout < 0)
                        {
                            Log.d(TAG, "MuxerMonitorThread stop TIMEOUT idx " + idx);
                            break;
                        }
                        Log.d(TAG, "ForceStopMuxerMonitorThread WAIT idx " + idx + " ...");
                    } catch(InterruptedException  ex) {
                    }
                }
            }
            mMuxerMonitorThread[idx] = null;
        }
    }
    
    private void ForceStopAllDecodeThread(int idx){

        int timeout = 5;
        if(mDecodeThread[idx] != null) {
            if(mDecodeThread[idx].isAlive())
            {
                Log.d(TAG, "Force stop MainDecodeThread idx "  + idx + " !!!");
                mDecodeThread[idx].stopThread();
            }
        }

        if (mPlayer_async[idx] != null) {
            if(mPlayer_async[idx].isAlive())
            {
                Log.d(TAG, "Force stop VideoDecodeThread idx "  + idx + " !!!");
                mPlayer_async[idx].stopThread();
            }
        }

        if (mAudioPlayer_async[idx] != null) {
            Log.d(TAG, "Force stop AudioDecodeThread "  + idx + " !!!");
            if(mAudioPlayer_async[idx].isAlive())
            {
                mAudioPlayer_async[idx].stopThread();
            }
        }

        timeout = 3;
        if (mDecodeThread[idx] != null) {
            while(mDecodeThread[idx].isAlive())
            {
                try {
                    Thread.sleep(1000);
                    timeout--;
                    if(timeout < 0)
                    {
                        Log.d(TAG, "MainDecodeThread stop TIMEOUT idx "  + idx + " !!!");
                        break;
                    }
                    Log.d(TAG, "ForceStopAllDecodeThread Video WAIT idx "  + idx + " ...");
                } catch(InterruptedException  ex) {
                }
            }
            mPlayer_async[idx] = null;
        }

        timeout = 5;
        if (mPlayer_async[idx] != null) {
            while(mPlayer_async[idx].isAlive())
            {
                try {
                    Thread.sleep(1000);
                    timeout--;
                    if(timeout < 0)
                    {
                        Log.d(TAG, "Start ForceReleaseVideoThread idx "  + idx + " !!!");
                        mPlayer_async[idx].releaseResource();
                        break;
                    }
                    Log.d(TAG, "ForceStopAllDecodeThread Video WAIT idx "  + idx + " ...");
                } catch(InterruptedException  ex) {
                }
            }
            mPlayer_async[idx] = null;
        }

        timeout = 5;
        if (mAudioPlayer_async[idx] != null) {
            while(mAudioPlayer_async[idx].isAlive())
            {
                try {
                    Thread.sleep(1000);
                    timeout--;
                    if(timeout < 0)
                    {
                        Log.d(TAG, "Start ForceReleaseAudioThread idx " + idx + " !!!");
                        mAudioPlayer_async[idx].releaseResource();
                        break;
                    }
                    Log.d(TAG, "ForceStopAllDecodeThread Audio WAIT idx "  + idx + " ...");
                } catch(InterruptedException  ex) {
                }
            }
            mAudioPlayer_async[idx] = null;
        }
        
        if(mMuxer[idx] != null) {
            if(mMuxerActive[idx] == true)
            {
                mMuxer[idx].stop();
                mMuxerActive[idx] = false;
            }
            mMuxer[idx].release();
            mMuxer[idx] = null;
        }
    }

    private class decodeThread extends Thread{
        private boolean stopThread = false;
        private int idx = 0;

        public decodeThread(int idx)
        {
            this.idx = idx;
        }
        
        public void run (){
            int timeout_count = 10;
            if(mHasVideo[idx] > 0)
            {
                createMediaMuxer(idx);
                mPlayer_async[idx] = new AsyncVideoDecodeThread(mSourcePath[idx], mMuxer[idx], mFrameRate[idx], mBitRate[idx], mIFrameInterval[idx], mResolution[idx], mDeinterlace[idx], mBitrateMode[idx], mProfile[idx], mLevel[idx]);
                Log.d(TAG, "Start VideoDecodeThread instance idx " + idx);
                mPlayer_async[idx].start();
            }
                
            if (mHasAudio[idx] > 0)
            {
                if(mAudioPlayer_async[idx] == null || !mAudioPlayer_async[idx].isAlive())
                {
                    while(mHasVideo[idx] > 0 && mPlayer_async[idx].startMs == 0 && stopThread == false)
                    {
                        try {
                            Thread.sleep(1000);
                            timeout_count--;
                            Log.d(TAG, "Wait Video startTime ready idx " + idx + " ...");
                        } catch(InterruptedException  ex) {
                        }
                        if(timeout_count < 0)
                        {
                            if(mMessenger != null)
                            {
                                Message msg = Message.obtain();
                                msg.arg1 = idx;
                                msg.arg2 = -1;
                                msg.obj = "Transcode crash, force to stop current job !!";
                                try {
                                    mMessenger.send(msg);
                                }
                                catch (android.os.RemoteException e1) {
                                    Log.w(TAG, "Exception sending message", e1);
                                }
                                try {
                                    Thread.sleep(1000);//Wait messenger
                                } catch(InterruptedException  ex) {
                                }
                                ForceStopTranscodeTask(idx);
                            }
                            mTrancecodeState[idx] = -1;
                            stopThread = true;
                            break;
                        }
                    }

                    if(stopThread == false)
                    {

                        if(mPlayer_async[idx] != null)
                        {
                            mAudioPlayer_async[idx] = new AsyncAudioDecodeThread(mSourcePath[idx], mPlayer_async[idx].startMs, mMuxer[idx], mBypass[idx], mSampleRate[idx], mAudioChannel[idx]);
                        }
                        else
                        {
                            createMediaMuxer(idx);
                            mAudioPlayer_async[idx] = new AsyncAudioDecodeThread(mSourcePath[idx], 0, mMuxer[idx], mBypass[idx], mSampleRate[idx], mAudioChannel[idx]);
                        }

                        Log.d(TAG, "Start AudioDecodeThread instance idx " + idx);
                        mAudioPlayer_async[idx].start();
                    }
                }
            }

            timeout_count = 10;
            while(((mHasVideo[idx] > 0) && mPlayer_async[idx] != null && mPlayer_async[idx].trackReady == false) || 
                ((mHasAudio[idx] > 0) && mAudioPlayer_async[idx] != null && mAudioPlayer_async[idx].trackReady == false))
            {
                if(stopThread == true)
                    break;
                try {
                    Thread.sleep(1000);
                    timeout_count--;
                    Log.d(TAG, "track not ready WAIT idx " + idx + " ...");
                } catch(InterruptedException  ex) {
                }
                if(timeout_count < 0)
                {
                    if(mMessenger != null)
                    {
                        Message msg = Message.obtain();
                        msg.arg1 = idx;
                        msg.arg2 = -1;
                        msg.obj = "Transcode crash, force to stop current job !!";
                        try {
                            mMessenger.send(msg);
                        }
                        catch (android.os.RemoteException e1) {
                            Log.w(TAG, "Exception sending message", e1);
                        }
                        try {
                            Thread.sleep(1000);//Wait messenger
                        } catch(InterruptedException  ex) {
                        }
                        ForceStopTranscodeTask(idx);
                    }
                    mTrancecodeState[idx] = -1;
                    break;
                }
            }

            if(stopThread == false)
            {
                mMuxer[idx].start();
                mMuxerActive[idx] = true;
                if(mHasVideo[idx] > 0)
                    mPlayer_async[idx].muxFlg = true;
                if(mHasAudio[idx] > 0)
                    mAudioPlayer_async[idx].muxFlg = true;
                mMuxerMonitorThread[idx].start();
            }
        }

        public void stopThread() {
            Log.d(TAG, "decodeThread being terminated idx " + idx);
            stopThread = true;
        }
    };

    public class muxerMonitorThread extends Thread {
        
        private boolean stopThread = false;
        private int mInstantCount = 0;
        private int idx = 0;
        private long mPrevTimestamp = -1;
        private int mErrorTimout = 0;
        private int mFinishedCount = 0;

        public muxerMonitorThread(int idx)
        {
            this.idx = idx;
        }
        
        @Override
        public void run() {
            if(mHasVideo[idx] > 0)
            {                
                while(mPlayer_async[idx] == null && !stopThread)
                {
                    try {
                        Thread.sleep(1000);
                        Log.d(TAG, "muxerMonitorThread video ready WAIT idx " + idx + " ...");
                    } catch(InterruptedException  ex) {
                    }
                }
            }

            if(mHasAudio[idx] > 0)
            {
                while(mAudioPlayer_async[idx] == null && !stopThread)
                {
                    try {
                        Thread.sleep(1000);
                        Log.d(TAG, "muxerMonitorThread last audio ready WAIT idx " + idx + " ...");
                    } catch(InterruptedException  ex) {
                    }
                }
            }
            Log.d(TAG, "Prepare to execute muxerMonitorThread !!!");
            while (!stopThread) {
                try {
                    Thread.sleep(1000);
                    mInstantCount++;
                    long tempTimeStamp;
                    DecimalFormat df = new DecimalFormat("#.##");
                    String decodeFrameRate;
                    String encodeFrameRate;
                    String decodeInstantFrameRate = "";
                    String encodeInstantFrameRate = "";
                    String threadStatus = "";
                    String stateArray = "";
                    String stateInstantArray = "";

                    if(mHasVideo[idx] > 0)
                    {
                        tempTimeStamp = mPlayer_async[idx].curTimeStamp/1000000;
                        decodeFrameRate = df.format(mPlayer_async[idx].mDecodeFPS);
                        encodeFrameRate = df.format(mPlayer_async[idx].mEncodeFPS);

                        if(mHasAudio[idx] > 0 && (mPlayer_async[idx] == null || !mPlayer_async[idx].isAlive()))
                            threadStatus = " (Waiting Audio)";
                        if(mInstantCount >= 3)
                        {
                            decodeInstantFrameRate = df.format(mPlayer_async[idx].mInstantDecodeframeCount/3.0);
                            encodeInstantFrameRate = df.format(mPlayer_async[idx].mInstantEncodeframeCount/3.0);
                            mPlayer_async[idx].resetInstantFrameCount();
                        }
                    }
                    else
                    {
                        tempTimeStamp = mAudioPlayer_async[idx].curTimeStamp/1000000;
                        decodeFrameRate = df.format(mAudioPlayer_async[idx].mDecodeFPS);
                        encodeFrameRate = df.format(mAudioPlayer_async[idx].mEncodeFPS);
                    }
                    int inputFileIdx = mSourcePath[idx].lastIndexOf('/') + 1;
                    int outFileIdx = mTargetPath[idx].lastIndexOf('/') + 1;
                    if((mSourcePath[idx].length() - inputFileIdx) > 10)
                        inputFileIdx = mSourcePath[idx].length() - 10;
                    if((mTargetPath[idx].length() - outFileIdx) > 10)
                        outFileIdx = mTargetPath[idx].length() - 10;
                    String tempSample = mSourcePath[idx].substring(inputFileIdx);
                    String tempTarget = mTargetPath[idx].substring(outFileIdx);
                    if(mMuxer[idx] != null)
                    {
                        stateArray = tempSample + " current PTS " + tempTimeStamp/60 + "m " + tempTimeStamp%60 + "s " +  "Decode FPS " + decodeFrameRate + " Encode FPS " + encodeFrameRate + threadStatus;
                        stateInstantArray = tempSample + " Decode Instant FPS " + decodeInstantFrameRate + " Encode Instant FPS " + encodeInstantFrameRate;
                    }
                    else
                    {
                        stateArray = tempTarget + " transcode DONE !!!";
                        stateInstantArray = "";
                    }
                    if(mPrevTimestamp != tempTimeStamp) {
                        mPrevTimestamp = tempTimeStamp;
                        mErrorTimout = 0;
                    }
                    else
                        mErrorTimout++;
                    if(mHasVideo[idx] > 0 && (mPlayer_async[idx] == null || !mPlayer_async[idx].isAlive()))
                    {
                        if((mHasAudio[idx] == 0 || mAudioPlayer_async[idx] == null || !mAudioPlayer_async[idx].isAlive()) && mMuxer[idx] != null)
                        {
                            if(mMuxerActive[idx] == true && stopThread == false)
                            {
                                mMuxer[idx].stop();
                                mMuxerActive[idx] = false;
                            }

                            if(stopThread == false)
                            {
                                mMuxer[idx].release();
                                mMuxer[idx] = null;
                            }

                            if(mMessenger != null)
                            {
                                Message msg = Message.obtain();
                                msg.arg1 = idx;
                                msg.arg2 = 1;
                                msg.obj = "Transcode finished !!";
                                try {
                                    mMessenger.send(msg);
                                }
                                catch (android.os.RemoteException e1) {
                                    Log.w(TAG, "Exception sending message", e1);
                                }
                            }
                            mTrancecodeState[idx] = 1;
                            Log.d(TAG, "mMuxer normally stoped idx " + idx + " ... ");
                            break;
                        }
                    }
                    else if(mHasVideo[idx] == 0 && mHasAudio[idx] > 0)
                    {
                        if((mAudioPlayer_async[idx] == null || !mAudioPlayer_async[idx].isAlive()) && mMuxer[idx] != null)
                        {
                            if(mMuxerActive[idx] == true && stopThread == false)
                            {
                                mMuxer[idx].stop();
                                mMuxerActive[idx] = false;
                            }

                            if(stopThread == false)
                            {
                                mMuxer[idx].release();
                                mMuxer[idx] = null;
                            }

                            if(mMessenger != null)
                            {
                                Message msg = Message.obtain();
                                msg.arg1 = idx;
                                msg.arg2 = 1;
                                msg.obj = "Transcode finished !!";
                                try {
                                    mMessenger.send(msg);
                                }
                                catch (android.os.RemoteException e1) {
                                    Log.w(TAG, "Exception sending message", e1);
                                }
                            }
                            mTrancecodeState[idx] = 1;
                            Log.d(TAG, "mMuxer normally stoped idx "  + idx + " ...");
                            break;
                        }
                    }
		//John
                Log.d(TAG, stateArray);
                if(mInstantCount == 3)
                {
                    //Log.d(TAG, stateInstantArray);
                    mInstantCount = 0;
                }

                if(mErrorTimout > 5 && threadStatus.compareTo(" (Waiting Audio)") != 0)
                {
                    Log.d(TAG, "Something wrong, transcode is idle idx " + idx + " !!!");
                    mErrorTimout = -5;
                    if(mMessenger != null)
                    {
                        Message msg = Message.obtain();
                        msg.arg1 = idx;
                        msg.arg2 = -1;
                        msg.obj = "Transcode crash, force to stop current job idx " + idx + " !!";
                        try {
                            mMessenger.send(msg);
                        }
                        catch (android.os.RemoteException e1) {
                            Log.w(TAG, "Exception sending message", e1);
                        }
                        try {
                            Thread.sleep(1000);//Wait messenger
                        } catch(InterruptedException  ex) {
                        }
                        ForceStopTranscodeTask(idx);
                    }
                    mTrancecodeState[idx] = -1;
                }

                } catch(InterruptedException  ex) {
                }
/** DBG
                if(mHasAudio[idx] > 0 && mHasVideo[idx] > 0)
                    Log.d(TAG, "[STATE " + idx + "] Audio " + mAudioPlayer_async[idx].flag + " Video " + mPlayer_async[idx].flag);
                else if(mHasVideo[idx] > 0)
                    Log.d(TAG, "[STATE " + idx + "] Video " + mPlayer_async[idx].flag);
                else
                    Log.d(TAG, "[STATE " + idx + "] Audio " + mAudioPlayer_async[idx].flag);
**/
            }

            while(stopThread == false)
            {
                try {
                    Thread.sleep(1000);
                } catch(InterruptedException  ex) {
                }

                if(mTrancecodeState[idx] == 1)
                    mFinishedCount++;

                if(mFinishedCount >= 5)
				//John
				{
					Log.d(TAG, "end transcode normally in this place");					
                    ForceStopTranscodeTask(idx);				
				}
            }
        }
        public void stopThread() {
            Log.d(TAG, "muxerMonitorThread being terminated idx " + idx);
            stopThread = true;
        }
    }
}
