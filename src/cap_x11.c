/*
 * ledmag - Display portion of screen on a LED-Setup using libniftyled
 * Copyright (C) 2006-2011 Daniel Hiepler <daniel@niftylight.de>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#ifdef HAVE_X

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <niftyled.h>
#include "capture.h"

#define X_LOG_ERR(code) {  NFT_LOG(L_ERROR, "%s", _xerr); };


/** private structure to hold info accros function-calls */
static struct {
        Display *display;
        int screen;
}_c;




/** X11 Error handler, displays error message */
static int _err_handler(Display *d, XErrorEvent *err)
{
        /** space for X error message */
        static char _xerr[1024];
        
        XGetErrorText(d, err->error_code, _xerr, sizeof(_xerr));
        return 0;
}


/**
 * capture image
 */
static NftResult _capture(LedFrame *frame, LedFrameCord x, LedFrameCord y)
{
        if(!frame)
                NFT_LOG_NULL(NFT_FAILURE);


        /* get screen-portion from X server */
        XImage *image = NULL;
        if(!(image = XGetImage(_c.display, RootWindow(_c.display, _c.screen),
                       x, y,
                       led_frame_get_width(frame), led_frame_get_height(frame),
                       AllPlanes, ZPixmap)))
        {
                NFT_LOG(L_ERROR, "XGetImage() failed");
                return NFT_FAILURE;
        }

        /* copy framebuffer */
        memcpy(led_frame_get_buffer(frame), image->data, led_frame_get_buffersize(frame));

        /* destroy images */
        XDestroyImage(image);
        
        return NFT_SUCCESS;
}


/**
 * initialize capture mechanism
 */
static NftResult _init()
{
        /* connect to display */
        if(!(_c.display = XOpenDisplay(NULL)))
                return NFT_FAILURE;

        /* get screen */
        _c.screen = DefaultScreen(_c.display);

        /* set X error handler */
        XSetErrorHandler(_err_handler);
        
        return NFT_SUCCESS;
}


/**
 * deinitialize capture mechanism
 */
static void _deinit()
{
        /* close connection to display */
        if(_c.display)
                XCloseDisplay(_c.display);
}

/**
 * return prefered frame format (if this is 100% correct, I eat my shorts ;)
 */
static const char *_format()
{
        if(!_c.display)
                NFT_LOG_NULL(NULL);

        /* our result */
        char *res = "error";
        
        /* get XVisualInfo */
        XVisualInfo vi_proto = { .screen = _c.screen };
        XVisualInfo *vi;
        int nvi = 0;
        vi = XGetVisualInfo(_c.display, VisualScreenMask, &vi_proto, &nvi);
        if(!nvi)
        {
                NFT_LOG(L_ERROR, "No VisualInfo's returned?!");
                return NULL;
        }
        
        /* get default visual id */
        VisualID id;
        id = XVisualIDFromVisual(DefaultVisual(_c.display, _c.screen));

        /* get our VisualInfo */
        XVisualInfo *myvi = NULL;
        int i;
        for(i=0; i<nvi; i++)
        {
                if((vi[i].visualid) == id)
                {
                        myvi = &vi[i];
                        break;
                }
        }

        /* this may not happen */
        if(!myvi)
        {
                NFT_LOG(L_ERROR, "Didn't find our VisualID?!");
                goto _f_exit;
        }


        /* decide about bits-per-component */
        switch(myvi->bits_per_rgb)
        {
                case 8:
                {
                        res = "ARGB (u8)";
                        break;
                }

                case 16:
                {
                        res = "ARGB (u16)";
                        break;
                }

                case 32:
                {
                        res = "ARGB (u32)";
                        break;
                }
                        
                default:
                {
                        NFT_LOG(L_ERROR, "Invalid bits-per-component: %d", myvi->bits_per_rgb);
                        return NULL;
                }
                        
        }
        
        
_f_exit:
        NFT_LOG(L_VERBOSE, "Depth: %d Red-mask: 0x%lx Green-mask: "
                           "0x%lx Blue-mask: 0x%lx Bits per component: %d",
                               myvi->depth, 
                               myvi->red_mask, 
                               myvi->green_mask, 
                               myvi->blue_mask,
                               myvi->bits_per_rgb);
        XFree(vi);
        
        return (const char *) res;
}


/**
 * return whether capture mechanism delivers big-endian ordered data
 */
static bool _is_big_endian()
{
        if(XImageByteOrder(_c.display) == LSBFirst)
                return TRUE;
        else
                return FALSE;
}

/** descriptor of this mechanism */
CaptureMechanism XLIB =
{
        .name = "Xlib",
        .init = _init,
        .deinit = _deinit,
        .capture = _capture,
        .format = _format,
        .is_big_endian = _is_big_endian,
};


#endif /* HAVE_X */
