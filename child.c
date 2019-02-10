#include "func.h"

void MakeChild(ProcessData* pManage,int ProcessNum){

	int i;
	pid_t pid;
	int fds[2];
	for(i=0;i<ProcessNum;i++)
	{
		socketpair(AF_LOCAL,SOCK_STREAM,0,fds);//创建一个本地全双工管道
		pid=fork();
		if(!pid)//如果为子进程
		{
			close(fds[1]);//每一个子进程关闭写端
			ChildHandler(fds[0]);
		}
		close(fds[0]);
		pManage[i].pid=pid;
		pManage[i].fds=fds[1];
	}
}

void ChildHandler(int fds)
{
	int new_fd;
	char flag=1;
	while(1)
	{
		recv_fd(fds,&new_fd);
		if(-1==new_fd)
		{
			exit(0);//子进程知道要退出
		}
		printf("我分配到任务了，开始发送文件\n");
		transFile(new_fd);
		close(new_fd);
		write(fds,&flag,sizeof(char));//通知父进程本子进程完成任务了
	}
}
