#include "func.h"

int transFile(int new_fd)//为了防止协议错乱，先把send都改造成为send_n
{
	train t;
	int ret = 0;
	//发送文件名,先把文件名拷贝到buf里面,再把文件名的长度告知datalen
	strcpy(t.buf,DOWNFILE);
	t.datalen=strlen(DOWNFILE);
	ret = send_n(new_fd,&t,4+t.datalen);//4是int的字节长度，t.datalen是buf（文件名的字节长度）
	if (-1 == ret){
		printf("client close\n");
		return -1;
	}
	int fd=open(DOWNFILE,O_RDONLY);
	//发送文件大小，以告知对方
	if(-1==fd){perror("open");return -1;}
	struct stat statbuf;
	fstat(fd,&statbuf);
	memcpy(t.buf,&statbuf.st_size,sizeof(statbuf.st_size));
	t.datalen=sizeof(statbuf.st_size);
	ret = send_n(new_fd,&t,4+t.datalen);
	if(-1 == ret){
		printf("client close\n");
		return -1;
	}
	//下面是发送文件内容
	while(t.datalen=read(fd,t.buf,sizeof(t.buf)))
	{
		ret = send_n(new_fd,&t,4+t.datalen);
		if(-1 == ret){
			printf("client close\n");
			return -1;
		}
	}
	//下面一句send_n是发送结束标志
	send_n(new_fd,&t,4+t.datalen);
	close(new_fd);
	return 0;
}
