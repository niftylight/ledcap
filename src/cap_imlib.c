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

#ifdef HAVE_IMLIB

#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <niftyled.h>
#include "capture.h"




/** private structure to hold info accros function-calls */
static struct 
{
        Display *display;
        Visual *visual;
        Screen *screen;
        Colormap colormap;
        int depth;
        Window root;
}_c;





/**
 * capture image at x/y and store in frame
 */
static NftResult _capture(LedFrame *frame, LedFrameCord x, LedFrameCord y)
{
        if(!frame)
                NFT_LOG_NULL(NFT_FAILURE);

        
        /* get screen-portion from X server */
        XImage *image = NULL;
        if(!(image = XGetImage(_c.display, _c.root,
                       x, y,
                       led_frame_get_width(frame), led_frame_get_height(frame),
                       AllPlanes, ZPixmap)))
        {
                NFT_LOG(L_ERROR, "Failed to capture XImage");
                return NFT_FAILURE;
        }

        /* convert image to 32 bit RGB */
        Imlib_Image *iimg;
        if(!(iimg = imlib_create_image_from_ximage(image, NULL, 0, 0,
                        led_frame_get_width(frame), led_frame_get_height(frame),
                        TRUE)))
        {
                NFT_LOG(L_ERROR, "Failed to create Imlib_Image from XImage");
                return NFT_FAILURE;
        }

        /* get data */
        DATA32 *data;
        imlib_context_set_image(iimg);
        if(!(data = imlib_image_get_data_for_reading_only()))
        {
                NFT_LOG(L_ERROR, "Failed to get data from Imlib_Image");
                return NFT_FAILURE;
        }

        /* copy data to our frame */
        memcpy(led_frame_get_buffer(frame), data, led_frame_get_buffersize(frame));
        /*if(!led_frame_buffer_convert(frame, data, "RGBA u8", 
                                     led_frame_get_width(frame)* 
                                     led_frame_get_height(frame)))
                return NFT_FAILURE;*/
        
        /* destroy images */
        imlib_free_image();
        XDestroyImage(image);
        
        return NFT_SUCCESS;
}


/**
 * return prefered frame format
 */
static const char *_format()
{
        return "ARGB u8";
}


/**
 * return whether capture mechanism delivers big-endian ordered data
 */
static bool _is_big_endian()
{
        /* we'll always get big-endian data from imlib */
        return TRUE;
}


/**
 * initialize capture mechanism
 */
static NftResult _init()
{
        if(!(_c.display = XOpenDisplay(NULL)))
        {
                NFT_LOG(L_ERROR, "Can't open X display.");
                return NFT_FAILURE;
        }
        
        _c.screen = ScreenOfDisplay(_c.display, DefaultScreen(_c.display));

        _c.visual = DefaultVisual(_c.display, XScreenNumberOfScreen(_c.screen));
        _c.depth = DefaultDepth(_c.display, XScreenNumberOfScreen(_c.screen));
        _c.colormap = DefaultColormap(_c.display, XScreenNumberOfScreen(_c.screen));
        _c.root = RootWindow(_c.display, XScreenNumberOfScreen(_c.screen));

        imlib_context_set_display(_c.display);
        imlib_context_set_visual(_c.visual);
        imlib_context_set_colormap(_c.colormap);
        imlib_context_set_color_modifier(NULL);
        imlib_context_set_operation(IMLIB_OP_COPY);
        
        return NFT_SUCCESS;
}


/**
 * deinitialize capture mechanism
 */
static void _deinit()
{
        /* close X display */
        if(_c.display)
                XCloseDisplay(_c.display);
}


/** descriptor of this mechanism */
CaptureMechanism IMLIB =
{
        .name = "Imlib2",
        .init = _init,
        .deinit = _deinit,
        .capture = _capture,
        .format = _format,
        .is_big_endian = _is_big_endian,
};


#endif /* HAVE_IMLIB */
