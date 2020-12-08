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

#include <linux/types.h>
#include <linux/timer.h>
//#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/delay.h>


#include "esdadio_fnc.h"
#include "esdadio_io.h"

long     esdadio_ioctl_dev(struct file *filp, unsigned int *cmd_p, unsigned long *arg_p)
{
	unsigned int    cmd;
	unsigned long arg;
	pid_t                cur_proc = 0;
	int                    minor    = 0;
	int                    d_num    = 0;
	int                    retval   = 0;
	struct pci_dev* pdev;
	device_ioctrl_data  data;
	int size_data;
	struct pciedev_dev       *dev ;
	struct esdadio_dev       *adiodev ;

	dev  = filp->private_data;
	adiodev  = (esdadio_dev   *)dev->dev_str;
	cmd              = *cmd_p;
	arg                = *arg_p;
	size_data       = sizeof(device_ioctrl_data);
	minor           = dev->dev_minor;
	d_num         = dev->dev_num;	
	cur_proc     = current->group_leader->pid;
	pdev            = (dev->pciedev_pci_dev);
    
	if(!dev->dev_sts){
		printk(KERN_ALERT "ESDADIO_IOCTRL: NO DEVICE %d\n", dev->dev_num);
		retval = -EFAULT;
		return retval;
	}
    
/*
    if (mutex_lock_interruptible(&dev->dev_mut)){
	printk(KERN_ALERT "DAMCBPM_IOCTRL: NO MUTEX\n");
         return -ERESTARTSYS;
    }
*/
	
	mutex_lock(&dev->dev_mut);
    
	switch (cmd) {
	   case ESDADIO_PHYSICAL_SLOT:
		         //printk(KERN_ALERT "ESDADIO_IOCTRL: ESDADIO_PHYSICAL_SLOT\n");
			retval = 0;
			data.data    = dev->slot_num;
			data.cmd     = PCIEDEV_PHYSICAL_SLOT;
			if (copy_to_user((device_ioctrl_data*)arg, &data, (size_t)size_data)) {
				retval = -EFAULT;
				mutex_unlock(&dev->dev_mut);
				//printk(KERN_ALERT "ESDADIO_IOCTRL: ESDADIO_PHYSICAL_SLOT ERROR\n");
				return retval;
			}
			break;
		case ESDADIO_DRIVER_VERSION:
			//printk(KERN_ALERT "ESDADIO_IOCTRL: ESDADIO_DRIVER_VERSION\n");
			retval = 0;
			data.data   = ESDADIO_DRV_VER_MAJ;
			data.offset = ESDADIO_DRV_VER_MIN;
			if (copy_to_user((device_ioctrl_data*)arg, &data, (size_t)size_data)) {
				retval = -EFAULT;
				mutex_unlock(&dev->dev_mut);
				//printk(KERN_ALERT "ESDADIO_IOCTRL: ESDADIO_DRIVER_VERSION ERROR\n");
				return retval;
			}
			break;
		case ESDADIO_FIRMWARE_VERSION:
			//printk(KERN_ALERT "ESDADIO_IOCTRL: ESDADIO_FIRMWARE_VERSION\n");
			retval = 0;
			data.data   = ESDADIO_DRV_VER_MAJ + 1;
			data.offset = ESDADIO_DRV_VER_MIN + 1;
			if (copy_to_user((device_ioctrl_data*)arg, &data, (size_t)size_data)) {
				retval = -EFAULT;
				mutex_unlock(&dev->dev_mut);
				//printk(KERN_ALERT "ESDADIO_IOCTRL: ESDADIO_FIRMWARE_VERSION ERROR\n");
				return retval;
			}
			break;
		default:
			//printk(KERN_ALERT "ESDADIO_IOCTRL: WRONG CMD\n");
			return -ENOTTY;
			break;
	}
	mutex_unlock(&dev->dev_mut);
	return retval;
}