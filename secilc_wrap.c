#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc,char**argv){
  int fd=open("/dev/kmsg",O_WRONLY);
  if(fd>=0){char b[512];int n=snprintf(b,sizeof b,"SECILC-WRAPPER:");
    for(int i=1;i<argc&&n<(int)sizeof(b)-1;i++)n+=snprintf(b+n,sizeof(b)-n," %s",argv[i]);
    if(n<(int)sizeof(b)-1){b[n++]='\n';write(fd,b,n);}close(fd);}
  const char*out=0;
  for(int i=1;i+1<argc;i++)if(!strcmp(argv[i],"-o"))out=argv[i+1];
  if(!out)return 0;
  int s=open("/vendor/etc/selinux/twrp_policy",O_RDONLY);
  if(s<0)return 1;
  int d=open(out,O_WRONLY|O_CREAT|O_TRUNC,0644);
  if(d<0){close(s);return 2;}
  char buf[65536];ssize_t r;
  while((r=read(s,buf,sizeof buf))>0){ssize_t w=0;
    while(w<r){ssize_t k=write(d,buf+w,r-w);if(k<=0)return 3;w+=k;}}
  close(s);close(d);return 0;
}
