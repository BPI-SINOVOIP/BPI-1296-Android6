/*
 * Copyright (C) 2015 The Android Open Source Project
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
package com.android.cts.preconditions;

import android.app.KeyguardManager;
import android.content.Context;
import android.os.Environment;
import android.test.AndroidTestCase;

/**
 * An AndroidTestCase class to verify that device-side preconditions are met for CTS
 */
public class PreconditionsTest extends AndroidTestCase {

    /**
     * Test if device has no screen lock
     * @throws Exception
     */
    public void testScreenUnlocked() throws Exception {
        KeyguardManager km =
                (KeyguardManager) getContext().getSystemService(Context.KEYGUARD_SERVICE);
        assertFalse("Device must have screen lock disabled", km.isDeviceSecure());
    }

    /**
     * Test if device has accessible external storage
     * @throws Exception
     */
    public void testExternalStoragePresent() throws Exception {
        String state = Environment.getExternalStorageState();
        assertTrue("Device must have writable external storage mounted in order to run CTS",
                Environment.MEDIA_MOUNTED.equals(state));
    }

}
