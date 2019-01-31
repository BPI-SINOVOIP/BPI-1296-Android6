package com.realtek.record;


import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.os.IBinder;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.Toast;

/**
 *  TopFloatService class
 *  used to start or stop Setting panel
 */
public class TopFloatService extends Service {
    private static final String TAG = "TopFloatService";
    
    public static boolean mServiceIsAlive = false;
	private WindowManager mWindowManager = null;
	private WindowManager.LayoutParams mBallWmParams = null;
	private View mBallView;
    private View mSettingPanelView;
	private float mTouchStartX;
	private float mTouchStartY;
	private float x;
	private float y;
	private Button mFloatButton;
    private Button mToggleButton;
	private boolean ismoving = false;
    private SettingPanel mSettingPanel = null;
    
	@Override
	public void onCreate() {
        Log.d(TAG, "onCreate");
		super.onCreate();
        mServiceIsAlive = true;

		setFloatingBall();
	}

	/**
	 *  create & show Floating-ball View
	 */
	private void setFloatingBall() {
        Log.d(TAG, "setFloatingBall");
        
		// load Floating-ball's layout
        LayoutInflater li = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		mBallView = li.inflate(R.layout.floatball, null);
		mFloatButton = (Button)mBallView.findViewById(R.id.float_image);                
        
		mWindowManager = (WindowManager) getApplicationContext().getSystemService("window");
		mBallWmParams =  ((MyApplication) getApplication()).getMywmParams();
		mBallWmParams.type = WindowManager.LayoutParams.TYPE_PHONE;
		mBallWmParams.flags |= WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
		mBallWmParams.gravity = Gravity.LEFT | Gravity.TOP;
		mBallWmParams.x = 0;
		mBallWmParams.y = 0;
		mBallWmParams.width = WindowManager.LayoutParams.WRAP_CONTENT;
		mBallWmParams.height = WindowManager.LayoutParams.WRAP_CONTENT;
		mBallWmParams.format = PixelFormat.RGBA_8888;
		// add View to show
		mWindowManager.addView(mBallView, mBallWmParams);
		// register listener
		mFloatButton.setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(View v, MotionEvent event) {
				x = event.getRawX();
				y = event.getRawY(); 
				switch (event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					ismoving = false;
					mTouchStartX = (int)event.getX();
					mTouchStartY = (int)event.getY();
					break;
				case MotionEvent.ACTION_MOVE:
					ismoving = true;
					updateViewPosition();
					break;
				case MotionEvent.ACTION_UP:
					mTouchStartX = mTouchStartY = 0;
					break;
				}
				// return false if floating-ball is moving
				if(ismoving == false){
					return false;
				}else{
					return true;
				}
			}

		});
		// register button's listener
		mFloatButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
                Log.d(TAG, "mFloatButton::onClick");

                showSettingPanel();              
			}
		});
	}
	
    private void dismissView() {
        Log.d(TAG, "dismissView");
        if(mWindowManager != null && mBallView != null) {
            mWindowManager.removeView(mBallView);
        }
    }
	/**
	 * 更新view的显示位置
	 */
	private void updateViewPosition() {
		mBallWmParams.x = (int) (x - mTouchStartX);
		mBallWmParams.y = (int) (y - mTouchStartY);
		mWindowManager.updateViewLayout(mBallView, mBallWmParams);
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        super.onDestroy();
        dismissView();
        mServiceIsAlive = false;
        if(PIPRecordService.mServiceIsAlive) {
            //stop service
            Intent serviceIntent = new Intent(this, com.realtek.record.PIPRecordService.class);
            this.stopService(serviceIntent);
            Toast.makeText(this, "Stop PIP Record !", Toast.LENGTH_SHORT).show();            
        }
    }
    
    void showSettingPanel() {
        Log.d(TAG, "showSettingPanel");
        if(!PIPRecordService.mServiceIsAlive) {
            // load SettingPanel loyout
            LayoutInflater li = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mSettingPanelView = (LinearLayout)li.inflate(R.layout.settingpanel, null);
            // create & show dialog
            AlertDialog.Builder builder = new AlertDialog.Builder(this, R.style.Translucent_NoTitle);
            builder.setView(mSettingPanelView);
            AlertDialog dialog = builder.create();
            dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
            dialog.setCanceledOnTouchOutside(false);
                
            mSettingPanel = new SettingPanel(mSettingPanelView, dialog);
            mSettingPanel.initPanel();
            dialog.show();
        } 
        else {
            //stop service
            Intent serviceIntent = new Intent(this, com.realtek.record.PIPRecordService.class);
            this.stopService(serviceIntent);
            Toast.makeText(this, "Stop PIP Record !", Toast.LENGTH_SHORT).show();
        }
    }
}
