#include "func.h"

int recv_n(int new_fd,char* p,int len)//实际能接多少接多少
{
	int total=0;
	int ret=0;
	while(total<len)
	{
		ret=recv(new_fd,p+total,len-total,0);
		total+=ret;
	}
	return 0;
}


int main(int argc,char** argv)
{
	args_check(argc,3);
	int socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==socketfd){perror("socket");return -1;}
	printf("socketfd=%d\n",socketfd);
	struct sockaddr_in server;
	memset(&server,0,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(atoi(argv[2]));//端口号
	server.sin_addr.s_addr=inet_addr(argv[1]);//IP地址
	int ret=connect(socketfd,(struct sockaddr*)&server,sizeof(struct sockaddr));
	if(-1==ret){perror("connect");return -1;}
	int datalen;
	char buf[1000]={0};//设定每次最大接收1000
	recv_n(socketfd,(char*)&datalen,sizeof(int));
	//先接文件名的长度
	recv_n(socketfd,buf,datalen);
	//接下来接文件名到buf里面
	off_t FileTotalSize,FileLoadSize=0;//已下载长度和总长度
	recv_n(socketfd,(char*)&datalen,sizeof(int));
	//接文件大小，因要做进度条
	recv_n(socketfd,(char*)&FileTotalSize,datalen);
	//把datalen接到off_t型的FileTotalSize里面
	int fd;
	fd=open(buf,O_WRONLY|O_CREAT,0666);//在客户端本地要创建这个文件名的文件呀，所以其实是打开或创建buf里的文件名的文件
	if(-1==fd){perror("open");return -1;}
	time_t start=time(NULL),now;
	while(1)//开始不断的接文件内容
	{
		recv_n(socketfd,(char*)&datalen,sizeof(int));
		if(datalen>0)
		{
			recv_n(socketfd,buf,datalen);//是那种不会发生协议错乱的循环读取
			write(fd,buf,datalen);
			FileLoadSize+=datalen;
			now=time(NULL);//每一次写完取一下当前时间，这比计时器成本低
			if(now-start > 0)
			{
				printf("%5.2f%%\r",(double)FileLoadSize/FileTotalSize*100);
				fflush(stdout);//为了每次都在一个地方变化
				start=now;
			}
		}
		else{
			printf("100.00%%\n");
			close(fd);
			printf("receive success\n");
			break;
		}
	}
	close(socketfd);
}

