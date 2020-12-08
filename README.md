                             The ESDADIO Linux device driver
                             
The DAMC device driver designed to provide access to the memmory and IO space of the ESDADIO module.
As well to make DMA transfer from ESDADIO module to host memmory space.

Installation:
The driver file should to be installed in the /lib/modules/`uname -r'/doocs directory.
(create this  directory: "sudo mkdir /lib/modules/`uname -r'/doocs" )

Creating udev rules (Ubuntu 8.x for other Linux versions read the udev manual):
1. add this line to the /etc/udev/permissions.rules:
# ESDADIO modules
SUBSYSTEM=="esdadio",      MODE="066" 

2.add lines to the /etc/udev/rules.d/60-symlinks.rules
    #rules to create damc device nodes
    KERNEL=="esdadios3",     SYMLINK="esdadio1"
    KERNEL=="esdadios1",     SYMLINK="esdadio2"
    KERNEL=="esdadios2",     SYMLINK="esdadio0"
 
for UBUNTU 10.x one has to craete the files 40-permissions.rules and 60-symlinks.rules in the /etc/udev/rules.d directory.

Call "depmod -a" and try to load the driver calling "modprobe esdadio"
To unload the driver call rmmod esdadio.

One can check the messages frm driver calling "sudo tail -f /var/log/kern.log" 

The driver during loading creates the "esdadio" class in /sys/class and device nodes esdadiosX in /dev 
directory. There X is a slot number in which the module is installed. To have standart device names 
(for example esdadio0 or esdadio1) one should create udev rules.
   
Using the driver:
The example code is esdadio_test.cpp.

