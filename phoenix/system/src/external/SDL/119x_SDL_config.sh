#!/bin/bash

SDL_SRC=$1

pushd $SDL_SRC

./configure --prefix=/home/barry/work/Phoenix/kernel/src --host=arm-linux \
	--disable-assembly \
	--disable-fast-install \
	--disable-joystick \
	--disable-sse --disable-sse2 --disable-ssemath \
	--disable-mmx --disable-3dnow --disable-altivec \
	--disable-video-x11 \
	--disable-video-x11-xcursor \
	--disable-video-x11-xinerama \
	--disable-video-x11-xinput \
	--disable-video-x11-xrandr \
	--disable-video-x11-scrnsaver \
	--disable-video-x11-xshape \
	--disable-video-x11-vm \
	--disable-x11-shared \
	--disable-video-cocoa \
	--disable-video-directfb --disable-directfb-shared \
	--disable-video-opengl --disable-video-opengles \
	--disable-directx --disable-render-d3d \
	--disable-fusionsound --disable-fusionsound-shared \
	--disable-oss --disable-alsa --disable-alsatest --disable-alsa-shared \
	--disable-esd --disable-esdtest --disable-esd-shared \
	--disable-pulseaudio --disable-pulseaudio-shared \
	--disable-arts --disable-arts-shared \
	--disable-nas --disable-nas-shared \
	--disable-sndio --disable-sndio-shared \
	CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++

popd $SDL_SRC
