#include "func.h"

int TcpPrepare(int *sfd,char* ip,char* port)
{
	int socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==socketfd){perror("socket");return -1;}
	printf("socketfd=%d\n",socketfd);
	int reuse=1;
	setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
	struct sockaddr_in server;
	memset(&server,0,sizeof(server));
	server.sin_family=AF_INET;
	server.sin_port=htons(atoi(port));
	server.sin_addr.s_addr=inet_addr(ip);
	int ret=bind(socketfd,(struct sockaddr*)&server,sizeof(server));
	if(-1==ret){perror("bind");return -1;}
	listen(socketfd,10);
	*sfd=socketfd;
	return 0;
}
