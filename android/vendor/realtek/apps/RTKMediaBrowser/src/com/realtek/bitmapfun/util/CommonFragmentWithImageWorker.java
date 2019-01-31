package com.realtek.bitmapfun.util;

import com.realtek.bitmapfun.util.*;
import com.rtk.mediabrowser.R;

import android.app.Activity;
import android.os.Bundle;
import android.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;

/**
 * @author Eugene Popovich
 */
public abstract class CommonFragmentWithImageWorker extends Fragment {
    protected ImageWorker mImageWorker;
    private static final String TAG = "CmnFragWithImageWorker";

    protected abstract void initImageWorker();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initImageWorker();
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }
    
    
    @Override
    public void onDestroyView() {
    	
    	Log.d(TAG,"destory mImageWorker:");
    	
    	
    	
        super.onDestroyView();
        if (mImageWorker != null && mImageWorker.getImageCache() != null)
        {
        	Log.d(TAG,"destory mImageWorker:"+mImageWorker.toString());
            mImageWorker.getImageCache().clearMemoryCache();
            mImageWorker = null;
            //System.gc();
        }
    }
    
    @Override
    public void onResume()
    {
    	if (mImageWorker != null)
        {
            mImageWorker.setExitTasksEarly(false);
        }
        else
        {
        	initImageWorker();
        }	
        super.onResume();
       
    }

    @Override
    public void onPause()
    {
        super.onPause();
        if (mImageWorker != null)
        {
            mImageWorker.setExitTasksEarly(true);
        }
        
        if (mImageWorker != null && mImageWorker.getImageCache() != null)
        {
        	Log.d(TAG,"destory mImageWorker:"+mImageWorker.toString());
            mImageWorker.getImageCache().clearMemoryCache();
            mImageWorker = null;
            //System.gc();
        }
    }
}
