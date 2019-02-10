#include "func.h"
int exitfds[2];//作退出用的传入传出参数

void sigfunc(int signum)
{
	write(exitfds[1],&signum,1);//不用管写入了什么，只要写入了东西让读端变成可读，那就代表要退出了
}

int main(int argc,char** argv)
{
	pipe(exitfds);//初始化无名管道,这个管道是单进程自己用的,以便做异步拉起同步操作
	signal(SIGUSR1,sigfunc);//让客户端发10号信号就可以了
	args_check(argc,4);//因为自己0，ip地址1，端口号2，进程个数3,共3个传参
	int ProcessNum=atoi(argv[3]);//输入的参数是字符串类型的
	ProcessData *pManage=(ProcessData*)calloc(ProcessNum,sizeof(ProcessData));
	/*上面这个结构体指针是因为要传进函数内，去改变结构体内容的，当然可以不用指针，改用传地址*/
	MakeChild(pManage,ProcessNum);
	int socketfd;
	TcpPrepare(&socketfd,argv[1],argv[2]);//打包从socket到bind函数
	printf("pid = %d\n",getpid());
	struct epoll_event event,*evs;//接下来要让父进程监控所创建的子进程的各个对端，之所以要指针类型是为了便于指向多个子进程
	evs=(struct epoll_event*)calloc(ProcessNum+2,sizeof(struct epoll_event));
	/*之所以用要+1是因为除了ProcessNum个子进程外还有1个socketfd,或者说父进程*/
	int epfd=epoll_create(1);//开始注册了
	event.events=EPOLLIN;//表示对应的文件描述符可以读
	event.data.fd=socketfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,socketfd,&event);
	event.data.fd=exitfds[0];
	epoll_ctl(epfd,EPOLL_CTL_ADD,exitfds[0],&event);//注册退出管道的读端以监听是否可读，可读也即要退出了
	int i;
	for(i=0;i<ProcessNum;i++)
	{
		event.data.fd=pManage[i].fds;//因为已通过MakeChild函数得到了每个子进程的pid
		epoll_ctl(epfd,EPOLL_CTL_ADD,pManage[i].fds,&event);//每个子进程的管道对端注册并监控起来
	}
	int idleNum;//空闲子进程数
	int new_fd;//accept后获得的新描述符
	int j;
	char flag;//用来做忙碌与否的标志
	while(1)
	{
		memset(evs,0,(ProcessNum+2)*sizeof(struct epoll_event));
		idleNum=epoll_wait(epfd,evs,ProcessNum+2,-1);//event时间是ProcessNum+1个，空闲子进程数是idleNum个，如果子进程正好全都空闲则idleNum==ProcessNum数
		for(i=0;i<idleNum;i++)
		{
			if(evs[i].data.fd==socketfd)
			{
				new_fd=accept(socketfd,NULL,NULL);
				for(j=0;j<ProcessNum;j++)//要去找到空闲的子进程来给它分配任务
				{
					if(0==pManage[j].busy)
					{
						send_fd(pManage[j].fds,new_fd);
						pManage[j].busy=1;//发送任务给选中的空闲子进程，并将其标记为忙碌状态，1即为忙碌状态
						printf("%d is busy \n",pManage[j].pid);
						break;
					}
				}
				close(new_fd);
			}
			for(j=0;j<ProcessNum;j++)//子进程完成任务该向主进程反馈我已空闲
			{
				if(pManage[j].fds==evs[i].data.fd)
				{
					pManage[j].busy=0;
					read(pManage[j].fds,&flag,sizeof(char));
					printf("%d is not busy\n",pManage[j].pid);
					break;
				}
			}
			if(exitfds[0]==evs[i].data.fd)//如果客户端发送10号信号，则会在上面的sigfunc里面写入退出管道写端，那么读端就变得可读
			{
				event.events=EPOLLIN;
				event.data.fd=socketfd;
				epoll_ctl(epfd,EPOLL_CTL_DEL,socketfd,&event);//把socketfd从集合里面删除出去
				close(socketfd);
				for(j=0;j<ProcessNum;j++)
				{
					send_fd(pManage[j].fds,-1);//就在childhandler里让子进程都退出
				}
				for(j=0;j<ProcessNum;j++)
				{
					wait(NULL);//回收已经退出的子进程的资源
					printf("child exit success\n");//回收成功一个打印一个
				}
				exit(0);//父进程也退出
			}
		}
	}
	return 0;
}

