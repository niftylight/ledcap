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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <getopt.h>

#include <niftyled.h>
#include "capture.h"
#include "version.h"



/** 
 * @todo make mechanism selectable
 * @todo print list of mechanisms with --help
 * @todo make mechanisms optional in configure.ac
 */

/** local structure to hold various information */
static struct
{
        /** currently selected screen-capture method */
        CaptureMethod method;
        /** running state (TRUE when running, set to FALSE to break main-loop */
	bool running;
	/** name of config-file */
	char prefsfile[1024];
        /** requested framerate */
        int fps;
        /** input frame x-offset */
        LedFrameCord x;
        /** input frame y-offset */
        LedFrameCord y;
        /** input frame width (in pixels) */
        LedFrameCord width;
        /** input frame height (in pixels) */
        LedFrameCord height;
}_c;



/******************************************************************************/



/** print a line with all valid logleves */
void _print_loglevels()
{
        /* print loglevels */
	printf("Valid loglevels:\n\t");
	NftLoglevel i;
	for(i = L_MAX+1; i<L_MIN-1; i++)
		printf("%s ", nft_log_level_to_string(i));
        printf("\n\n");
}


/** print commandline help */
static void _print_help(char *name)
{
	printf("Capture portion of screen & display on LED hardware - %s\n"
	       "Usage: %s [options]\n\n"
	       "Valid options:\n"
	       "\t--help\t\t\t-h\t\tThis help text\n"
               "\t--mechanism <name>\t-m <name>\tCapture mechanism (default: \"Xlib\")\n"
               "\t--plugin-help\t\t-p\t\tList of installed plugins + information\n"
	       "\t--config <file>\t\t-c <file>\tLoad this config file (default: ~/.ledcat.xml) \n"
               "\t--x <x>\t\t\t-x <x>\t\tX-coordinate of capture rectangle (default: 0)\n"
               "\t--y <y>\t\t\t-y <y>\t\tY-coordinate of capture rectangle (default: 0)\n"
               "\t--dimensions <w>x<h>\t-d <w>x<h>\tDefine width and height of capture rectangle. (default: auto)\n"
               "\t--fps <n>\t\t-f <n>\t\tFramerate to play multiple frames at (default: 25)\n"
	       "\t--loglevel <level>\t-l <level>\tOnly show messages with loglevel <level> (default: info)\n\n",
	       PACKAGE_URL, name);

        printf("\n");
	_print_loglevels();
        capture_print_mechanisms();
}


/** print list of installed plugins + information they provide */
static void _print_plugin_help()
{

        /* save current loglevel */
        NftLoglevel ll_current = nft_log_level_get();
        nft_log_level_set(L_INFO);
        
        int i;
        for(i = 0; i < led_hardware_plugin_total_count(); i++)
        {
                const char *name;
                if(!(name = led_hardware_plugin_get_family_by_n(i)))
                        continue;

                printf("======================================\n\n");
                
                LedHardware *h;
                if(!(h = led_hardware_new("tmp01", name)))
                        continue;
                
                printf("\tID Example: %s\n",
                       led_hardware_plugin_get_id_example(h));

                
                led_hardware_destroy(h);
                       
        }

        /* restore logolevel */
        nft_log_level_set(ll_current);
}


/** parse commandline arguments */
static NftResult _parse_args(int argc, char *argv[])
{
	int index, argument;

	static struct option loptions[] =
	{
		{"help", 0, 0, 'h'},
                {"plugin-help", 0, 0, 'p'},
		{"loglevel", required_argument, 0, 'l'},
		{"config", required_argument, 0, 'c'},
                {"x", required_argument, 0, 'x'},
                {"y", required_argument, 0, 'y'},
                {"dimensions", required_argument, 0, 'd'},
                {"fps", required_argument, 0, 'f'},
                {"mechanism", required_argument, 0, 'm'},
		{0,0,0,0}
	};

	while((argument = getopt_long(argc, argv, "hpl:c:x:y:d:f:m:", loptions, &index)) >= 0)
	{
		
		switch(argument)
		{			
			/** --help */
			case 'h':
			{
				_print_help(argv[0]);
				return NFT_FAILURE;
			}

                        /* --plugin-help */
                        case 'p':
                        {
                                _print_plugin_help();
                                return NFT_FAILURE;
                        }

                        /* --mechanism */
                        case 'm':
                        {
                                _c.method = capture_method_from_string(optarg);
                                break;
                        }
                                
			/** --config */
			case 'c':
			{
				/* save filename for later */
				strncpy(_c.prefsfile, optarg, sizeof(_c.prefsfile));
				break;
			}

                        /** --x */
                        case 'x':
                        {
                                if(sscanf(optarg, "%d", (int*) &_c.x) != 1)
				{
					NFT_LOG(L_ERROR, "Invalid x-coordinate \"%s\" (Use an integer)", optarg);
					return NFT_FAILURE;
				}
				break;
                        }

                        /** --y */
                        case 'y':
                        {
                                if(sscanf(optarg, "%d", (int*) &_c.y) != 1)
				{
					NFT_LOG(L_ERROR, "Invalid y-coordinate \"%s\" (Use an integer)", optarg);
					return NFT_FAILURE;
				}
				break;
                        }
                                
                        /** --dimensions */
                        case 'd':
                        {
                                if(sscanf(optarg, "%dx%d", (int*) &_c.width, (int*) &_c.height) != 2)
				{
					NFT_LOG(L_ERROR, "Invalid dimension \"%s\" (Use something like 320x400)", optarg);
					return NFT_FAILURE;
				}
                                break;
                        }
                                
                        /** --fps */
                        case 'f':
                        {
                                if(sscanf(optarg, "%d", (int*) &_c.fps) != 1)
				{
					NFT_LOG(L_ERROR, "Invalid framerate \"%s\" (Use an integer)", optarg);
					return NFT_FAILURE;
				}
				break;
                        }
                                
			/** --loglevel */
			case 'l':
			{
				if(!nft_log_level_set(nft_log_level_from_string(optarg)))
                                {
                                        _print_loglevels();
                                        return NFT_FAILURE;
                                }
				break;
			}
				
			/* invalid argument */
			case '?':
			{
				NFT_LOG(L_ERROR, "argument %d is invalid", index);
				_print_help(argv[0]);
				return NFT_FAILURE;
			}

			/* unhandled arguments */
			default:
			{
				NFT_LOG(L_ERROR, "argument %d is invalid", index);
				break;
			}
		}
	}
	
	
	return NFT_SUCCESS;
}

/** signal handler for exiting */
void _exit_signal_handler(int signal)
{
	NFT_LOG(L_INFO, "Exiting...");
	_c.running = FALSE;
}



/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int main(int argc, char *argv[])
{
        /** return value of main() */
        int res = 1;
        /* current configuration */
        LedPrefs *prefs = NULL;
	/* current setup */
	LedSetup *setup = NULL;
        /** framebuffer for captured image */
        LedFrame *frame = NULL;

        

        /* check binary version compatibility */
        NFT_LED_CHECK_VERSION

                
        /* set default loglevel to INFO */
	nft_log_level_set(L_INFO);

        
	/* initialize exit handlers */
	int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGABRT };
	int i;
	for(i=0; i<sizeof(signals)/sizeof(int); i++)
	{
		if(signal(signals[i], _exit_signal_handler) == SIG_ERR)
		{
			NFT_LOG_PERROR("signal()");
			goto _m_exit;
		}
	}


	
        /* default fps */
        _c.fps = 25;
      
        /* default mechanism */
        _c.method = METHOD_XLIB;
        
        /* default config-filename */
        if(!led_prefs_default_filename(_c.prefsfile, sizeof(_c.prefsfile), ".ledcap.xml"))
                goto _m_exit;

	
        /* parse cmdline-arguments */
        if(!_parse_args(argc, argv))
                goto _m_exit;

	
	/* print welcome msg */
	NFT_LOG(L_INFO, "%s %s (c) D.Hiepler 2006-2012", PACKAGE_NAME, ledcap_version_long());
	NFT_LOG(L_VERBOSE, "Loglevel: %s", nft_log_level_to_string(nft_log_level_get()));
        


	/* initialize preferences context */
    	if(!(prefs = led_prefs_init()))
    		return -1;
    
	/* parse prefs-file */
    	LedPrefsNode *pnode;
    	if(!(pnode = nft_prefs_node_from_file(prefs, _c.prefsfile)))
    	{
		NFT_LOG(L_ERROR, "Failed to open configfile \"%s\"", 
		        		_c.prefsfile);
		goto _m_exit;
	}
	
        /* create setup from prefs-node */
    	if(!(setup = led_prefs_setup_from_node(prefs, pnode)))
    	{
		NFT_LOG(L_ERROR, "No valid setup found in preferences file.");
		nft_prefs_node_free(pnode);
		goto _m_exit;
	}

    	/* free preferences node */
    	nft_prefs_node_free(pnode);

        
                    

        /* determine width of input-frames */
        LedFrameCord width, height;
        if((width = led_setup_get_width(setup)) > _c.width)
        {
                NFT_LOG(L_WARNING, "LED-Setup width (%d) > our width (%d). Using setup-value", 
                        width,_c.width);
                /* use dimensions of mapped chain */
                _c.width = width;
        }

        /* determine height of input-frames */
        if((height = led_setup_get_height(setup)) > _c.height)
        {
                NFT_LOG(L_WARNING, "LED-Setup height (%d) > our height (%d). Using setup-value.",
                        height, _c.height);
                /* use dimensions of mapped chain */
                _c.height = height;
        }


        if(_c.width < 0)
        {
                NFT_LOG(L_ERROR, "width (%d) < 0", _c.width);
                goto _m_exit;
        }
        
        if(_c.height < 0)
        {
                NFT_LOG(L_ERROR, "height (%d) < 0", _c.height);
                goto _m_exit;
        }
        
        /* sanitize x-offset @todo check for maximum */
        if(_c.x < 0)
        {
                NFT_LOG(L_ERROR, "Invalid x coordinate: %d, using 0", _c.x);
                _c.x = 0;
        }

        /* sanitize y-offset @todo check for maximum */
        if(_c.y < 0)
        {
                NFT_LOG(L_ERROR, "Invalid y coordinate: %d, using 0", _c.y);
                _c.y = 0;
        }
               
                
        /* initialize capture mechanism (only imlib for now) */
        if(!capture_init(_c.method))
                goto _m_exit;

        /* allocate framebuffer */
        NFT_LOG(L_INFO, "Allocating frame: %dx%d (%s)", 
	        _c.width, _c.height, capture_format());
        if(!(frame = led_frame_new(_c.width, _c.height, led_pixel_format_from_string(capture_format()))))
                goto _m_exit;

        /* respect endianess */
        led_frame_set_big_endian(frame, capture_is_big_endian());

        /* get first hardware */
        LedHardware *hw;
        if(!(hw = led_setup_get_hardware(setup)))
                goto _m_exit;
         
        /* initialize pixel->led mapping */
        if(!led_hardware_list_refresh_mapping(hw))
                goto _m_exit;

        /* precalc memory offsets for actual mapping */
        if(!led_chain_map_from_frame(led_hardware_get_chain(hw), frame))
                goto _m_exit;
                                        
        /* set saved gain to all registered hardware instances */
        if(!led_hardware_list_refresh_gain(hw))
                goto _m_exit;
        

	/* print some debug-info */
        led_frame_print(frame, L_VERBOSE);
        led_hardware_print(hw, L_VERBOSE);
        
        
        /* initially sample time for frametiming */
        if(!led_fps_sample())
                goto _m_exit;


        /* output some useful info */
        NFT_LOG(L_INFO, "Capturing %dx%d pixels at position x/y: %d/%d", _c.width, _c.height, _c.x, _c.y);
        
        /* loop until _c.running is set to FALSE */
        _c.running = TRUE;
        while(_c.running)
        {
                /* capture frame */
                if(!(capture_frame(frame, _c.x, _c.y)))
                        break;
                
                /* print frame for debugging */
                //led_frame_buffer_print(frame);

                /* map from frame */
                LedHardware *h;
                for(h = hw; h; h = led_hardware_list_get_next(h))
                {
                        if(!led_chain_fill_from_frame(led_hardware_get_chain(h), frame))
                        {
                                NFT_LOG(L_ERROR, "Error while mapping frame");
                                break;
                        }
                }

                
                /* send frame to hardware(s) */
                led_hardware_list_send(hw);
                
                /* delay in respect to fps */
                if(!led_fps_delay(_c.fps))
                        break;
                
                /* show frame */
                led_hardware_list_show(hw);

                /* save time when frame is displayed */
                if(!led_fps_sample())
                        break;
        }

        
        
        /* mark success */
        res = 0;
        
_m_exit:	
        /* deinitialize capture mechanism */
        capture_deinit();

	/* free frame */
        led_frame_destroy(frame);
	
	/* destroy config */
        led_setup_destroy(setup);
	
	/* destroy config */
        led_prefs_deinit(prefs);

	
        return res;
}
