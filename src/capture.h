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

#ifndef _CAPTURE_H
#define _CAPTURE_H


/** all available capture methods */
typedef enum
{
        METHOD_MIN = 0,
#ifdef HAVE_X
        METHOD_XLIB,
#endif /* HAVE_X */
#ifdef HAVE_IMLIB
        METHOD_IMLIB,
#endif /* HAVE_IMLIB */
        /* insert new method above this line
           don't forget to add the descriptor to
           _mechanisms[] in capture.c */
        METHOD_MAX,
} CaptureMethod;


/** the descriptor for a capture mechanism */
typedef struct
{
        /** name of this mechanism */
        char name[32];
        /** initialization function */
        NftResult (*init)(void);
        /** deinitalization function */
        void (*deinit)(void);
        /** get format prefered by mechanism */
        const char *(*format)(void);
        /** return whether capture mechanism delivers big-endian ordered data */
        bool (*is_big_endian)(void);
        /** capture image */
        NftResult (*capture)(LedFrame *, LedFrameCord, LedFrameCord);
} CaptureMechanism;

/** macro to check if a capture-method is valid */
#define METHOD_VALID(m) ((m > METHOD_MIN) && (m < METHOD_MAX))



void            capture_print_mechanisms();
const char *    capture_method_to_string(CaptureMethod m);
CaptureMethod   capture_method_from_string(const char *name);
bool            capture_is_big_endian();
const char *    capture_format();
NftResult       capture_frame(LedFrame *frame, LedFrameCord x, LedFrameCord y);
NftResult       capture_init(CaptureMethod m);
void            capture_deinit();



#endif /** _CAPTURE_H */
