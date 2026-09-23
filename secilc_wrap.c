#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/mman.h>
#include <errno.h>

static int kfd=-1;
static void klog(const char*m){ if(kfd<0)kfd=open("/dev/kmsg",O_WRONLY); if(kfd>=0){ssize_t r=write(kfd,m,strlen(m)); (void)r;} }

static void mknode(const char*name){
  char p[160],b[64],dev[160],msg[192];
  snprintf(p,sizeof p,"/sys/class/misc/%s/dev",name);
  int f=open(p,O_RDONLY);
  if(f<0){ snprintf(msg,sizeof msg,"SECILC-WRAPPER: misc device MISSING: %s\n",name); klog(msg); return; }
  ssize_t r=read(f,b,sizeof b-1); close(f);
  if(r<=0)return; b[r]=0;
  int ma=0,mi=0;
  if(sscanf(b,"%d:%d",&ma,&mi)!=2)return;
  snprintf(dev,sizeof dev,"/dev/%s",name);
  mknod(dev,S_IFCHR|0666,makedev(ma,mi));
  chmod(dev,0666);
  snprintf(msg,sizeof msg,"SECILC-WRAPPER: created %s (%d:%d)\n",dev,ma,mi); klog(msg);
}

static void probe(void){
  char m[160];
  int fd=open("/dev/binder",O_RDWR|O_CLOEXEC);
  if(fd<0){ snprintf(m,sizeof m,"SECILC-WRAPPER: probe open err=%d\n",errno); klog(m); return; }
  void*p=mmap(NULL,1040384,PROT_READ,MAP_PRIVATE|MAP_NORESERVE,fd,0);
  if(p==MAP_FAILED){ snprintf(m,sizeof m,"SECILC-WRAPPER: probe mmap1M err=%d\n",errno); klog(m); }
  else { klog("SECILC-WRAPPER: probe mmap1M OK\n"); munmap(p,1040384); }
  void*q=mmap(NULL,16384,PROT_READ,MAP_PRIVATE|MAP_NORESERVE,fd,0);
  if(q==MAP_FAILED){ snprintf(m,sizeof m,"SECILC-WRAPPER: probe mmap16K err=%d\n",errno); klog(m); }
  else { klog("SECILC-WRAPPER: probe mmap16K OK\n"); munmap(q,16384); }
  close(fd);
}

int main(int argc,char**argv){
  klog("SECILC-WRAPPER: start\n");
  probe();
  mknode("binder"); mknode("hwbinder"); mknode("vndbinder");
  { char b[512]; int n=snprintf(b,sizeof b,"SECILC-WRAPPER:");
    for(int i=1;i<argc&&n<(int)sizeof(b)-1;i++)n+=snprintf(b+n,sizeof(b)-n," %s",argv[i]);
    if(n<(int)sizeof(b)-1){b[n++]='\n'; if(kfd>=0){ssize_t r=write(kfd,b,n); (void)r;}} }
  const char*out=0;
  for(int i=1;i+1<argc;i++)if(!strcmp(argv[i],"-o"))out=argv[i+1];
  if(!out){ if(kfd>=0)close(kfd); return 0; }
  int s=open("/vendor/etc/selinux/twrp_policy",O_RDONLY);
  if(s<0){ if(kfd>=0)close(kfd); return 1; }
  int d=open(out,O_WRONLY|O_CREAT|O_TRUNC,0644);
  if(d<0){close(s); if(kfd>=0)close(kfd); return 2; }
  char buf[65536]; ssize_t r;
  while((r=read(s,buf,sizeof buf))>0){ssize_t w=0;
    while(w<r){ssize_t k=write(d,buf+w,r-w);if(k<=0){close(s);close(d);if(kfd>=0)close(kfd); return 3;}w+=k;}}
  close(s);close(d);
  klog("SECILC-WRAPPER: policy copied\n");
  if(kfd>=0)close(kfd);
  return 0;
}
