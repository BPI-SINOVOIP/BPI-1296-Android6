//  $Id$
/*****************
buffer_2D.h

Realtek 2013/10/3
by Tracey Chen
******************/


#ifndef __BUFFER_2D_H
#define __BUFFER_2D_H

//#########################################

//#######################################

typedef struct _BUFFER_2D
{
    int image_width;
    int image_height;
    unsigned char *dp;
    unsigned char *pTop, *pBot;
} BUFFER_2D;


#endif


