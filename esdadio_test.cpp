/**
*Copyright 2016-  DESY (Deutsches Elektronen-Synchrotron, www.desy.de)
*
*This file is part of ESDADIO driver.
*
*ESDADIO is free software: you can redistribute it and/or modify
*it under the terms of the GNU General Public License as published by
*the Free Software Foundation, either version 3 of the License, or
*(at your option) any later version.
*
*ESDADIO is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with ESDADIO.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

#include "esdadio_io.h"

/* useconds from struct timeval */
#define	MILLS(tv) (((double)(tv).tv_usec ) + ((double)(tv).tv_sec * 1000000.0)) 	

int	         fd;
int main(int argc, char* argv[])
{
    int code = 0;
    int	 ch_in        = 0;
    char nod_name[15] = "";
    device_rw	          l_Read;
    device_ioctrl_data    io_RW;
    int			  tmp_mode = 0;
    u_int	          tmp_offset = 0;
    int      	          tmp_data = 0;
    int      	          tmp_barx = 0;
    int      	          tmp_count = 0;
    int      	          tmp_riesing_stepA = 0;
    int      	          tmp_falling_stepA = 0;
    int      	          tmp_riesing_stepB = 0;
    int      	          tmp_falling_stepB = 0;
    int      	          tmp_riesing_data = 0;
    int      	          tmp_falling_data = 0;
    int      	          tmp_riesing_dataA = 0;
    int      	          tmp_falling_dataA = 0;
    int      	          tmp_riesing_dataB = 0;
    int      	          tmp_falling_dataB = 0;
    int      	          tmp_channel = 0;
    int      	          tmp_channel1 = 0;
    int      	          tmp_increment = 0;
    float                 tmp_fdata = 0.0;
    int                   len = 0;
    int                   k = 0;
    int                   i = 0;
    int                   itemsize = 0;
    
    itemsize = sizeof(device_rw);
	
    if(argc ==1){
        printf("Input \"prog /dev/damc0\" \n");
        return 0;
    }

    strncpy(nod_name,argv[1],sizeof(nod_name));
    fd = open (nod_name, O_RDWR);
    if (fd < 0) {
        printf ("#CAN'T OPEN FILE \n");
        exit (1);
    }

     while (ch_in != 11){
        printf("\n READ (1) or WRITE (0) or END (11) ?-");
        printf("\n GET DRIVER VERSION (2) or GET FIRMWARE VERSION (3)?-");
        printf("\n GET SLOT NUM (4) or GET_TIME (5) ?-");
	printf("\n BUF_WRITE (6) CHANNEL_WRITE (7)?-");
        scanf("%d",&ch_in);
        fflush(stdin);
        l_Read.offset_rw   = 0;
        l_Read.data_rw     = 0;
        l_Read.mode_rw     = 0;
        l_Read.barx_rw     = 0;
        l_Read.size_rw     = 0;
	switch (ch_in){
            case 0 :
                printf ("\n INPUT  BARx (0,1,2,3...)  -");
                scanf ("%x",&tmp_barx);
                fflush(stdin);

                printf ("\n INPUT  MODE  (0-D8,1-D16,2-D32)  -");
                scanf ("%x",&tmp_mode);
                fflush(stdin);

                printf ("\n INPUT  ADDRESS (IN HEX)  -");
                scanf ("%x",&tmp_offset);
                fflush(stdin);

                printf ("\n INPUT DATA (IN HEX)  -");
                scanf ("%x",&tmp_data);
                fflush(stdin);

                l_Read.data_rw   = tmp_data;
                l_Read.offset_rw = tmp_offset;
                l_Read.mode_rw   = tmp_mode;
                l_Read.barx_rw   = tmp_barx;
                l_Read.size_rw   = 0;

                printf ("MODE - %X , OFFSET - %X, DATA - %X\n",
                     l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw);

                len = write (fd, &l_Read, sizeof(device_rw));
                if (len != itemsize ){
                        printf ("#CAN'T READ FILE \n");
                }

                break;
	    case 1 :
                printf ("\n INPUT  BARx (0,1,2,3)  -");
                scanf ("%x",&tmp_barx);
                fflush(stdin);
                printf ("\n INPUT  MODE  (0-D8,1-D16,2-D32)  -");
                scanf ("%x",&tmp_mode);
                fflush(stdin);
                printf ("\n INPUT OFFSET (IN HEX)  -");
                scanf ("%x",&tmp_offset);
                fflush(stdin);
                l_Read.data_rw   = 0;
                l_Read.offset_rw = tmp_offset;
                l_Read.mode_rw   = tmp_mode;
                l_Read.barx_rw   = tmp_barx;
                l_Read.size_rw   = 0;
                printf ("MODE - %X , OFFSET - %X, DATA - %X\n",
                        l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw);
                len = read (fd, &l_Read, sizeof(device_rw));
                if (len != itemsize ){
		   printf ("#CAN'T READ FILE \n");
		}

                printf ("READED : MODE - %X , OFFSET - %X, DATA - %X\n",
			l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw);
		break;
           case 2 :
                ioctl(fd, ESDADIO_DRIVER_VERSION, &io_RW);
                tmp_fdata = (float)((float)io_RW.offset/10.0);
                tmp_fdata += (float)io_RW.data;
                printf ("DRIVER VERSION IS %f\n", tmp_fdata);
                break;
	    case 3 :
                ioctl(fd, ESDADIO_FIRMWARE_VERSION, &io_RW);
                printf ("FIRMWARE VERSION IS - %X\n", io_RW.data);
		break;
            case 4 :
                code = ioctl(fd, ESDADIO_PHYSICAL_SLOT, &io_RW);
                printf ("SLOT NUM IS - %X retval %i\n", io_RW.data, code);
		break;
	    case 6 :
                printf ("\n INPUT  BARx (0,1,2,3...)  -");
                scanf ("%x",&tmp_barx);
                fflush(stdin);

                printf ("\n INPUT  MODE  (0-D8,1-D16,2-D32)  -");
                scanf ("%x",&tmp_mode);
                fflush(stdin);

                printf ("\n INPUT  ADDRESS (IN HEX)  -");
                scanf ("%x",&tmp_offset);
                fflush(stdin);

                printf ("\n INPUT DATA (IN HEX)  -");
                scanf ("%x",&tmp_data);
                fflush(stdin);

		printf ("\n COUNT  -");
                scanf ("%i",&tmp_count);
                fflush(stdin);

		printf ("\n INCREMENT 0-NO, 1-YES)  -");
                scanf ("%i",&tmp_increment);
                fflush(stdin);

		printf ("\n SET rising steps 00  -");
                scanf ("%i",&tmp_channel);
                fflush(stdin);

		printf ("\n SET falling steps 00  -");
                scanf ("%i",&tmp_channel1);
                fflush(stdin);

		tmp_riesing_stepA = (tmp_data & 0xFFFF) / tmp_channel;
		tmp_falling_stepA = (tmp_data & 0xFFFF) / tmp_channel1;

		tmp_riesing_stepB = ((tmp_data >> 16) & 0xFFFF)  / tmp_channel;
		tmp_falling_stepB = ((tmp_data >> 16) & 0xFFFF) / tmp_channel1;

		printf("STEPS RISING A/B %X/%X \n", tmp_riesing_stepA, tmp_riesing_stepB);
		printf("STEPS FALLING A/B %X/%X \n", tmp_falling_stepA, tmp_falling_stepB);


                l_Read.data_rw   = tmp_data;
                l_Read.offset_rw = tmp_offset;
                l_Read.mode_rw   = tmp_mode;
                l_Read.barx_rw   = tmp_barx;
                l_Read.size_rw   = 0;

                printf ("MODE - %X , OFFSET - %X, DATA - %X\n",
                     l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw);

		tmp_riesing_data = 0;
		tmp_falling_data = 0;

		tmp_riesing_dataA = 0;
		tmp_falling_dataA = 0;
		tmp_riesing_dataB = 0;
		tmp_falling_dataB = 0;

/*
		l_Read.mode_rw   = tmp_mode;
	        l_Read.barx_rw   = tmp_barx;
	        l_Read.size_rw   = 0;
		l_Read.data_rw = tmp_riesing_data;
		l_Read.offset_rw = tmp_offset;
	        len = write (fd, &l_Read, sizeof(device_rw));
	        if (len != itemsize ){
	                printf ("#CAN'T READ FILE \n");
	        }
*/
		for(i = 0; i < tmp_count; ++i){
			
			l_Read.data_rw   = tmp_data;
			if(tmp_channel){
				if(i <= tmp_channel){ 
					
					tmp_riesing_dataA = tmp_riesing_stepA * i;
					tmp_riesing_dataB = ((tmp_riesing_stepB * i) << 16);
					//tmp_riesing_data  += (tmp_riesing_dataA + tmp_riesing_dataB);
					tmp_riesing_data  = (tmp_riesing_dataA + tmp_riesing_dataB);
					l_Read.data_rw = tmp_riesing_data;
				}
			}
			if(tmp_channel1){
				if(i >= (tmp_count - tmp_channel1)){ 
					tmp_falling_dataA = tmp_falling_stepA * (tmp_count - i - 1);
					tmp_falling_dataB = ((tmp_falling_stepB * (tmp_count - i - 1)) << 16);
					//tmp_falling_data  += (tmp_falling_dataA + tmp_falling_dataB);
					tmp_falling_data  = (tmp_falling_dataA + tmp_falling_dataB);
					l_Read.data_rw = tmp_falling_data;
				}
			}
		        l_Read.offset_rw = tmp_offset;
			printf("OFFSET %X DATA %X \n", (i*4), l_Read.data_rw);
			if(tmp_increment){
				l_Read.offset_rw = tmp_offset + i*4;
			}
		        l_Read.mode_rw   = tmp_mode;
		        l_Read.barx_rw   = tmp_barx;
		        l_Read.size_rw   = 0;
		        len = write (fd, &l_Read, sizeof(device_rw));
		        if (len != itemsize ){
		                printf ("#CAN'T READ FILE \n");
		        }
		}
/*
		l_Read.mode_rw   = tmp_mode;
	        l_Read.barx_rw   = tmp_barx;
	        l_Read.size_rw   = 0;
		l_Read.data_rw = tmp_riesing_data;
		l_Read.offset_rw = tmp_offset + tmp_count*4;
	        len = write (fd, &l_Read, sizeof(device_rw));
	        if (len != itemsize ){
	                printf ("#CAN'T READ FILE \n");
	        }
*/
                break;
	    case 7 :
                printf ("\n INPUT  BARx (0,1,2,3...)  -");
                scanf ("%x",&tmp_barx);
                fflush(stdin);

                printf ("\n INPUT  MODE  (0-D8,1-D16,2-D32)  -");
                scanf ("%x",&tmp_mode);
                fflush(stdin);

                printf ("\n INPUT  CHANNEL (IN HEX)  -");
                scanf ("%x",&tmp_offset);
                fflush(stdin);

                printf ("\n INPUT MIN DATA (IN HEX)  -");
                scanf ("%x",&tmp_data);
                fflush(stdin);

		printf ("\n COUNT  -");
                scanf ("%i",&tmp_count);
                fflush(stdin);

                l_Read.data_rw   = tmp_data;
                l_Read.offset_rw = tmp_offset;
                l_Read.mode_rw   = tmp_mode;
                l_Read.barx_rw   = tmp_barx;
                l_Read.size_rw   = 0;

                printf ("MODE - %X , OFFSET - %X, DATA - %X\n",
                     l_Read.mode_rw, l_Read.offset_rw, l_Read.data_rw);

		for(i = 0; i < tmp_count; ++i){
			l_Read.data_rw   = ((tmp_data * i)&0xFFFF) + ((tmp_offset & 0xF)<<16);
			if(i < 4) l_Read.data_rw = 0 + ((tmp_offset & 0xF)<<16);
			if(i > 252) l_Read.data_rw = 0 + ((tmp_offset & 0xF)<<16);
		        l_Read.offset_rw = 0xFC;
		        l_Read.mode_rw   = tmp_mode;
		        l_Read.barx_rw   = tmp_barx;
		        l_Read.size_rw   = 0;
		        len = write (fd, &l_Read, sizeof(device_rw));
		        if (len != itemsize ){
		                printf ("#CAN'T READ FILE \n");
		        }
		}

                break;
            default:
		break;
	}
    }

    close(fd);
    return 0;
}

