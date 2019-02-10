#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<time.h>
#include<pwd.h>
#include<grp.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/wait.h> 
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h> 
#include<signal.h> 
#include<sys/msg.h>
#include<pthread.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<errno.h>
#include<sys/uio.h>

#define args_check(argc,num) {if(argc!=num){printf("error args\n");return -1;}}
#define opendir_error_check(pdir) {if(NULL==pdir){perror("opendir");return;}}

typedef struct{
	pid_t pid;
	int fds;//子进程的管道对端
	short busy;//0为空闲，1为忙碌
}ProcessData;
//应用层协议设计——小火车
typedef struct{
	int datalen;//是用来放buf里的实际字节数
	char buf[1000];
}train;//用来真正传输文件——实现下载用的结构体
#define DOWNFILE "file"


void MakeChild(ProcessData* pManage,int ProcessNum);
void ChildHandler(int);
int TcpPrepare(int*,char*,char*);
void send_fd(int,int);
void recv_fd(int,int*);
int transFile(int);
int send_n(int ,train*,int);//所谓的循环发送就是能发多少发多少
int recv_n(int ,char*,int);//实际能接多少接多少
