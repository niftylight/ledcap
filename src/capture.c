/*
 * ledmag - Display portion of screen on a LED-Setup using libniftyled
 * Copyright (C) 2006-2013 Daniel Hiepler <daniel@niftylight.de>
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


#include <niftyled.h>
#include "config.h"
#include "capture.h"
#ifdef HAVE_X
#include "cap_x11.h"
#endif /* HAVE_X */
#ifdef HAVE_IMLIB
#include "cap_imlib.h"
#endif /* HAVE_IMLIB */


/** private structure to hold infos for this module */
static struct
{
        /** currently used capture method */
        CaptureMethod method;
} _c;

/** all registered capture-methods */
static CaptureMechanism *_mechanisms[] = {
#ifdef HAVE_X
        /** standard X11 capture using XGetImage() */
        &XLIB,
#endif /* HAVE_X */

#ifdef HAVE_IMLIB
        /** use imlib + X11 to capture screen */
        &IMLIB,
#endif /* HAVE_IMLIB */

        /** add descriptor of new mechanism above this line
           don't forget to add CaptureMethod in capture.h */
        NULL,
};

/** helper macro */
#define MECHANISM(m) (_mechanisms[m-1])


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/** print all available capture-mechanisms */
void capture_print_mechanisms()
{
        printf("Supported capture mechanisms:\n\t");

        int i;
        for(i = METHOD_MIN + 1; i < METHOD_MAX; i++)
        {
                printf("%s ", MECHANISM(i)->name);
        }
        printf("\n");
}

/**
 * convert capture-method to string
 */
const char *capture_method_to_string(CaptureMethod m)
{
        if(!METHOD_VALID(m))
                return "Invalid capture method";

        return (const char *) MECHANISM(m)->name;
}


/**
 * convert capture-mechanism name to capture-method id
 */
CaptureMethod capture_method_from_string(const char *name)
{
        int i;
        for(i = METHOD_MIN + 1; METHOD_VALID(i); i++)
        {
                if(strcmp(name, MECHANISM(i)->name) == 0)
                        return i;
        }

        return -1;
}


/**
 * capture a frame
 */
NftResult capture_frame(LedFrame * f, LedFrameCord x, LedFrameCord y)
{
        if(MECHANISM(_c.method)->capture)
        {
				LedFrameCord w,h;
				if(!led_frame_get_dim(f, &w, &h))
						return NFT_FAILURE;
				
                NFT_LOG(L_VERBOSE, "Capturing image x: %d, y: %d, %dx%d",
                        x, y, w, h);

                if(!(MECHANISM(_c.method)->capture(f, x, y)))
                {
                        NFT_LOG(L_ERROR,
                                "Capture with mechanism \"%s\" failed",
                                MECHANISM(_c.method)->name);
                        return NFT_FAILURE;
                }
        }
        else
        {
                NFT_LOG(L_ERROR, "Mechanism \"%s\" has no capture function?!",
                        MECHANISM(_c.method)->name);
                return NFT_FAILURE;
        }

        /* set endianness (flag will be changed when conversion occurs) */
        led_frame_set_big_endian(f, capture_is_big_endian());

        return NFT_SUCCESS;
}


/**
 * initialize capture-mechanism 
 */
NftResult capture_init(CaptureMethod m)
{
        /* validate CaptureMethod */
        if(!METHOD_VALID(m))
        {
                NFT_LOG(L_ERROR, "Invalid capture method");
                return NFT_FAILURE;
        }

        NFT_LOG(L_VERBOSE, "Initializing capture-method \"%s\"",
                MECHANISM(m)->name);

        /* initialize capture-method */
        if(MECHANISM(m)->init)
        {
                if(!MECHANISM(m)->init())
                {
                        NFT_LOG(L_ERROR,
                                "Initialization of capture-method \"%s\" failed",
                                MECHANISM(m)->name);
                        return NFT_FAILURE;
                }
        }

        /* save capture-method */
        _c.method = m;

        return NFT_SUCCESS;
}


/** deinitialize capture-mechanism */
void capture_deinit()
{
        /* validate current method */
        if(!METHOD_VALID(_c.method))
                return;

        /* deinitialize capture-method */
        if(MECHANISM(_c.method)->deinit)
        {
                MECHANISM(_c.method)->deinit();
        }
}


/** return format-string suitable for this capture-mechanism */
const char *capture_format()
{
        /* validate current method */
        if(!METHOD_VALID(_c.method))
                return NULL;

        if(!MECHANISM(_c.method)->format)
        {
                NFT_LOG(L_ERROR,
                        "Mechanism \"%s\" doesn't provide \"format\" function",
                        MECHANISM(_c.method)->name);
                return NULL;
        }

        return MECHANISM(_c.method)->format();
}

/** return whether the capture-method provides big-endian ordered data */
bool capture_is_big_endian()
{
        if(!METHOD_VALID(_c.method))
        {
                NFT_LOG(L_ERROR, "Invalid capture-method. This is a bug!");
                return false;
        }

        if(!MECHANISM(_c.method)->is_big_endian)
        {
                NFT_LOG(L_ERROR,
                        "Mechanism \"%s\" doesn't provide \"is_big_endian\" function. Defaulting to little-endian",
                        MECHANISM(_c.method)->name);
                return false;
        }

        return MECHANISM(_c.method)->is_big_endian();
}
