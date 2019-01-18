/*
 * Copyright (C) 2014 The Android Open Source Project
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>

#include <sys/cdefs.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <linux/fb.h>
#include <linux/kd.h>

#include "minui.h"
#include "graphics.h"

static GRSurface* fbdev_init(minui_backend*);
static GRSurface* fbdev_flip(minui_backend*);
static void fbdev_blank(minui_backend*, bool);
static void fbdev_exit(minui_backend*);

static GRSurface gr_framebuffer[2];
static GRSurface gr_framebuffer_dp[2];

static bool double_buffered;
static GRSurface* gr_draw = NULL;
static GRSurface* gr_draw_dp = NULL;

static int displayed_buffer; // used to illustrate which framebuffer is using

static fb_var_screeninfo vi;
static fb_var_screeninfo vi_dp;

static int fb_fd = -1;
static int fb_fd_dp = -1;

static int SecondScreen =-1;

static minui_backend my_backend = {
    .init = fbdev_init,
    .flip = fbdev_flip,
    .blank = fbdev_blank,
    .exit = fbdev_exit,
};

minui_backend* open_fbdev() {
    return &my_backend;
}

static void fbdev_blank(minui_backend* backend __unused, bool blank)
{
    int ret;

    ret = ioctl(fb_fd, FBIOBLANK, blank ? FB_BLANK_POWERDOWN : FB_BLANK_UNBLANK);
    if (ret < 0)
        perror("ioctl(): blank");
	
    ret = ioctl(fb_fd_dp, FBIOBLANK, blank ? FB_BLANK_POWERDOWN : FB_BLANK_UNBLANK);
    if (ret < 0)
        perror("ioctl(): blank");
}

static void set_displayed_framebuffer(unsigned n)
{
    if (n > 1 || !double_buffered) return;

    vi.yres_virtual = gr_framebuffer[0].height * 2;
    vi.yoffset = n * gr_framebuffer[0].height;
    vi.bits_per_pixel = gr_framebuffer[0].pixel_bytes * 8;
    if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vi) < 0) {
        perror("active fb0 swap failed");
    }

	if(SecondScreen ==1){
		//second screen set
	    vi_dp.yres_virtual = gr_framebuffer_dp[0].height * 2;
	    vi_dp.yoffset = n * gr_framebuffer_dp[0].height;
	    vi_dp.bits_per_pixel = gr_framebuffer_dp[0].pixel_bytes * 8;
	    if (ioctl(fb_fd_dp, FBIOPUT_VSCREENINFO, &vi_dp) < 0) {
	        perror("active fb1 swap failed");
	    }
	}
	
    displayed_buffer = n;
}

static void initializeSecondScreen(){ //second use

	int fd_Second = open("/dev/graphics/fb1",O_RDWR);
	if(fd_Second == -1){
		perror("cannot open fb1,return to normal process\n");
		return ;
	}

	fb_fix_screeninfo fi_dp;	
	
	if (ioctl(fd_Second,FBIOGET_FSCREENINFO, &fi_dp) < 0){
        perror("failed to get fb1 info");
        close(fd_Second);
        return ;
	}
	
	if (ioctl(fd_Second,FBIOGET_VSCREENINFO, &vi_dp) < 0){
        perror("failed to get fb1 info");
        close(fd_Second);
        return ;			
	}
	
    printf("fb1 reports (possibly inaccurate):\n"
           "  vi.bits_per_pixel = %d\n"
           "  vi.red.offset   = %3d   .length = %3d\n"
           "  vi.green.offset = %3d   .length = %3d\n"
           "  vi.blue.offset  = %3d   .length = %3d\n"
           "  vi.xres         = %3d   .yres   = %3d\n"
           "  vi.xres_vir     = %3d   .yres_vir   = %3d\n",
           vi_dp.bits_per_pixel,
           vi_dp.red.offset, vi_dp.red.length,
           vi_dp.green.offset, vi_dp.green.length,
           vi_dp.blue.offset, vi_dp.blue.length,
           vi_dp.xres       , vi_dp.yres       ,
           vi_dp.xres_virtual, vi_dp.yres_virtual);


    void* bits = mmap(0, fi_dp.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_Second, 0);
    if (bits == MAP_FAILED) {
	   perror("failed to mmap framebuffer");
	   close(fd_Second);
	   return ;
   }
	
    memset(bits, 0, fi_dp.smem_len);

    gr_framebuffer_dp[0].width = vi_dp.xres;
    gr_framebuffer_dp[0].height = vi_dp.yres;
    gr_framebuffer_dp[0].row_bytes = fi_dp.line_length;
    gr_framebuffer_dp[0].pixel_bytes = vi_dp.bits_per_pixel / 8;
    gr_framebuffer_dp[0].data = reinterpret_cast<uint8_t*>(bits);
    memset(gr_framebuffer_dp[0].data, 0, gr_framebuffer_dp[0].height * gr_framebuffer_dp[0].row_bytes);

    /* it is sure dp supports double buffer */
        //double_buffered = true;
        perror("double_buffered confirm! \n");

    memcpy(gr_framebuffer_dp+1, gr_framebuffer_dp, sizeof(GRSurface));
	
    gr_framebuffer_dp[1].data = gr_framebuffer_dp[0].data +
                            gr_framebuffer_dp[0].height * gr_framebuffer_dp[0].row_bytes;

   gr_draw_dp = gr_framebuffer_dp+1;

   
   memset(gr_draw_dp->data, 0, gr_draw_dp->height * gr_draw_dp->row_bytes);
   fb_fd_dp = fd_Second;
   SecondScreen = 1;
   //set_displayed_framebuffer(0);
   
   //printf("framebuffer: %d (%d x %d)\n", fb_fd, gr_draw->width, gr_draw->height);
   printf("framebuffer1: %d (%d x %d)\n", fb_fd_dp, gr_draw_dp->width, gr_draw_dp->height);

}

static GRSurface* fbdev_init(minui_backend* backend) {
    int fd = open("/dev/graphics/fb0", O_RDWR);
    if (fd == -1) {
        perror("cannot open fb0");
        return NULL;
    }

    fb_fix_screeninfo fi;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fi) < 0) {
        perror("failed to get fb0 info");
        close(fd);
        return NULL;
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &vi) < 0) {
        perror("failed to get fb0 info");
        close(fd);
        return NULL;
    }


    // We print this out for informational purposes only, but
    // throughout we assume that the framebuffer device uses an RGBX
    // pixel format.  This is the case for every development device I
    // have access to.  For some of those devices (eg, hammerhead aka
    // Nexus 5), FBIOGET_VSCREENINFO *reports* that it wants a
    // different format (XBGR) but actually produces the correct
    // results on the display when you write RGBX.
    //
    // If you have a device that actually *needs* another pixel format
    // (ie, BGRX, or 565), patches welcome...

    printf("fb0 reports (possibly inaccurate):\n"
           "  vi.bits_per_pixel = %d\n"
           "  vi.red.offset   = %3d   .length = %3d\n"
           "  vi.green.offset = %3d   .length = %3d\n"
           "  vi.blue.offset  = %3d   .length = %3d\n"
           "  vi.xres         = %3d   .yres   = %3d\n"
           "  vi.xres_vir     = %3d   .yres_vir   = %3d\n",
           vi.bits_per_pixel,
           vi.red.offset,   vi.red.length,
           vi.green.offset, vi.green.length,
           vi.blue.offset,  vi.blue.length,
           vi.xres       ,  vi.yres       ,
           vi.xres_virtual, vi.yres_virtual);

    void* bits = mmap(0, fi.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (bits == MAP_FAILED) {
        perror("failed to mmap framebuffer");
        close(fd);
        return NULL;
    }

	printf("Length of frame buffer mem : %d\n",fi.smem_len);
	printf("line_length : %d\n",fi.line_length);
	
    memset(bits, 0, fi.smem_len);

    gr_framebuffer[0].width = vi.xres;
    gr_framebuffer[0].height = vi.yres;
    gr_framebuffer[0].row_bytes = fi.line_length;
    gr_framebuffer[0].pixel_bytes = vi.bits_per_pixel / 8;
    gr_framebuffer[0].data = reinterpret_cast<uint8_t*>(bits);
    memset(gr_framebuffer[0].data, 0, gr_framebuffer[0].height * gr_framebuffer[0].row_bytes);

    /* check if we can use double buffering */
    if (vi.yres * fi.line_length * 2 <= fi.smem_len) {
        double_buffered = true;
        perror("double_buffered confirm! \n");

        memcpy(gr_framebuffer+1, gr_framebuffer, sizeof(GRSurface));
        gr_framebuffer[1].data = gr_framebuffer[0].data +
            gr_framebuffer[0].height * gr_framebuffer[0].row_bytes;

        gr_draw = gr_framebuffer+1;

    } else {
        double_buffered = false;

        // Without double-buffering, we allocate RAM for a buffer to
        // draw in, and then "flipping" the buffer consists of a
        // memcpy from the buffer we allocated to the framebuffer.

        gr_draw = (GRSurface*) malloc(sizeof(GRSurface));
        memcpy(gr_draw, gr_framebuffer, sizeof(GRSurface));
        gr_draw->data = (unsigned char*) malloc(gr_draw->height * gr_draw->row_bytes);
        if (!gr_draw->data) {
            perror("failed to allocate in-memory surface");
            return NULL;
        }
    }

    memset(gr_draw->data, 0, gr_draw->height * gr_draw->row_bytes);
    fb_fd = fd;

	
	initializeSecondScreen();
    set_displayed_framebuffer(0);

    printf("framebuffer0: %d (%d x %d)\n", fb_fd,    gr_draw->width,    gr_draw->height);

    fbdev_blank(backend, true);
    fbdev_blank(backend, false);

    return gr_draw;
}

static GRSurface* fbdev_flip(minui_backend* backend __unused) {
    if (double_buffered) {
#if defined(RECOVERY_BGRA)
        // In case of BGRA, do some byte swapping
        unsigned int idx;
        unsigned char tmp;
        unsigned char* ucfb_vaddr = (unsigned char*)gr_draw->data;
        for (idx = 0 ; idx < (gr_draw->height * gr_draw->row_bytes);
                idx += 4) {
            tmp = ucfb_vaddr[idx];
            ucfb_vaddr[idx    ] = ucfb_vaddr[idx + 2];
            ucfb_vaddr[idx + 2] = tmp;
        }
#endif
		if(SecondScreen == 1){
        
			//copy the content of fb0 to fb1 
			// 1-displayed_buffer is the now using buffer
			//gr_draw points to the current framebuffer
			memcpy(gr_framebuffer_dp[1-displayed_buffer].data,
			       gr_framebuffer[1-displayed_buffer].data,
			       gr_draw->height * gr_draw->row_bytes);

		    //switch synchronously
			gr_draw_dp = gr_framebuffer_dp + displayed_buffer;
		}

		// Change gr_draw to point to the buffer currently displayed,
		// then flip the driver so we're displaying the other buffer
		// instead.
		gr_draw = gr_framebuffer + displayed_buffer;
		
		
        set_displayed_framebuffer(1-displayed_buffer);

    } else {
        // Copy from the in-memory surface to the framebuffer.
        memcpy(gr_framebuffer[0].data, gr_draw->data,
               gr_draw->height * gr_draw->row_bytes);
    }

    return gr_draw;
}

static void fbdev_exit(minui_backend* backend __unused) {
    close(fb_fd);
    fb_fd = -1;

	close(fb_fd_dp);
	fb_fd_dp =-1;

	SecondScreen = -1;

    if (!double_buffered && gr_draw) {
        free(gr_draw->data);
        free(gr_draw);

		free(gr_draw_dp->data);
		free(gr_draw_dp);
    }
    gr_draw = NULL;
}
