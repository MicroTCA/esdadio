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

#ifndef _ESDADIO_IO_H
#define _ESDADIO_IO_H

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#include "pciedev_io.h"

#define ESDADIO_IOC               'O'
#define ESDADIO_PHYSICAL_SLOT     _IOWR(ESDADIO_IOC, 80, int)
#define ESDADIO_DRIVER_VERSION    _IOWR(ESDADIO_IOC, 81, int)
#define ESDADIO_FIRMWARE_VERSION  _IOWR(ESDADIO_IOC, 82, int)
#define ESDADIO_IOC_MAXNR  83
#define ESDADIO_IOC_MINNR  80

#endif
