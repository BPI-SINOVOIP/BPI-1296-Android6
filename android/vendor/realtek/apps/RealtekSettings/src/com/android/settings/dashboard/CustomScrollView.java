package com.android.settings.dashboard;

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.SoundEffectConstants;
import android.widget.ScrollView;

public class CustomScrollView extends ScrollView {

    public CustomScrollView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public boolean dispatchKeyEvent (KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) { 
            if(event.getKeyCode() == KeyEvent.KEYCODE_DPAD_UP) {
                playSoundEffect(SoundEffectConstants.NAVIGATION_UP);
            } else if (event.getKeyCode() == KeyEvent.KEYCODE_DPAD_DOWN) {
                playSoundEffect(SoundEffectConstants.NAVIGATION_DOWN);
            }
        }
        return super.dispatchKeyEvent(event);
    }    
}
