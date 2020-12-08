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

#include <linux/module.h>
#include <linux/fs.h>	
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/timer.h>

#include "esdadio_io.h"
#include "esdadio_fnc.h"

MODULE_AUTHOR("Ludwig Petrosyan");
MODULE_DESCRIPTION("DESY ESDADIO board driver");
MODULE_VERSION("4.0.0");
MODULE_LICENSE("Dual BSD/GPL");

pciedev_cdev      *esdadio_cdev_m = 0;
esdadio_dev       *esdadio_dev_p[PCIEDEV_NR_DEVS];
esdadio_dev       *esdadio_dev_pp;

static int        esdadio_open( struct inode *inode, struct file *filp );
static int        esdadio_release(struct inode *inode, struct file *filp);
static ssize_t esdadio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t esdadio_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static long     esdadio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int        esdadio_remap_mmap(struct file *filp, struct vm_area_struct *vma);

void register_esdadio_proc(int num, char * dfn, struct pciedev_dev     *p_upcie_dev, struct pciedev_cdev     *p_upcie_cdev);
void unregister_esdadio_proc(int num, char *dfn);

struct file_operations esdadio_fops = {
    .owner                  =  THIS_MODULE,
    .read                     =  esdadio_read,
    .write                    =  esdadio_write,
    .unlocked_ioctl    =  esdadio_ioctl,
    .open                    =  esdadio_open,
    .release                =  esdadio_release,
    .mmap                 = esdadio_remap_mmap,
};

static struct pci_device_id esdadio_ids[]  = {
    { ESDADIO_VENDOR_ID, ESDADIO_DEVICE_ID,
                   ESDADIO_SUBVENDOR_ID, ESDADIO_SUBDEVICE_ID, 0, 0, 0},
    { 0, }
};
MODULE_DEVICE_TABLE(pci, esdadio_ids);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
    static int  esdadio_probe(struct pci_dev *dev, const struct pci_device_id *id)
#else 
    static int __devinit esdadio_probe(struct pci_dev *dev, const struct pci_device_id *id)
#endif
{  
	int result                     = 0;
	int tmp_brd_num        = 1;
	int tmp_slot_num        = 0;
	u32 tmp_slot_cap        = 0;
	int tmp_slot_num_u    = 0;
	int tmp_dev_num        = 0;
	int pcie_cap;
	pciedev_dev                *pciedevdev;
	char f_name[64];
	char prc_entr[64];

	printk(KERN_ALERT "PCIEDEV_PROBE CALLED \n");
	result = pciedev_probe_exp(dev, id, &esdadio_fops, esdadio_cdev_m, ESDADIO_DEVNAME, &tmp_brd_num);
	printk(KERN_ALERT "PCIEDEV_PROBE_EXP CALLED  FOR BOARD %i\n", tmp_brd_num);
	/*if board has created we will create our structure and pass it to pcedev_dev*/
	if(!result){
		printk(KERN_ALERT "PCIEDEV_PROBE_EXP CREATING CURRENT STRUCTURE FOR BOARD %i\n", tmp_brd_num);
		esdadio_dev_pp = kzalloc(sizeof(esdadio_dev), GFP_KERNEL);
		if(!esdadio_dev_pp){
				return -ENOMEM;
		}
		printk(KERN_ALERT "PCIEDEV_PROBE CALLED; CURRENT STRUCTURE CREATED \n");
		esdadio_dev_p[tmp_brd_num] = esdadio_dev_pp;
		esdadio_dev_pp->brd_num      = tmp_brd_num;
		esdadio_dev_pp->parent_dev  = esdadio_cdev_m->pciedev_dev_m[tmp_brd_num];
		pciedev_set_drvdata(esdadio_cdev_m->pciedev_dev_m[tmp_brd_num], esdadio_dev_p[tmp_brd_num]);

	//special for bridged device
	pciedevdev = dev_get_drvdata(&(dev->dev));
	tmp_slot_num_u    = esdadio_cdev_m->pciedev_dev_m[tmp_brd_num]->slot_num;
	printk(KERN_INFO "ESDADIO_PROBE:ORG SLOT NUM %d\n",tmp_slot_num_u);
	pcie_cap = pci_find_capability (dev->bus->parent->self, PCI_CAP_ID_EXP);
	pci_read_config_dword(dev->bus->parent->self, (pcie_cap +PCI_EXP_SLTCAP), &tmp_slot_cap);
	tmp_slot_num = (tmp_slot_cap >> 19);
	tmp_dev_num  = tmp_slot_num;
	printk(KERN_ALERT "ESDADIO_PROBE:SLOT NUM %d DEV NUM%d SLOT_CAP %X\n",
								    tmp_slot_num,tmp_dev_num,tmp_slot_cap);
	
	printk(KERN_INFO "ESDADIO_PROBE:DESTROY DEVICE FOR SLOT NUM %d\n",tmp_slot_num_u);
	device_destroy(esdadio_cdev_m->pciedev_class,  MKDEV(esdadio_cdev_m->PCIEDEV_MAJOR, 
						  esdadio_cdev_m->PCIEDEV_MINOR + tmp_brd_num));

	unregister_esdadio_proc(tmp_slot_num_u, ESDADIO_DEVNAME);
	printk(KERN_INFO "ESDADIO_PROBE:CRATE DEVICE SLOT NUM %d\n",tmp_slot_num);
	esdadio_cdev_m->pciedev_dev_m[tmp_brd_num]->slot_num  = tmp_slot_num;
	sprintf(f_name, "%ss%d", ESDADIO_DEVNAME, tmp_slot_num);
	sprintf(prc_entr, "%ss%d", ESDADIO_DEVNAME, tmp_slot_num);
	printk(KERN_INFO "PCIEDEV_PROBE:  CREAT DEVICE MAJOR %i MINOR %i F_NAME %s DEV_NAME %s\n",
			   esdadio_cdev_m->PCIEDEV_MAJOR, (esdadio_cdev_m->PCIEDEV_MINOR + tmp_brd_num), f_name);

	device_create(esdadio_cdev_m->pciedev_class, NULL, MKDEV(esdadio_cdev_m->PCIEDEV_MAJOR,  
			    esdadio_cdev_m->pciedev_dev_m[tmp_brd_num]->dev_minor),
			    & esdadio_cdev_m->pciedev_dev_m[tmp_brd_num]->pciedev_pci_dev->dev, f_name, 
			    esdadio_cdev_m->pciedev_dev_m[tmp_brd_num]->dev_name);


	register_esdadio_proc(tmp_slot_num, ESDADIO_DEVNAME, esdadio_cdev_m->pciedev_dev_m[tmp_brd_num], esdadio_cdev_m);


	//pciedev_setup_interrupt(esdadio_interrupt, esdadio_cdev_m->pciedev_dev_m[tmp_brd_num], DMACBPMNAME);
	}
	return result;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
    static void esdadio_remove(struct pci_dev *dev)
#else
    static void __devexit esdadio_remove(struct pci_dev *dev)
#endif
{
     int result               = 0;
     int tmp_slot_num = -1;
     int tmp_brd_num = -1;
     printk(KERN_ALERT "REMOVE CALLED\n");
     tmp_brd_num =pciedev_get_brdnum(dev);
     printk(KERN_ALERT "REMOVE CALLED FOR BOARD %i\n", tmp_brd_num);
     /* clean up any allocated resources and stuff here */
     kfree(esdadio_dev_p[tmp_brd_num]);
     /*now we can call pciedev_remove_exp to clean all standard allocated resources
      will clean all interrupts if it seted 
      */
     result = pciedev_remove_exp(dev,  esdadio_cdev_m, ESDADIO_DEVNAME, &tmp_slot_num);
     printk(KERN_ALERT "PCIEDEV_REMOVE_EXP CALLED  FOR SLOT %i\n", tmp_slot_num);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)    
    static struct pci_driver pci_esdadio_driver = {
    .name       = ESDADIO_DEVNAME,
    .id_table   = esdadio_ids,
    .probe      = esdadio_probe,
    .remove    = esdadio_remove,
};
#else 
static struct pci_driver pci_esdadio_driver = {
    .name       = ESDADIO_DEVNAME,
    .id_table   = esdadio_ids,
    .probe      = esdadio_probe,
    .remove    = __devexit_p(esdadio_remove),
};
#endif

static int esdadio_open( struct inode *inode, struct file *filp )
{
    int    result = 0;
    result = pciedev_open_exp( inode, filp );
    return result;
}

static int esdadio_release(struct inode *inode, struct file *filp)
{
    int result            = 0;
    result = pciedev_release_exp(inode, filp);
    return result;
} 

static ssize_t esdadio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t    retval         = 0;
    retval  = pciedev_read_exp(filp, buf, count, f_pos);
    return retval;
}

static ssize_t esdadio_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t         retval = 0;
    retval = pciedev_write_exp(filp, buf, count, f_pos);
    return retval;
}

static int esdadio_remap_mmap(struct file *filp, struct vm_area_struct *vma)
{
	ssize_t         retval = 0;
	retval =pciedev_remap_mmap_exp(filp, vma);
	return 0;
}

static long  esdadio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long result = 0;
     
	 if (_IOC_TYPE(cmd) == PCIEDOOCS_IOC){
		if (_IOC_NR(cmd) <= PCIEDOOCS_IOC_MAXNR && _IOC_NR(cmd) >= PCIEDOOCS_IOC_MINNR) {
			//printk(KERN_ALERT "ESDADIO_IOCTRL_PCIEDEV: \n");
			result = pciedev_ioctl_exp(filp, &cmd, &arg, esdadio_cdev_m);
		}else{
			return -ENOTTY;
		}
	}else{
		if (_IOC_TYPE(cmd) == ESDADIO_IOC){
			if (_IOC_NR(cmd) <= ESDADIO_IOC_MAXNR && _IOC_NR(cmd) >= ESDADIO_IOC_MINNR) {
				//printk(KERN_ALERT "ESDADIO_IOCTRL_DEV: \n");
				result = esdadio_ioctl_dev(filp, &cmd, &arg);
			}else{
				return -ENOTTY;
			}
		}
	}
	return result;
}



static void __exit esdadio_cleanup_module(void)
{
    printk(KERN_NOTICE "ESDADIO_CLEANUP_MODULE: PCI DRIVER UNREGISTERED\n");
    pci_unregister_driver(&pci_esdadio_driver);
    printk(KERN_NOTICE "ESDADIO_CLEANUP_MODULE CALLED\n");
    upciedev_cleanup_module_exp(&esdadio_cdev_m);
}

static int __init esdadio_init_module(void)
{
    int   result  = 0;
    
    printk(KERN_ALERT "ESDADIO_INIT:REGISTERING PCI DRIVER\n");
    printk(KERN_ALERT "&&&&&ESDADIO_INIT; THIS MODULE VERSION %s \n", THIS_MODULE->version);
    result = upciedev_init_module_exp(&esdadio_cdev_m, &esdadio_fops, ESDADIO_DEVNAME);
    result = pci_register_driver(&pci_esdadio_driver);
    printk(KERN_ALERT "ESDADIO_INIT:REGISTERING PCI DRIVER RESUALT %d\n", result);
    return 0; /* succeed */
}



#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
    void register_esdadio_proc(int num, char * dfn, struct pciedev_dev     *p_upcie_dev, struct pciedev_cdev     *p_upcie_cdev)
    {
        char prc_entr[32];
        sprintf(prc_entr, "%ss%i", dfn, num);
        p_upcie_cdev->pciedev_procdir = create_proc_entry(prc_entr, S_IFREG | S_IRUGO, 0);
        p_upcie_cdev->pciedev_procdir->read_proc = pciedev_procinfo;
        p_upcie_cdev->pciedev_procdir->data = p_upcie_dev;
    }

    void unregister_esdadio_proc(int num, char *dfn)
    {
        char prc_entr[32];
        sprintf(prc_entr, "%ss%i", dfn, num);
        remove_proc_entry(prc_entr,0);
    }

#else
    void register_esdadio_proc(int num, char * dfn, struct pciedev_dev     *p_upcie_dev, struct pciedev_cdev     *p_upcie_cdev)
    {
        char prc_entr[32];
        sprintf(prc_entr, "%ss%i", dfn, num);
        p_upcie_cdev->pciedev_procdir = proc_create_data(prc_entr, S_IFREG | S_IRUGO, 0, &upciedev_proc_fops, p_upcie_dev); 
    }

    void unregister_esdadio_proc(int num, char *dfn)
    {
        char prc_entr[32];
        sprintf(prc_entr, "%ss%i", dfn, num);
        remove_proc_entry(prc_entr,0);
    }

#endif

module_init(esdadio_init_module);
module_exit(esdadio_cleanup_module);

