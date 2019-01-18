/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.widget;

import android.content.Context;
import android.graphics.PixelFormat;
import android.media.AudioManager;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityNodeInfo;
import android.widget.SeekBar.OnSeekBarChangeListener;

import com.android.internal.policy.PhoneWindow;

import java.util.Formatter;
import java.util.Locale;

import android.widget.Toast;
import android.app.AlertDialog;
import android.widget.EditText;
import android.content.DialogInterface;

/**
 * A view containing controls for a MediaPlayer. Typically contains the
 * buttons like "Play/Pause", "Rewind", "Fast Forward" and a progress
 * slider. It takes care of synchronizing the controls with the state
 * of the MediaPlayer.
 * <p>
 * The way to use this class is to instantiate it programatically.
 * The RtkMediaController will create a default set of controls
 * and put them in a window floating above your application. Specifically,
 * the controls will float above the view specified with setAnchorView().
 * The window will disappear if left idle for three seconds and reappear
 * when the user touches the anchor view.
 * <p>
 * Functions like show() and hide() have no effect when RtkMediaController
 * is created in an xml layout.
 * 
 * RtkMediaController will hide and
 * show the buttons according to these rules:
 * <ul>
 * <li> The "previous" and "next" buttons are hidden until setPrevNextListeners()
 *   has been called
 * <li> The "previous" and "next" buttons are visible but disabled if
 *   setPrevNextListeners() was called with null listeners
 * <li> The "rewind" and "fastforward" buttons are shown unless requested
 *   otherwise by using the RtkMediaController(Context, boolean) constructor
 *   with the boolean set to false
 * </ul>
 */
public class RtkMediaController extends FrameLayout {
    private String TAG = "RtkMediaController";
    private MediaPlayerControl  mPlayer;
    private Context             mContext;
    private View                mAnchor;
    private View                mRoot;
    private WindowManager       mWindowManager;
    private Window              mWindow;
    private View                mDecor;
    private WindowManager.LayoutParams mDecorLayoutParams;
    private ProgressBar         mProgress;
    private TextView            mEndTime, mCurrentTime;
    private boolean             mShowing;
    private boolean             mDragging;
    private static final int    sDefaultTimeout = 10000;
    private static final int    FADE_OUT = 1;
    private static final int    SHOW_PROGRESS = 2;
    private boolean             mUseFastForward;
    private boolean             mFromXml;
    private boolean             mListenersSet;
    private View.OnClickListener mNextListener, mPrevListener;
    StringBuilder               mFormatBuilder;
    Formatter                   mFormatter;
    private ImageButton         mPauseButton;
    private ImageButton         mFfwdButton;
    private ImageButton         mRewButton;
    private ImageButton         mNextButton;
    private ImageButton         mPrevButton;
    private Button         mGetSubtitle;
    private Button         mChangeSubtitle;
    private Button         mSetAutoSubtitle;
    private Button         mSetExtSubtitlePath;
    private Button         mGetAudioTrack;
    private Button         mChangeAudioTrack;
    private Button         mTestSetGetNavProperty;
    private Button         mNavControlUP;
    private Button         mNavControlDOWN;
    private Button         mNavControlLEFT;
    private Button         mNavControlRIGHT;
    private Button         mNavControlENTER;
    private int                mTotalSubtitleNum=0;
    private int                mCurrentSubtitle=0;
    private int                mTotalAudioTrackNum=0;
    private int                mCurrentAudioTrack=0;
    private int                mSpeed=0;
    private int                mAutoScanSubtitle=1;

    public static class retSubtitle{
        public int iTotalStreamsNum;
        public int iCurrentStream;
        public int iLangCode;
        public String retInfo;
    }

    public static class retAudioTrack{
        public int iTotalStreamsNum;
        public int iCurrentStream;
        public int iAudioType;
        public int iChannelNum;
        public int iBitsPerSample;
        public int iSampleRate;
        public int iPid;
        public String retInfo;
    }

    public RtkMediaController(Context context, AttributeSet attrs) {
        super(context, attrs);
        mRoot = this;
        mContext = context;
        mUseFastForward = true;
        mFromXml = true;
    }

    @Override
    public void onFinishInflate() {
        if (mRoot != null)
            initControllerView(mRoot);
    }

    public RtkMediaController(Context context, boolean useFastForward) {
        super(context);
        mContext = context;
        mUseFastForward = useFastForward;
        initFloatingWindowLayout();
        initFloatingWindow();
    }

    public RtkMediaController(Context context) {
        this(context, true);
    }

    private void initFloatingWindow() {
        mWindowManager = (WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE);
        mWindow = new PhoneWindow(mContext);
        mWindow.setWindowManager(mWindowManager, null, null);
        mWindow.requestFeature(Window.FEATURE_NO_TITLE);
        mDecor = mWindow.getDecorView();
        mDecor.setOnTouchListener(mTouchListener);
        mWindow.setContentView(this);
        mWindow.setBackgroundDrawableResource(android.R.color.transparent);
        
        // While the media controller is up, the volume control keys should
        // affect the media stream type
        mWindow.setVolumeControlStream(AudioManager.STREAM_MUSIC);

        setFocusable(true);
        setFocusableInTouchMode(true);
        setDescendantFocusability(ViewGroup.FOCUS_AFTER_DESCENDANTS);
        requestFocus();
    }

    // Allocate and initialize the static parts of mDecorLayoutParams. Must
    // also call updateFloatingWindowLayout() to fill in the dynamic parts
    // (y and width) before mDecorLayoutParams can be used.
    private void initFloatingWindowLayout() {
        mDecorLayoutParams = new WindowManager.LayoutParams();
        WindowManager.LayoutParams p = mDecorLayoutParams;
        p.gravity = Gravity.TOP | Gravity.LEFT;
        p.height = LayoutParams.WRAP_CONTENT;
        p.x = 0;
        p.format = PixelFormat.TRANSLUCENT;
        p.type = WindowManager.LayoutParams.TYPE_APPLICATION_PANEL;
        p.flags |= WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM
                | WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                | WindowManager.LayoutParams.FLAG_SPLIT_TOUCH;
        //p.token = null;
        p.windowAnimations = 0; // android.R.style.DropDownAnimationDown;
    }

    // Update the dynamic parts of mDecorLayoutParams
    // Must be called with mAnchor != NULL.
    private void updateFloatingWindowLayout() {
        int [] anchorPos = new int[2];
        mAnchor.getLocationOnScreen(anchorPos);

        // we need to know the size of the controller so we can properly position it
        // within its space
        mDecor.measure(MeasureSpec.makeMeasureSpec(mAnchor.getWidth(), MeasureSpec.AT_MOST),
                MeasureSpec.makeMeasureSpec(mAnchor.getHeight(), MeasureSpec.AT_MOST));

        WindowManager.LayoutParams p = mDecorLayoutParams;
        p.width = mAnchor.getWidth();
        p.x = anchorPos[0] + (mAnchor.getWidth() - p.width) / 2;
        p.y = anchorPos[1] + mAnchor.getHeight() - mDecor.getMeasuredHeight();
    }

    // This is called whenever mAnchor's layout bound changes
    private OnLayoutChangeListener mLayoutChangeListener =
            new OnLayoutChangeListener() {
        public void onLayoutChange(View v, int left, int top, int right,
                int bottom, int oldLeft, int oldTop, int oldRight,
                int oldBottom) {
            updateFloatingWindowLayout();
            if (mShowing) {
                mWindowManager.updateViewLayout(mDecor, mDecorLayoutParams);
            }
        }
    };

    private OnTouchListener mTouchListener = new OnTouchListener() {
        public boolean onTouch(View v, MotionEvent event) {
            if (event.getAction() == MotionEvent.ACTION_DOWN) {
                if (mShowing) {
                    hide();
                }
            }
            return false;
        }
    };
    
    public void setMediaPlayer(MediaPlayerControl player) {
        mPlayer = player;
        mDecorLayoutParams.token = ((View)mPlayer).getWindowToken();
        updatePausePlay();
    }

    /**
     * Set the view that acts as the anchor for the control view.
     * This can for example be a VideoView, or your Activity's main view.
     * When VideoView calls this method, it will use the VideoView's parent
     * as the anchor.
     * @param view The view to which to anchor the controller when it is visible.
     */
    public void setAnchorView(View view) {
        if (mAnchor != null) {
            mAnchor.removeOnLayoutChangeListener(mLayoutChangeListener);
        }
        mAnchor = view;
        if (mAnchor != null) {
            mAnchor.addOnLayoutChangeListener(mLayoutChangeListener);
        }

        FrameLayout.LayoutParams frameParams = new FrameLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
        );

        removeAllViews();
        View v = makeControllerView();
        addView(v, frameParams);
    }

    /**
     * Create the view that holds the widgets that control playback.
     * Derived classes can override this to create their own.
     * @return The controller view.
     * @hide This doesn't work as advertised
     */
    protected View makeControllerView() {
        LayoutInflater inflate = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mRoot = inflate.inflate(com.android.internal.R.layout.rtk_media_controller, null);

        initControllerView(mRoot);

        return mRoot;
    }

    private void initControllerView(View v) {

        Log.d(TAG, "initControllerView");

        
        mPauseButton = (ImageButton) v.findViewById(com.android.internal.R.id.pause);
        if (mPauseButton != null) {
            mPauseButton.requestFocus();
            mPauseButton.setOnClickListener(mPauseListener);
        }

        mFfwdButton = (ImageButton) v.findViewById(com.android.internal.R.id.ffwd);
        if (mFfwdButton != null) {
            mFfwdButton.setOnClickListener(mFfwdListener);
            if (!mFromXml) {
                mFfwdButton.setVisibility(mUseFastForward ? View.VISIBLE : View.GONE);
            }
        }

        mRewButton = (ImageButton) v.findViewById(com.android.internal.R.id.rew);
        if (mRewButton != null) {
            mRewButton.setOnClickListener(mRewListener);
            if (!mFromXml) {
                mRewButton.setVisibility(mUseFastForward ? View.VISIBLE : View.GONE);
            }
        }

        // By default these are hidden. They will be enabled when setPrevNextListeners() is called 
        mNextButton = (ImageButton) v.findViewById(com.android.internal.R.id.next);
        if (mNextButton != null && !mFromXml && !mListenersSet) {
            mNextButton.setVisibility(View.GONE);
        }
        mPrevButton = (ImageButton) v.findViewById(com.android.internal.R.id.prev);
        if (mPrevButton != null && !mFromXml && !mListenersSet) {
            mPrevButton.setVisibility(View.GONE);
        }

        mGetSubtitle = (Button) v.findViewById(com.android.internal.R.id.get_subtitle);
        if (mGetSubtitle != null) {
            mGetSubtitle.setOnClickListener(mGetSubtitleListener);
            if (!mFromXml) {
		  mGetSubtitle.setText("Get Subtitle");
                mGetSubtitle.setVisibility(View.VISIBLE);
            }
        }

        mChangeSubtitle = (Button) v.findViewById(com.android.internal.R.id.change_subtitle);
        if (mChangeSubtitle != null) {
            mChangeSubtitle.setOnClickListener(mChangeSubtitleListener);
            if (!mFromXml) {
		  mChangeSubtitle.setText("Change Subtitle");
                mChangeSubtitle.setVisibility(View.VISIBLE);
            }
        }

        mSetAutoSubtitle = (Button) v.findViewById(com.android.internal.R.id.set_autoscan_subtitle);
        if (mSetAutoSubtitle != null) {
            mSetAutoSubtitle.setOnClickListener(mSetAutoSubtitleListener);
            if (!mFromXml) {
		  mSetAutoSubtitle.setText("Scan Subtitle");
                mSetAutoSubtitle.setVisibility(View.VISIBLE);
            }
        }
        mSetExtSubtitlePath = (Button) v.findViewById(com.android.internal.R.id.set_ext_subtitle_path);
        if (mSetExtSubtitlePath != null) {
            mSetExtSubtitlePath.setOnClickListener(mSetExtSubtitlePathListener);
            if (!mFromXml) {
		  mSetExtSubtitlePath.setText("SetSub Path");
                mSetExtSubtitlePath.setVisibility(View.VISIBLE);
            }
        }
        mGetAudioTrack = (Button) v.findViewById(com.android.internal.R.id.get_audio_track);
        if (mGetAudioTrack != null) {
            mGetAudioTrack.setOnClickListener(mGetAudioTrackListener);
            if (!mFromXml) {
		  mGetAudioTrack.setText("Get Audio");
                mGetAudioTrack.setVisibility(View.VISIBLE);
            }
        }

        mChangeAudioTrack = (Button) v.findViewById(com.android.internal.R.id.change_audio_track);
        if (mChangeAudioTrack != null) {
            mChangeAudioTrack.setOnClickListener(mChangeAudioTrackListener);
            if (!mFromXml) {
		  mChangeAudioTrack.setText("Change Audio");
                mChangeAudioTrack.setVisibility(View.VISIBLE);
            }
        }

        mTestSetGetNavProperty = (Button) v.findViewById(com.android.internal.R.id.test_set_get_nav_property);
        if (mTestSetGetNavProperty != null) {
            mTestSetGetNavProperty.setOnClickListener(mTestSetGetNavPropertyListener);
            if (!mFromXml) {
		  mTestSetGetNavProperty.setText("S/G Nav Prop");
                mTestSetGetNavProperty.setVisibility(View.VISIBLE);
            }
        }
        mNavControlUP = (Button) v.findViewById(com.android.internal.R.id.up);
        if (mNavControlUP != null) {
            mNavControlUP.setOnClickListener(mNavControlUPListener);
            if (!mFromXml) {
                mNavControlUP.setText("UP");
                mNavControlUP.setVisibility(View.VISIBLE);
            }
        }
        mNavControlDOWN = (Button) v.findViewById(com.android.internal.R.id.down);
        if (mNavControlDOWN != null) {
            mNavControlDOWN.setOnClickListener(mNavControlDOWNListener);
            if (!mFromXml) {
                mNavControlDOWN.setText("DOWN");
                mNavControlDOWN.setVisibility(View.VISIBLE);
            }
        }
        mNavControlLEFT = (Button) v.findViewById(com.android.internal.R.id.left);
        if (mNavControlLEFT != null) {
            mNavControlLEFT.setOnClickListener(mNavControlLEFTListener);
            if (!mFromXml) {
                mNavControlLEFT.setText("LEFT");
                mNavControlLEFT.setVisibility(View.VISIBLE);
            }
        }
        mNavControlRIGHT = (Button) v.findViewById(com.android.internal.R.id.right);
        if (mNavControlRIGHT != null) {
            mNavControlRIGHT.setOnClickListener(mNavControlRIGHTListener);
            if (!mFromXml) {
                mNavControlRIGHT.setText("RIGHT");
                mNavControlRIGHT.setVisibility(View.VISIBLE);
            }
        }
        mNavControlENTER = (Button) v.findViewById(com.android.internal.R.id.enter);
        if (mNavControlENTER != null) {
            mNavControlENTER.setOnClickListener(mNavControlENTERListener);
            if (!mFromXml) {
                mNavControlENTER.setText("ENTER");
                mNavControlENTER.setVisibility(View.VISIBLE);
            }
        }

        mProgress = (ProgressBar) v.findViewById(com.android.internal.R.id.mediacontroller_progress);
        if (mProgress != null) {
            if (mProgress instanceof SeekBar) {
                SeekBar seeker = (SeekBar) mProgress;
                seeker.setOnSeekBarChangeListener(mSeekListener);
            }
            mProgress.setMax(1000);
        }

        mEndTime = (TextView) v.findViewById(com.android.internal.R.id.time);
        mCurrentTime = (TextView) v.findViewById(com.android.internal.R.id.time_current);
        mFormatBuilder = new StringBuilder();
        mFormatter = new Formatter(mFormatBuilder, Locale.getDefault());

        installPrevNextListeners();
    }

    /**
     * Show the controller on screen. It will go away
     * automatically after 3 seconds of inactivity.
     */
    public void show() {
        show(sDefaultTimeout);
    }

    /**
     * Disable pause or seek buttons if the stream cannot be paused or seeked.
     * This requires the control interface to be a MediaPlayerControlExt
     */
    private void disableUnsupportedButtons() {
        try {
            if (mPauseButton != null && !mPlayer.canPause()) {
                mPauseButton.setEnabled(false);
            }
            if (mRewButton != null && !mPlayer.canSeekBackward()) {
                mRewButton.setEnabled(false);
            }
            if (mFfwdButton != null && !mPlayer.canSeekForward()) {
                mFfwdButton.setEnabled(false);
            }
        } catch (IncompatibleClassChangeError ex) {
            // We were given an old version of the interface, that doesn't have
            // the canPause/canSeekXYZ methods. This is OK, it just means we
            // assume the media can be paused and seeked, and so we don't disable
            // the buttons.
        }
    }
    
    /**
     * Show the controller on screen. It will go away
     * automatically after 'timeout' milliseconds of inactivity.
     * @param timeout The timeout in milliseconds. Use 0 to show
     * the controller until hide() is called.
     */
    public void show(int timeout) {
        if (!mShowing && mAnchor != null) {
            setProgress();
            if (mPauseButton != null) {
                mPauseButton.requestFocus();
            }
            disableUnsupportedButtons();
            updateFloatingWindowLayout();
            mWindowManager.addView(mDecor, mDecorLayoutParams);
            mShowing = true;
        }
        updatePausePlay();
        
        // cause the progress bar to be updated even if mShowing
        // was already true.  This happens, for example, if we're
        // paused with the progress bar showing the user hits play.
        mHandler.sendEmptyMessage(SHOW_PROGRESS);

        Message msg = mHandler.obtainMessage(FADE_OUT);
        if (timeout != 0) {
            mHandler.removeMessages(FADE_OUT);
            mHandler.sendMessageDelayed(msg, timeout);
        }
    }
    
    public boolean isShowing() {
        return mShowing;
    }

    /**
     * Remove the controller from the screen.
     */
    public void hide() {
        if (mAnchor == null)
            return;

        if (mShowing) {
            try {
                mHandler.removeMessages(SHOW_PROGRESS);
                mWindowManager.removeView(mDecor);
            } catch (IllegalArgumentException ex) {
                Log.w(TAG, "already removed");
            }
            mShowing = false;
        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            int pos;
            switch (msg.what) {
                case FADE_OUT:
                    hide();
                    break;
                case SHOW_PROGRESS:
                    pos = setProgress();
                    if (!mDragging && mShowing && mPlayer.isPlaying()) {
                        msg = obtainMessage(SHOW_PROGRESS);
                        sendMessageDelayed(msg, 1000 - (pos % 1000));
                    }
                    break;
            }
        }
    };

    private String stringForTime(int timeMs) {
        int totalSeconds = timeMs / 1000;

        int seconds = totalSeconds % 60;
        int minutes = (totalSeconds / 60) % 60;
        int hours   = totalSeconds / 3600;

        mFormatBuilder.setLength(0);
        if (hours > 0) {
            return mFormatter.format("%d:%02d:%02d", hours, minutes, seconds).toString();
        } else {
            return mFormatter.format("%02d:%02d", minutes, seconds).toString();
        }
    }

    private int setProgress() {
        if (mPlayer == null || mDragging) {
            return 0;
        }
        int position = mPlayer.getCurrentPosition();
        int duration = mPlayer.getDuration();
        if (mProgress != null) {
            if (duration > 0) {
                // use long to avoid overflow
                long pos = 1000L * position / duration;
                mProgress.setProgress( (int) pos);
            }
            int percent = mPlayer.getBufferPercentage();
            mProgress.setSecondaryProgress(percent * 10);
        }

        if (mEndTime != null)
            mEndTime.setText(stringForTime(duration));
        if (mCurrentTime != null)
            mCurrentTime.setText(stringForTime(position));

        return position;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        show(sDefaultTimeout);
        return true;
    }

    @Override
    public boolean onTrackballEvent(MotionEvent ev) {
        show(sDefaultTimeout);
        return false;
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        int keyCode = event.getKeyCode();
        final boolean uniqueDown = event.getRepeatCount() == 0
                && event.getAction() == KeyEvent.ACTION_DOWN;
        if (keyCode ==  KeyEvent.KEYCODE_HEADSETHOOK
                || keyCode == KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE
                || keyCode == KeyEvent.KEYCODE_SPACE) {
            if (uniqueDown) {
                doPauseResume();
                show(sDefaultTimeout);
                if (mPauseButton != null) {
                    mPauseButton.requestFocus();
                }
            }
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_MEDIA_PLAY) {
            if (uniqueDown && !mPlayer.isPlaying()) {
                mPlayer.start();
                updatePausePlay();
                show(sDefaultTimeout);
            }
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_MEDIA_STOP
                || keyCode == KeyEvent.KEYCODE_MEDIA_PAUSE) {
            if (uniqueDown && mPlayer.isPlaying()) {
                mPlayer.pause();
                updatePausePlay();
                show(sDefaultTimeout);
            }
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN
                || keyCode == KeyEvent.KEYCODE_VOLUME_UP
                || keyCode == KeyEvent.KEYCODE_VOLUME_MUTE
                || keyCode == KeyEvent.KEYCODE_CAMERA) {
            // don't show the controls for volume adjustment
            return super.dispatchKeyEvent(event);
        } else if (keyCode == KeyEvent.KEYCODE_BACK || keyCode == KeyEvent.KEYCODE_MENU) {
            if (uniqueDown) {
                hide();
            }
            Log.d(TAG, "keyCode = " + keyCode);
            return true;
        }

        show(sDefaultTimeout);
        return super.dispatchKeyEvent(event);
    }

    private View.OnClickListener mPauseListener = new View.OnClickListener() {
        public void onClick(View v) {
            doPauseResume();
            show(sDefaultTimeout);
        }
    };

    private void updatePausePlay() {
        if (mRoot == null || mPauseButton == null)
            return;

        if (mPlayer.isPlaying()) {
            mPauseButton.setImageResource(com.android.internal.R.drawable.ic_media_pause);
        } else {
            mPauseButton.setImageResource(com.android.internal.R.drawable.ic_media_play);
        }
    }

    private void doPauseResume() {
        if (mPlayer.isPlaying()) {
            mPlayer.pause();
        } else {
            mPlayer.start();
        }
        updatePausePlay();
    }

    // There are two scenarios that can trigger the seekbar listener to trigger:
    //
    // The first is the user using the touchpad to adjust the posititon of the
    // seekbar's thumb. In this case onStartTrackingTouch is called followed by
    // a number of onProgressChanged notifications, concluded by onStopTrackingTouch.
    // We're setting the field "mDragging" to true for the duration of the dragging
    // session to avoid jumps in the position in case of ongoing playback.
    //
    // The second scenario involves the user operating the scroll ball, in this
    // case there WON'T BE onStartTrackingTouch/onStopTrackingTouch notifications,
    // we will simply apply the updated position without suspending regular updates.
    private OnSeekBarChangeListener mSeekListener = new OnSeekBarChangeListener() {
        public void onStartTrackingTouch(SeekBar bar) {
            show(3600000);

            mDragging = true;

            // By removing these pending progress messages we make sure
            // that a) we won't update the progress while the user adjusts
            // the seekbar and b) once the user is done dragging the thumb
            // we will post one of these messages to the queue again and
            // this ensures that there will be exactly one message queued up.
            mHandler.removeMessages(SHOW_PROGRESS);
        }

        public void onProgressChanged(SeekBar bar, int progress, boolean fromuser) {
            if (!fromuser) {
                // We're not interested in programmatically generated changes to
                // the progress bar's position.
                return;
            }

            long duration = mPlayer.getDuration();
            long newposition = (duration * progress) / 1000L;
            mPlayer.seekTo( (int) newposition);
            if (mCurrentTime != null)
                mCurrentTime.setText(stringForTime( (int) newposition));
        }

        public void onStopTrackingTouch(SeekBar bar) {
            mDragging = false;
            setProgress();
            updatePausePlay();
            show(sDefaultTimeout);

            // Ensure that progress is properly updated in the future,
            // the call to show() does not guarantee this because it is a
            // no-op if we are already showing.
            mHandler.sendEmptyMessage(SHOW_PROGRESS);
        }
    };

    @Override
    public void setEnabled(boolean enabled) {
        if (mPauseButton != null) {
            mPauseButton.setEnabled(enabled);
        }
        if (mFfwdButton != null) {
            mFfwdButton.setEnabled(enabled);
        }
        if (mRewButton != null) {
            mRewButton.setEnabled(enabled);
        }
        if (mNextButton != null) {
            mNextButton.setEnabled(enabled && mNextListener != null);
        }
        if (mPrevButton != null) {
            mPrevButton.setEnabled(enabled && mPrevListener != null);
        }
        if (mProgress != null) {
            mProgress.setEnabled(enabled);
        }
        disableUnsupportedButtons();
        super.setEnabled(enabled);
    }

    @Override
    public void onInitializeAccessibilityEvent(AccessibilityEvent event) {
        super.onInitializeAccessibilityEvent(event);
        event.setClassName(RtkMediaController.class.getName());
    }

    @Override
    public void onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo info) {
        super.onInitializeAccessibilityNodeInfo(info);
        info.setClassName(RtkMediaController.class.getName());
    }

    private View.OnClickListener mRewListener = new View.OnClickListener() {
        public void onClick(View v) {
            if(mSpeed == 0 || mSpeed == 1)
                mSpeed = -1;
            else if(mSpeed > 0 || mSpeed <= -8)
                mSpeed = 0;
            else
                mSpeed *= 2;
            mPlayer.fastrewind(-1 * mSpeed);
            Log.i(TAG, "fastrewind");
/*
            int pos = mPlayer.getCurrentPosition();
            pos -= 5000; // milliseconds
            mPlayer.seekTo(pos);
            setProgress();

            show(sDefaultTimeout);
*/
        }
    };

    private View.OnClickListener mFfwdListener = new View.OnClickListener() {
        public void onClick(View v) {
            if(mSpeed < 0)
                mSpeed = 1;
            else if(mSpeed == 0)
                mSpeed = 2;
            else if(mSpeed >= 8)
                mSpeed = 1;
            else
                mSpeed *= 2;
            mPlayer.fastforward(mSpeed);
            Log.i(TAG, "fastforward");
/*
            int pos = mPlayer.getCurrentPosition();
            pos += 15000; // milliseconds
            mPlayer.seekTo(pos);
            setProgress();

            show(sDefaultTimeout);
*/
        }
    };

    private View.OnClickListener mGetSubtitleListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "Get subtitle");
            retSubtitle result=mPlayer.getSubtitleInfo();
            if(result.iTotalStreamsNum>0)
            {
                mTotalSubtitleNum=result.iTotalStreamsNum;
                mCurrentSubtitle=result.iCurrentStream;
                Toast.makeText(mContext,result.retInfo,Toast.LENGTH_SHORT).show();
            }
        }
    };

    private View.OnClickListener mChangeSubtitleListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "change subtitle");
            if(mTotalSubtitleNum>0)
            {
                if(mCurrentSubtitle<1 || mCurrentSubtitle>=mTotalSubtitleNum)
                    mCurrentSubtitle=1;
                else
                    mCurrentSubtitle++;
                mPlayer.setSubtitleInfo(mCurrentSubtitle,1,0,5,17,0,0);
                Toast.makeText(mContext,"set subtitle "+mCurrentSubtitle,Toast.LENGTH_SHORT).show();
            }
            else
            {
                Toast.makeText(mContext,"no subtitle in this video",Toast.LENGTH_SHORT).show();
            }
        }
    };

    private View.OnClickListener mSetAutoSubtitleListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "scan subtitle");
            if(mAutoScanSubtitle == 1)
                mAutoScanSubtitle=0;
            else
                mAutoScanSubtitle=1;
            mPlayer.setAutoScanExtSubtitle(mAutoScanSubtitle);
            Toast.makeText(mContext,"autoscan subtitle mode change to "+mAutoScanSubtitle,Toast.LENGTH_SHORT).show();
        }
    };

    private View.OnClickListener mSetExtSubtitlePathListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "set ext subtitle path");
            AlertDialog.Builder alert = new AlertDialog.Builder(mContext);
            alert.setTitle("Title"); 
            alert.setMessage("Message");  
            // Set an EditText view to get user input  
            final EditText input = new EditText(mContext); 
            input.setText("/data/1.srt");
            alert.setView(input);  
            alert.setPositiveButton("Ok", new DialogInterface.OnClickListener() { 
                public void onClick(DialogInterface dialog, int whichButton) {   
                    String value = input.getText().toString();  
                    mPlayer.setExtSubtitlePath(value);
                    Toast.makeText(mContext,"set ext subtitle path = "+value,Toast.LENGTH_SHORT).show();
                    } 
                });  
            alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {     
                    Toast.makeText(mContext,"cancel to set ext subtitle path",Toast.LENGTH_SHORT).show();
                    } 
                });  
            alert.show();            
//            mPlayer.setExtSubtitlePath("/data/1.srt");
//            Toast.makeText(mContext,"set ext subtitle path = /data/1.srt",Toast.LENGTH_SHORT).show();
        }
    };
    
    private View.OnClickListener mGetAudioTrackListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "Get audio track");
            retAudioTrack result=mPlayer.getAudioTrackInfo(-1);
            if(result.iTotalStreamsNum>0)
            {
                mTotalAudioTrackNum=result.iTotalStreamsNum;
                mCurrentAudioTrack=result.iCurrentStream;
                Toast.makeText(mContext,result.retInfo,Toast.LENGTH_SHORT).show();
            }
        }
    };

    private View.OnClickListener mChangeAudioTrackListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "change audio track");
            if(mTotalAudioTrackNum>0)
            {
                if(mCurrentAudioTrack<1 || mCurrentAudioTrack>=mTotalAudioTrackNum)
                    mCurrentAudioTrack=1;
                else
                    mCurrentAudioTrack++;
                mPlayer.setAudioTrackInfo(mCurrentAudioTrack);
                Toast.makeText(mContext,"set audio track "+mCurrentAudioTrack,Toast.LENGTH_SHORT).show();
            }
            else
            {
                Toast.makeText(mContext,"no audio track in this video",Toast.LENGTH_SHORT).show();
            }
        }
    };

    private View.OnClickListener mTestSetGetNavPropertyListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "test set get nav property");
            int value1 = 113;
            int value2 = 25;
            byte[] inputArray=new byte[] {
                (byte) (value1 >> 24),
                (byte) (value1 >> 16),
                (byte) (value1 >> 8),
                (byte) value1,
                (byte) (value2 >> 24),
                (byte) (value2 >> 16),
                (byte) (value2 >> 8),
                (byte) value2
            };
            
            byte[] outputArray=mPlayer.execSetGetNavProperty(1000,inputArray);
            if(outputArray != null)
            {
                int outValue=outputArray[0] << 24 | (outputArray[1] & 0xFF) << 16 | (outputArray[2] & 0xFF) << 8 | (outputArray[3] & 0xFF);
                Toast.makeText(mContext,"test set get nav property in1 = "+value1+", in2 = "+value2+" ,out = "+
                    outValue+", out length = "+outputArray.length,Toast.LENGTH_SHORT).show();
            }
            else
                Toast.makeText(mContext,"test set get nav property in1 = "+value1+", in2 = "+value2+" ,out = null",Toast.LENGTH_SHORT).show();
        }
    };
    private View.OnClickListener mNavControlUPListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "navi up button");
            mPlayer.deliverNaviControlCmd(0, 0);
        }
    };
    private View.OnClickListener mNavControlDOWNListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "navi down button");
            mPlayer.deliverNaviControlCmd(1, 0);
        }
    };
    private View.OnClickListener mNavControlLEFTListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "navi left button");
            mPlayer.deliverNaviControlCmd(2, 0);
        }
    };
    private View.OnClickListener mNavControlRIGHTListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "navi right button");
            mPlayer.deliverNaviControlCmd(3, 0);
        }
    };
    private View.OnClickListener mNavControlENTERListener = new View.OnClickListener() {
        public void onClick(View v) {
            Log.i(TAG, "navi enter button");
            mPlayer.deliverNaviControlCmd(4, 0);
        }
    };

    private void installPrevNextListeners() {
        if (mNextButton != null) {
            mNextButton.setOnClickListener(mNextListener);
            mNextButton.setEnabled(mNextListener != null);
        }

        if (mPrevButton != null) {
            mPrevButton.setOnClickListener(mPrevListener);
            mPrevButton.setEnabled(mPrevListener != null);
        }
    }

    public void setPrevNextListeners(View.OnClickListener next, View.OnClickListener prev) {
        mNextListener = next;
        mPrevListener = prev;
        mListenersSet = true;

        if (mRoot != null) {
            installPrevNextListeners();
            
            if (mNextButton != null && !mFromXml) {
                mNextButton.setVisibility(View.VISIBLE);
            }
            if (mPrevButton != null && !mFromXml) {
                mPrevButton.setVisibility(View.VISIBLE);
            }
        }
    }

    public interface MediaPlayerControl {
        void    start();
        void    pause();
        int     getDuration();
        int     getCurrentPosition();
        void    seekTo(int pos);
        boolean fastforward(int speed);
        boolean fastrewind(int speed);
        retSubtitle getSubtitleInfo();
        void setSubtitleInfo(int streamNum,int enable,int textEncoding,int testColor,int fontSize, int syncTime, int offset);
        void setAutoScanExtSubtitle(int autoScan);
        void setExtSubtitlePath(String path);
        retAudioTrack getAudioTrackInfo(int streamNum);
        void setAudioTrackInfo(int streamNum);
        byte[] execSetGetNavProperty(int propertyID,byte[] inputArray);
        boolean deliverNaviControlCmd(int param1, int param2);
        boolean isPlaying();
        int     getBufferPercentage();
        boolean canPause();
        boolean canSeekBackward();
        boolean canSeekForward();

        /**
         * Get the audio session id for the player used by this VideoView. This can be used to
         * apply audio effects to the audio track of a video.
         * @return The audio session, or 0 if there was an error.
         */
        int     getAudioSessionId();
    }
}
