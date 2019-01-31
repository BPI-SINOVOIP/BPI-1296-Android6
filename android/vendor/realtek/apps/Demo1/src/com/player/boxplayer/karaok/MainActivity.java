/**
 * @Project:
 * @Create Date: ${2018-09-17}
 * @Author: justlink-peter
 * Copyright ® 2018  Inc. All Rights Reserved.
 * 注意：本内容所有权仅限于justlink个人所有,任何单位和个人"未经允许"
 * 禁止拷贝,传播,以及用于其他个人目的或公司的商业用途.
 * 以免带来不必要的麻烦和纠纷.
 * 现在或将来,必须保留本文件的来源.
 */
package com.player.boxplayer.karaok;

import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.graphics.SurfaceTexture;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.TextureView.SurfaceTextureListener;
import com.player.boxplayer.R;

public class MainActivity extends Activity  implements MediaPlayer.OnErrorListener,
                  MediaPlayer.OnCompletionListener, MediaPlayer.OnPreparedListener
{ 
    private final String TAG = "justlink";
    private MediaPlayer mPreMediaPlayer;
	private Surface surface;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_karaok1);
        TextureView textureView=(TextureView) findViewById(R.id.surfaceview1);
        textureView.setSurfaceTextureListener(surfaceTextureListener);
    }

    
	private SurfaceTextureListener surfaceTextureListener = new SurfaceTextureListener() 
    {
        //SurfaceTexture可用
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int width,int height) 
        {
            Log.v(TAG,"onSurfaceTextureAvailable");
            surface=new Surface(surfaceTexture);
            if(mPreMediaPlayer == null)
            {
                mPreMediaPlayer = new MediaPlayer();
                mPreMediaPlayer.setOnErrorListener(MainActivity.this);
                mPreMediaPlayer.setOnPreparedListener(MainActivity.this);
                mPreMediaPlayer.setOnCompletionListener(MainActivity.this);
                mPreMediaPlayer.setVolume((float) 100, (float) 100);
                mPreMediaPlayer.setSurface(surface);
                //mPreMediaPlayer.useRTMediaPlayer(MediaPlayer.FORCE_RT_MEDIAPLAYER);
            }
            else
            {
                mPreMediaPlayer.reset();
            }
            try
            {
                mPreMediaPlayer.setDataSource("/mnt/sdcard/mg/vod/default1.mpg");
                mPreMediaPlayer.prepareAsync();
            }
            catch (Exception e)
            {
                Log.v(TAG,"Exception" + e.toString());;
            }
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width,int height) 
        {   //尺寸改变
            Log.v(TAG,"onSurfaceTextureSizeChanged");
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) 
        {   //销毁
            surface=null;
            mPreMediaPlayer.stop();
            mPreMediaPlayer.release();
            mPreMediaPlayer=null;
            Log.v(TAG,"onSurfaceTextureDestroyed");
            return true;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) 
        {   //更新
            //Log.v(TAG,"onSurfaceTextureUpdated");
        }
    };

    
    protected void onResume() 
    {
        super.onResume();
    }
	
	protected void onPause() 
	{
        super.onPause();
    }

    @Override
    protected void onDestroy() 
    {
        super.onDestroy();
    }

    @Override
    public void onPrepared(MediaPlayer mp)
    {
        Log.v(TAG,"onPrepared");
        mp.start();
    }

    @Override
    public void onCompletion(MediaPlayer mp)
    {
        Log.v(TAG,"onCompletion");
        try
        {
            mPreMediaPlayer.setDataSource("/mnt/sdcard/mg/vod/default1.mpg");
            mPreMediaPlayer.prepareAsync();
        }
        catch (Exception e)
        {
            Log.v(TAG,"Exception" + e.toString());;
        }
    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra)
    {
        Log.v(TAG,"onError");
        mPreMediaPlayer.reset();
        try
        {
            mPreMediaPlayer.setDataSource("/mnt/sdcard/mg/vod/default1.mpg");
            mPreMediaPlayer.prepareAsync();
        }
        catch (Exception e)
        {
            Log.v(TAG,"Exception" + e.toString());;
        }
        return false;
    }    
}



