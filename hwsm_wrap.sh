#!/system/bin/sh
exec strace -f -e trace=open,ioctl,mmap,mmap2,close /system/bin/hwservicemanager.real 2>> /dev/kmsg
