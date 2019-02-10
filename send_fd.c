#include "func.h"

void send_fd(int socketpipew,int fd)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));//必须清空，因为结构体内还有其他不用但是不清零会有影响的内容
	//char buf[10]="xiongda";
	struct iovec iov[1];
	iov[0].iov_base=&fd;
	iov[0].iov_len=4;
	msg.msg_iov=iov;
	msg.msg_iovlen=1;
	struct cmsghdr *cmsg;//这是struct msghdr结构体里的control结构体
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr*)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;//16~18固定写法
	*(int*)CMSG_DATA(cmsg)=fd;//这样可以知道最后一个成员的空间,让内核传进来文件描述符
	if(-1==fd)//main中发的，描述符等于-1的时候说明子进程要退出了，都关闭后，
	{
		*(int*)CMSG_DATA(cmsg)=0;
	}
	msg.msg_control=cmsg;//把成员赋成这个空间的起始地址
	msg.msg_controllen=len;
	int ret;
	ret=sendmsg(socketpipew,&msg,0);//前面那一大堆都是因为sendmsg里面的结构体初始化繁杂
	if(-1==ret)
	{
		perror("sendmsg");
		return;
	}
}
void recv_fd(int socketpiper,int* fd)//因为传过来的是取地址的，所以要int*，主要是为了改变传进来的变量，不能单是值传递
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));//必须清空，因为结构体内还有其他不用但是不清零会有影响的内容
	int exitflag;
	//char buf[10]={0};
	struct iovec iov[1];
	iov[0].iov_base=&exitflag;//告知内核退出标志
	iov[0].iov_len=4;
	msg.msg_iov=iov;
	msg.msg_iovlen=1;
	struct cmsghdr *cmsg;//这是struct msghdr结构体里的control结构体
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr*)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;//16~18固定写法
	msg.msg_control=cmsg;//把成员赋成这个空间的起始地址
	msg.msg_controllen=len;
	int ret;
	ret=recvmsg(socketpiper,&msg,0);//前面那一大堆都是因为sendmsg里面的结构体初始化繁杂
	if(-1==ret)
	{
		perror("recvmsg");
		return;
	}
	*fd=*(int*)CMSG_DATA(cmsg);
	if(-1==exitflag)//退出的时候才成立
	{
		*fd=-1;
	}
}
