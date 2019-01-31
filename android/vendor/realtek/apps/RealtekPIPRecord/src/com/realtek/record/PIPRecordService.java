package com.realtek.record;

import android.app.Service;
import android.os.AsyncTask;
import android.os.Binder;
import android.os.Bundle;
import android.os.Environment;
import android.os.IBinder;
import android.os.ParcelFileDescriptor;
import android.os.SystemClock;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileWriter;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.IOException;
import java.net.MulticastSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.Date;

import com.realtek.hardware.RtkPIPRecordManager;


/**
 * Descriptions:
 *  this is class to demo RtkPIPRecordManager
 */
public class PIPRecordService extends Service {
    private static final String TAG = "PIPRecordService";

    public static boolean mServiceIsAlive = false;
    private File mRecordFile = null;
    public RtkPIPRecordManager mRtkPIPRecordManager;
    /**
     * video and audio parameters
     */
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
    
    /**
     * udp streaming related variables
     */
    private MulticastSocket multicastSocket;
    // pipe
    private ParcelFileDescriptor[] mPipe;
    private ParcelFileDescriptor mReadPfd = null;
    private ParcelFileDescriptor mWritePfd = null;
    private FileOutputStream mFileOutputStream;
    private boolean mLocalSocketTaskAlive;
    private byte[] mPipeBuffer = new byte[4096];
    private boolean mProcessingUDPSocket;
    private ParcelFileDescriptor mUdpSocketPfd;
    

    @Override
    public IBinder onBind (Intent intent) {
        return null;
    }
    
    @Override
    public void onCreate() {
        Log.d(TAG, "onCreate");
        super.onCreate();
        mServiceIsAlive = true;
    }
    
    @Override
    public int onStartCommand (Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand");
        
        /**
         * obtain video and audio parameters
         */
        Bundle bundle = intent.getExtras();
        mTrackOption = bundle.getInt("track-option");
        mSourceType = bundle.getInt("source-type");
        mOutputFormat = bundle.getInt("output-format");
        mWidth = bundle.getInt("width");
        mHeight = bundle.getInt("height");
        mFrameRate = bundle.getInt("frame-rate");
        mBitRate = bundle.getInt("bit-rate");
        mFrameInterval = bundle.getInt("i-frame-interval");
        mSampleRate = bundle.getInt("sample-rate");
        mChannelCount = bundle.getInt("channel-count");
        mStreamPort = bundle.getInt("stream-port");
        mStreamIp = bundle.getString("stream-ip");
        mStoragePath = bundle.getString("storage-path");   
        
        /**
         * create ParcelFileDescriptor instance used to storage media data
         */
        ParcelFileDescriptor pfd = null;
        if(mStoragePath.equals("UDP")) {
            Log.d(TAG, "UDP streaming");
            mProcessingUDPSocket = true;
            new DatagramSocketCreateTask().executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
            while(mProcessingUDPSocket) {
                SystemClock.sleep(100);
            }
    
            pfd = mWritePfd;
    
            if(pfd != null) {
                mLocalSocketTaskAlive = true;
                new LocalSocketReadTask().executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
            }            
        } else {
            Log.d(TAG, "store in local file");
            pfd = createLocalFd();
        }      
        
        if(pfd == null) {
            Log.e(TAG, "create fd error");
            return Service.START_NOT_STICKY;
        }
        
        /**
         *create & configure & start RtkPIPRecordManager
         */
        mRtkPIPRecordManager = new RtkPIPRecordManager();
        mRtkPIPRecordManager.prepare(mSourceType); // Mixer1 or V1
        
        RtkPIPRecordManager.VideoConfig mVideoConfig = new RtkPIPRecordManager.VideoConfig(
                                                        mWidth, mHeight, mFrameRate, mBitRate, mFrameInterval, 1, 0, 0, 0);
        RtkPIPRecordManager.AudioConfig mAudioConfig = new RtkPIPRecordManager.AudioConfig(
                                                        mChannelCount, 0, mSampleRate, 32000);
        mRtkPIPRecordManager.configureTargetFormat(mVideoConfig, mAudioConfig);
               
        mRtkPIPRecordManager.setTargetFd(pfd, RtkPIPRecordManager.OUTPUT_FILE_FORMAT_TS);
        
        int err = 0;       
        err = mRtkPIPRecordManager.start();
        if(err != 0) {
            Log.d(TAG, "start PIPRecordManager failed!");
            if(!mStoragePath.equals("UDP")) {
                if(mRecordFile!=null && mRecordFile.exists()) {
                    mRecordFile.delete();
                }
            }
            Toast.makeText(this, "Can't get data from V1 source, stop PIPRecordService!", Toast.LENGTH_SHORT).show(); 
            stopSelf();
        } else {
            Log.d(TAG, "start PIPRecordManager success");
        }

        return Service.START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        super.onDestroy();
        mServiceIsAlive = false;
        mProcessingUDPSocket = false;
        mLocalSocketTaskAlive = false;
        if(mRtkPIPRecordManager != null) {
            mRtkPIPRecordManager.stop();
            mRtkPIPRecordManager.release();
        }
    }    

    private ParcelFileDescriptor createLocalFd() {
        String extension = ".ts";
        if(mOutputFormat == RtkPIPRecordManager.OUTPUT_FILE_FORMAT_MP4) {
            extension = ".mp4";
        }

        mRecordFile = getOutputMediaFile(mStoragePath, "PIPRecord", extension);
        try {
            int mode = ParcelFileDescriptor.MODE_CREATE|ParcelFileDescriptor.MODE_READ_WRITE;
            ParcelFileDescriptor pfd = ParcelFileDescriptor.open(mRecordFile,mode);
            return pfd;
        } catch(FileNotFoundException e) {
            e.printStackTrace();
            return null;
        }
    }
    
    public static File getOutputMediaFile(String path, String token,String extension){
        // To be safe, you should check that the SDCard/USB disk is mounted
        File storagePath = new File(path);
        if (!Environment.getExternalStorageState(storagePath).equalsIgnoreCase(Environment.MEDIA_MOUNTED)) {
            return  null;
        }

        File mediaStorageDir = new File(path + File.separator + "PIPRecord");

        // Create the storage directory if it does not exist
        if (! mediaStorageDir.exists()){
            if (! mediaStorageDir.mkdirs()) {
                Log.d(TAG, "failed to create directory");
                return null;
            }
        }

        // Create a media file name
        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
        File mediaFile;
        mediaFile = new File(mediaStorageDir.getPath() + File.separator +
                    "VID_"+ timeStamp +"-"+ token +extension);


        return mediaFile;
    }

    /**
     * Consider that network IO has protential broken issue,
     * we use a pipe to redirect network I/O
     */
    private ParcelFileDescriptor createUdpFd() {
        // create pipe
        try {
            mPipe = ParcelFileDescriptor.createPipe();
        }catch(IOException e) {
            e.printStackTrace();
            return null;
        }

        // assign read/write fd
        mReadPfd = mPipe[0];
        mWritePfd = mPipe[1];

        try {
            multicastSocket = new MulticastSocket(mStreamPort);
            multicastSocket.setBroadcast(true);
            multicastSocket.connect(InetAddress.getByName(mStreamIp),mStreamPort);
            mUdpSocketPfd = ParcelFileDescriptor.fromDatagramSocket(multicastSocket);

            mFileOutputStream = new FileOutputStream(mUdpSocketPfd.getFileDescriptor());
            Log.d(TAG, "udp fd create complete");
            return mWritePfd;
        } catch(SocketException e) {
            e.printStackTrace();
            return null;
        } catch(UnknownHostException e) {
            e.printStackTrace();
            return null;
        } catch(IOException e) {
            e.printStackTrace();
            return null;
        }
    }
    
    class DatagramSocketCreateTask extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected Boolean doInBackground(Void... voids) {
            ParcelFileDescriptor fd = createUdpFd();
            Log.d(TAG, "DatagramSocketCreateTask doInBackground done fd:"+fd);
            mProcessingUDPSocket=false;
            if(fd!=null)
                return true;
            else
                return false;
        }
        @Override
        protected void onPostExecute(Boolean result) {
            Log.d(TAG, "DatagramSocketCreateTask set mProcessingUDPSocket to "+mProcessingUDPSocket);
        }
    } 
    
    class LocalSocketReadTask extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected Boolean doInBackground(Void... voids) {
            Log.d(TAG, "LocalSocketReadTask doInBackground..");
            // create InputStream from mReadPfd.
            InputStream ips = new ParcelFileDescriptor.AutoCloseInputStream(mReadPfd);
            FileDescriptor fd = null;
            try {
                fd = mFileOutputStream.getFD();
            } catch(IOException e) {
                e.printStackTrace();
            }

            while(mLocalSocketTaskAlive) {
                // read from pipe read pfd
                try {
                    int data = ips.available();
                    if(data>0) {
                        int readSize = ips.read(mPipeBuffer);
                        if(readSize>0){
                            if(fd != null && fd.valid()) {
                                mFileOutputStream.write(mPipeBuffer,0,readSize);
                            }
                        }
                    }
                } catch(IOException e) {
                    e.printStackTrace();
                    //mLocalSocketTaskAlive = false;
                    fd = null;
                }
            }
            try {
                // always close FileOutputStream
                Log.d(TAG, "close file");
                mFileOutputStream.close();

                /*
                Log.d(TAG, "close read socket");
                mReadLocalSocket.close();
                SystemClock.sleep(50);
                Log.d(TAG, "close serversocket");
                mLocalServerSocket.close();
                */

                Log.d(TAG, "close pipe");
                mWritePfd.close();
                mReadPfd.close();

            } catch(IOException e) {
                e.printStackTrace();
                return false;
            }
            return true;
        }
        @Override
        protected void onPostExecute(Boolean result) {
            Log.d(TAG, "LocalSocketReadTask onPostExecute");
        }
    }    
}