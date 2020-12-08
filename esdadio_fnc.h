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

#ifndef _ESDADIO_FNC_H_
#define _ESDADIO_FNC_H_

#include "pciedev_io.h"
#include "pciedev_ufn.h"

#ifndef ESDADIO_NR_DEVS
#define ESDADIO_NR_DEVS 15  /* esdadio0 through esdadio11 */
#endif

#define ESDADIO_DEVNAME "esdadio"	        /* name of device */
#define ESDADIO_VENDOR_ID 0x10B5	/* PLX vendor ID */
#define ESDADIO_DEVICE_ID 0x9056	/* PLX device ID */
#define ESDADIO_SUBVENDOR_ID 0x12FE	/* ESDADIO subvendor ID */
#define ESDADIO_SUBDEVICE_ID 0x0600	/* ESDADIO subdevice ID */

static u_short ESDADIO_DRV_VER_MAJ=3;
static u_short ESDADIO_DRV_VER_MIN=0;

/*ESD FPGA REGISTERS*/
/*
static int DMAWAS    = 0x00;
static int DMAWAD_L  = 0x04;
static int DMAWAD_U  = 0x08;
static int DMARAS_L  = 0x0C;
static int DMARAS_U  = 0x10;
static int DMARAD    = 0x14;
static int DMAWXS    = 0x18;
static int DMARXS    = 0x1C;
static int ADCCTRL   = 0x20;
static int DMACST    = 0x28;
static int DMAWRP    = 0x30;
static int DMARDP    = 0x34;
static int DMARW_RST = 0xA;
*/

struct esdadio_dev {
	int                              brd_num;
	struct pciedev_dev  *parent_dev;
};
typedef struct esdadio_dev esdadio_dev;

long     esdadio_ioctl_dev(struct file *, unsigned int *, unsigned long *);

#endif /* _ESDADIO_FNC_H_ */
