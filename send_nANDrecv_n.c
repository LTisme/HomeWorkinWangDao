#include "func.h"

int send_n(int new_fd,train* p,int len)//所谓的循环发送就是能发多少发多少
{//传描述符，传结构体，传结构体的长度
	int total=0;
	int ret=0;
	while(total<len)
	{
		ret=send(new_fd,p+total,len-total,0);//循环接收&发送也是依靠send与recv的
		if(-1 == ret){
			return -1;
		}
		total+=ret;
	}
	return 0;
}

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
