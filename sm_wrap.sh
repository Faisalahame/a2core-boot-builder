#!/system/bin/sh
exec strace -f -e trace=open,ioctl,mmap,mmap2,close /system/bin/servicemanager.real 2>> /dev/kmsg
