package com.realtek.decode;

import java.io.IOException;
import java.io.FileOutputStream;

import java.lang.reflect.Method;
import java.nio.ByteBuffer;

import android.media.AudioFormat;
import android.media.AudioAttributes;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.media.MediaCodec.BufferInfo;
import android.media.MediaMuxer;
import android.util.Log;
import android.os.Handler;
import android.os.HandlerThread;

public class AsyncAudioDecodeThread extends Thread {

    public static final String TAG = "AsyncAudioDecodeThread";
    private static int Q_SIZE = 3;
    private MediaExtractor extractor = null;
    private MediaCodec decoder = null;
    private MediaCodec encoder = null;
    private MediaMuxer muxer;
    private String audioPath;
    private MediaFormat mMediaFormat;
    private boolean eosThread = false;
    private boolean formatInited = false;
    private int outOfTimeThreshold = 10;//ms
    private int terminateTimeout = 3;
    private int audioTrackIdx = 0;
    public long startMs = 0;
    public int flag = 0;
    public boolean trackReady = false;
    public boolean muxFlg =false;
    public long curTimeStamp = 0;
    private double mDecodeframeCount = 0;
    private double mEncodeframeCount = 0;
    public double mDecodeFPS = 0;
    public double mEncodeFPS = 0;
    private BufferInfo decoderInfo;
    private HandlerThread decoderCallbackThread = null;
    private HandlerThread encoderCallbackThread = null;
    private Handler decoderHandler;
    private Handler encoderHandler;
    private ByteBuffer mBuffer = null;
    private boolean isEOS = false;
    private boolean byPass = true;
    private int mSampleRate = 48000;
    private int mChannelCount = 2;
    private boolean mStartRealease = false;
    private boolean mDecodeErr = false;
    private boolean mEncodeErr = false;
    private int mRptr = -1;
    private int mWptr = -1;
    private ByteBuffer mBufferQueue[] = new ByteBuffer[Q_SIZE];
    private int mBufferQueueSize[] = new int[Q_SIZE];

    public AsyncAudioDecodeThread(String path, long startTime, MediaMuxer muxer, boolean byPass, int sampleRate, int channel) {
        this.audioPath = path;
        this.startMs = startTime;
        this.muxer = muxer;
        this.byPass = byPass;
        this.mSampleRate = sampleRate;
        this.mChannelCount = channel;
    }

    @Override
    public void run() {
        extractor = new MediaExtractor();
        Log.d(TAG, "Start setDataSource ... " + audioPath);
        try {
            extractor.setDataSource(audioPath);
        } catch (IOException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
        Log.d(TAG, "End setDataSource ... " + audioPath);

        if(byPass == true)
        {
            for (int i = 0; i < extractor.getTrackCount(); i++) {
                MediaFormat format = extractor.getTrackFormat(i);
                String mime = format.getString(MediaFormat.KEY_MIME);
                if (mime.startsWith("audio/"))
                {
                    extractor.selectTrack(i);
                    mMediaFormat = format;
                    Log.d(TAG, "Get mMediaFormat");
                    break;
                }
            }
            audioTrackIdx = muxer.addTrack(mMediaFormat);
            trackReady = true;
            while(muxFlg == false && eosThread != true)
            {
                try {
                    Thread.sleep(100);
                    Log.d(TAG, "Waiting video track ready " + audioPath);
                } catch(InterruptedException  ex) {
                }
            }
            decoderInfo = new BufferInfo();
            mBuffer = ByteBuffer.allocate(1024*1024);
            startMs = System.currentTimeMillis();
            while (true) {
                if (!isEOS) {
                    int sampleSize = extractor.readSampleData(mBuffer, 0);
                    if (sampleSize < 0) {
                        isEOS = true;
                    } else {
                        mBuffer.position(0);
                        mBuffer.limit(sampleSize);
                        decoderInfo.presentationTimeUs = extractor.getSampleTime();
                        decoderInfo.offset = 0;
                        decoderInfo.size = sampleSize;
                        decoderInfo.flags = 0;
                        if(eosThread)
                        {
                            Log.d(TAG, "InputBuffer force BUFFER_FLAG_END_OF_STREAM " + audioPath);
                            decoderInfo.flags = MediaCodec.BUFFER_FLAG_END_OF_STREAM;
                            isEOS = true;
                        }

                        if(muxFlg == true && muxer != null)
                            muxer.writeSampleData(audioTrackIdx, mBuffer, decoderInfo);
                        curTimeStamp = decoderInfo.presentationTimeUs;
//                        Log.d(TAG, "Muxer size " + decoderInfo.size + " PTS " + decoderInfo.presentationTimeUs);
                        mEncodeframeCount++;
                        mEncodeFPS = mEncodeframeCount / (double)((System.currentTimeMillis() - startMs)/1000);
                        extractor.advance();
                    }
                }
                else
                    break;
            }
            extractor.release();
        }
        else
        {
            for (int i = 0; i < extractor.getTrackCount(); i++) {
                MediaFormat format = extractor.getTrackFormat(i);
                String mime = format.getString(MediaFormat.KEY_MIME);
                if (mime.startsWith("audio/")) {
                    extractor.selectTrack(i);
                    try {
                        decoder = MediaCodec.createDecoderByType(mime);
                        Log.d(TAG, "create decoder with mime " + mime + audioPath);
                    } catch (java.io.IOException e) {
                        Log.e(TAG, "Create codec from type ERROR !!! " + audioPath);
                    }

                    if(eosThread == true)
                    {
                        releaseResource();
                        return;
                    }

                    mMediaFormat = format;
                    if(eosThread == true)
                    {
                        releaseResource();
                        return;
                    }

                    decoderCallbackThread = new HandlerThread("DecoderHanlderThread");
                    decoderCallbackThread.start();
                    decoderHandler = new Handler(decoderCallbackThread.getLooper());

                    encoderCallbackThread = new HandlerThread("EncoderHanlderThread");
                    encoderCallbackThread.start();
                    encoderHandler = new Handler(encoderCallbackThread.getLooper());

                    setupDecoderCallback(decoderHandler);

                    if(eosThread == true)
                    {
                        releaseResource();
                        return;
                    }
                    
                    decoder.configure(mMediaFormat, null, null, 0);
                    break;
                }
            }

            if (decoder == null) {
                Log.e(TAG, "Create decoder or encoder FAIL !! " + audioPath);
                trackReady = true;
                return;
            }      

            flag = 0;
            for(int i=0;i<Q_SIZE; i++)
                mBufferQueue[i] = ByteBuffer.allocate(1024*1024);
            decoderInfo = new BufferInfo();
            decoder.start();
            flag = 2;
            startMs = System.currentTimeMillis();
            while (true) {
                if ((decoderInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    Log.d(TAG, "OutputBuffer BUFFER_FLAG_END_OF_STREAM " + audioPath);
                    break;
                }
                else if((isEOS == true) || eosThread == true)
                {
                    try {
                        Thread.sleep(10);
                        Log.d(TAG, "Waiting for OutputBuffer BUFFER_FLAG_END_OF_STREAM " + audioPath);
                    } catch(InterruptedException  ex) {
                    }
                    flag = 29;
                    if(terminateTimeout > 0)
                        terminateTimeout--;
                    else
                    {
                        Log.d(TAG, "Terminate thread occur TIMEOUT !! ");
                        break;
                    }
                    flag = 25;
                }

                try {
                        Thread.sleep(1000);
                    } catch(InterruptedException  ex) {
                    }
            }
            
            releaseResource();
        }
    }

    public void releaseResource()
    {
        eosThread = true;
        if(mStartRealease == false)
        {
            mStartRealease = true;

            flag = 30;

            Log.d(TAG, "Audio Thread start releasing decoder !! " + audioPath);
            if(decoder != null)
            {
                if(mDecodeErr == false)
                {
                    decoder.flush();
                    decoder.stop();
                }
                decoder.release();
            }

            Log.d(TAG, "Audio Thread start releasing encoder !! " + audioPath);
            if(encoder != null)
            {
                if(mEncodeErr == false)
                {
                    encoder.flush();
                    encoder.stop();
                }
                encoder.release();
            }

            Log.d(TAG, "Audio Thread start releasing extractor !! " + audioPath);

            if(extractor != null)
                extractor.release();

            Log.d(TAG, "Audio Thread start releasing callback !! " + audioPath);
            if(decoderCallbackThread != null && encoderCallbackThread != null)
            {
                decoderCallbackThread.quitSafely();
                encoderCallbackThread.quitSafely();
                try {
                    decoderCallbackThread.join();
                    Log.d(TAG, "Audio decoderCallbackThread release done !! " + audioPath);
                    encoderCallbackThread.join();
                    Log.d(TAG, "Audio encoderCallbackThread release done !! " + audioPath);
                    } catch(InterruptedException  ex) {
                    }
            }
            flag = 31;
            Log.d(TAG, "Audio Thread normally Stop !! " + audioPath);
        }
    }
    private void setupDecoderCallback(Handler handle)
    {
        decoder.setCallback(new MediaCodec.Callback() {
        @Override
        public void onInputBufferAvailable(MediaCodec mc, int inputBufferId) 
        {
            if(mDecodeErr == true || mStartRealease == true)
                return;
            if (!isEOS) {
                ByteBuffer buffer = mc.getInputBuffer(inputBufferId);
                flag = 4;
                int sampleSize = extractor.readSampleData(buffer, 0);
                flag = 5;
                if (sampleSize < 0) {
                    Log.d(TAG, "InputBuffer BUFFER_FLAG_END_OF_STREAM " + audioPath);
                    flag = 6;
                    mc.queueInputBuffer(inputBufferId, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                    flag = 7;
                    isEOS = true;
                } else {
                    if(eosThread && formatInited)
                    {
                        Log.d(TAG, "InputBuffer force BUFFER_FLAG_END_OF_STREAM " + audioPath);
                        flag = 8;
                        mc.queueInputBuffer(inputBufferId, 0, sampleSize, extractor.getSampleTime(), MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                        flag = 9;
                        isEOS = true;
                    }
                    else
                    {
                        flag = 10;
                        mc.queueInputBuffer(inputBufferId, 0, sampleSize, extractor.getSampleTime(), 0);
                        flag = 11;
                    }
                    flag = 12;
                    extractor.advance();
                    flag = 13;
                }
            }
        }

        @Override
        public void onOutputBufferAvailable(MediaCodec mc, int outputBufferId, MediaCodec.BufferInfo info) 
        {
            if(mDecodeErr == true || mStartRealease == true)
                return;
            while(((mWptr+1)%Q_SIZE == mRptr || (mRptr==-1 && mWptr==Q_SIZE-1))&& mStartRealease == false)
            {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException  ex) {
                }
            }
            synchronized(mBufferQueue) {
            mWptr = (mWptr+1)%Q_SIZE;
            decoderInfo = info;
            ByteBuffer buffer = mc.getOutputBuffer(outputBufferId);
            buffer.position(info.offset);
            buffer.limit(info.offset + info.size);
            mBufferQueue[mWptr].put(buffer);
            mBufferQueue[mWptr].flip();
            mBufferQueueSize[mWptr] = decoderInfo.size;
            }
            mc.releaseOutputBuffer(outputBufferId, false);
        }

        @Override
        public void onOutputFormatChanged(MediaCodec mc, MediaFormat format) 
        {
            Log.d(TAG, "New format " + decoder.getOutputFormat() + audioPath);
            formatInited = true;
            String encode_mime = MediaFormat.MIMETYPE_AUDIO_AAC;
            int sampleRate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE);
            int channelCount = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
            MediaFormat encode_format = MediaFormat.createAudioFormat(encode_mime, sampleRate, channelCount);
            encode_format.setInteger("audio-samp-rate", mSampleRate);
            encode_format.setInteger("audio-chan-count", mChannelCount);
            encode_format.setInteger(MediaFormat.KEY_AAC_PROFILE, 2);//OMX_AUDIO_AACObjectLC
            encode_format.setInteger(MediaFormat.KEY_BIT_RATE, 128000);
            encode_format.setInteger("max-input-size", 92160);//90K
            try {
                encoder = MediaCodec.createEncoderByType(encode_mime);
            } catch (java.io.IOException e) {
                Log.e(TAG, "Create encoder codec from type ERROR !!!");
            }
            if(encoder != null) {
                setupEncoderCallback(encoderHandler);
                encoder.configure(encode_format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
                encoder.start();
            }
            else
                Log.d(TAG, "Create encoder fail");
            flag = 18;
        }

        @Override
        public void onError(MediaCodec codec, MediaCodec.CodecException e)
        {
            mDecodeErr = true;
            e.printStackTrace();
        }
        }, handle);
    }

    private void setupEncoderCallback(Handler handle)
    {
        encoder.setCallback(new MediaCodec.Callback() {
        @Override
        public void onInputBufferAvailable(MediaCodec mc, int inputBufferId) 
        {
            if(mEncodeErr == true || mStartRealease == true)
                return;
            while(((mRptr+1)%Q_SIZE == mWptr || mWptr == -1 )&& mStartRealease == false)
            {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException  ex) {
                }
            }

            if(mStartRealease == false)
            {
//                Log.d(TAG, "Encode size " + decoderInfo.size + " PTS " + decoderInfo.presentationTimeUs);
                synchronized(mBufferQueue) {
                mRptr = (mRptr+1)%Q_SIZE;
                ByteBuffer buffer = mc.getInputBuffer(inputBufferId);
                buffer.clear();
                buffer.put(mBufferQueue[mRptr]);
                buffer.flip();
                mBufferQueue[mRptr].clear();
                }
                mc.queueInputBuffer(inputBufferId, 0, mBufferQueueSize[mRptr], decoderInfo.presentationTimeUs, 0);
                mDecodeframeCount++;
                mDecodeFPS = mDecodeframeCount / (double)((System.currentTimeMillis() - startMs)/1000);
            }
        }

        @Override
        public void onOutputBufferAvailable(MediaCodec mc, int outputBufferId, MediaCodec.BufferInfo info) 
        {
            if(mEncodeErr == true || mStartRealease == true)
                return;
            ByteBuffer buffer = mc.getOutputBuffer(outputBufferId);
            buffer.position(info.offset);
            buffer.limit(info.offset + info.size);
            if(muxFlg == true && muxer != null && eosThread == false)
                muxer.writeSampleData(audioTrackIdx, buffer, info);
            curTimeStamp = info.presentationTimeUs;
//            Log.d(TAG, "Muxer size " + info.size + " PTS " + info.presentationTimeUs);
            mc.releaseOutputBuffer(outputBufferId, false);
            mEncodeframeCount++;
            mEncodeFPS = mEncodeframeCount / (double)((System.currentTimeMillis() - startMs)/1000);
            flag = 24;
        }

        @Override
        public void onOutputFormatChanged(MediaCodec mc, MediaFormat format) 
        {
            Log.d(TAG, "New format " + encoder.getOutputFormat() + audioPath);
            audioTrackIdx = muxer.addTrack(encoder.getOutputFormat());
            trackReady = true;
            int errTimeout = 10;
            while(muxFlg == false && eosThread != true)
            {
                try {
                    Thread.sleep(1000);
                    errTimeout--;
                    if(errTimeout < 0)
                        releaseResource();
                    Log.d(TAG, "Waiting video track ready " + audioPath);
                } catch(InterruptedException  ex) {
                }
            }
            flag = 18;
        }

        @Override
        public void onError(MediaCodec codec, MediaCodec.CodecException e)
        {
            mEncodeErr = true;
            e.printStackTrace();
        }
        }, handle);
    }
    
    private static void dumpFile(String fileName, byte[] data) {
        FileOutputStream outStream;
        try {
            Log.v(TAG, "output will be saved as " + fileName);
            outStream = new FileOutputStream(fileName, true);
        } catch (IOException ioe) {
            throw new RuntimeException("Unable to create debug output file " + fileName, ioe);
        }

        try {
            outStream.write(data);
            outStream.close();
        } catch (IOException ioe) {
            throw new RuntimeException("failed writing data to file " + fileName, ioe);
        }
    }

    public void stopThread() {
        eosThread = true;
        Log.d(TAG, "received stop CMD " + audioPath);
    }
}
