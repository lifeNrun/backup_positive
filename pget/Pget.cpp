#include <iostream>
#include <fstream>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <map>
using namespace std;
#define HTTP_BUFFER_SIZE 1024*800
#define ACME "216.27.178.28"
#define BAIDU "180.97.33.108"
#define PICTURE "220.189.220.80"
#define LOCAL "192.168.27.122"
void getIP(char name[],char ip[])
{
	 hostent *host;
	 const char *pIP;
	 string s;
	 char tempStr[32];
	 host = gethostbyname(name);
	 if(host == NULL)
		 exit(-1);
	 //inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
	 if(host->h_addrtype == AF_INET)
	 {
		 cout<<host->h_length<<endl;
		 int i = 0;
		 while(host->h_addr_list[i] != NULL)
		 {
			 pIP = inet_ntop(AF_INET, host->h_addr_list[i], tempStr, sizeof(tempStr));
			 cout<<pIP<<endl;
			 i++;
		 }
	 }
	 strcpy(ip,pIP);
	 cout<<ip<<endl;
	 //exit(0);
}
int getHostName(char *argv, char name[],char path[],char filename[])
{
	
	if(argv == NULL)
		return -1;
	int len = strlen(argv);
	string s(argv);
	int pos = s.find('/',0);
	s.copy(name,pos,0);//-1表示拷贝到末尾
	
	if(pos != -1)
	{
		s.copy(path,-1,pos);
		int pos2 = s.rfind("/",-1);
		cout<<pos2<<endl;
		cout<<len<<endl;
		if(pos2 != len-1)
		{
			s.copy(filename,-1,pos2+1);
		}
		else{
			s.copy(filename,-1,pos+1);
		}
		
	}
	else{
		strcpy(path,"/index.html");
		strcpy(filename,"temp.html");
	}
	cout<<"path " <<path<<endl;
	cout<<"name "<<name<<endl;
	cout<<"filename "<<filename<<endl;
}
int getPath(string head)
{
	return 1;
}
int main(int argc, char**argv)
{
   if(argc != 2)
   {
	   cout<<"bad arguments"<<endl;
   }
   char name[300];
   char filename[300];
   char path[300]; 
   memset(filename,'\0',300*sizeof(char));
   memset(name,'\0',300*sizeof(char));
   memset(path,'\0',300*sizeof(char));
   getHostName(argv[1],name,path,filename);
   int fd = 0, res = 0, len = 0;
   char serverIp[32];
   getIP(name,serverIp);
   const unsigned int server_port = 80;
   //const char *server_ip = LOCAL;
   char getBuf[HTTP_BUFFER_SIZE] = "";
   struct sockaddr_in server;
   
   server.sin_family = AF_INET;
   server.sin_port = htons(server_port);
   inet_pton(AF_INET,serverIp,&server.sin_addr.s_addr);
   
   //create tcp socket
   fd = socket(AF_INET, SOCK_STREAM, 0);
   if(fd < 0)
   {
	   perror("socket");
	   exit(-1);
   }
   //设置套接字fd为非阻塞式IO
   cout << "create socket success fd: "<<fd<<endl;
   
   //connect to server...
   res = connect(fd, (struct sockaddr *)&server, sizeof(server));
   if(res != 0)
   {
	    perror("connect");
		close(fd);
		exit(-1);
   }
   
   cout << "connect to server success."<<endl;
   //char buf[] = "GET./.HTTP/1.0..User-Agent:.Wget/1.11.4..Accept:.*/*..Host:.www.website1.com..Connection:.Keep-Alive....";
   char sendBuf[1024*8];
   sprintf(sendBuf,"GET %s HTTP/1.0\r\nUser-Agent: test\r\nAccept:*/*\r\n \
   Host: %s\r\nConnection: Keep-Alive\r\n\r\n",path,name);
   send(fd, sendBuf, strlen(sendBuf),0);
   
   int recvLen = HTTP_BUFFER_SIZE;
   bzero(getBuf, HTTP_BUFFER_SIZE);
   len=read(fd, getBuf,recvLen);
   cout<<"read len "<<len<<endl;
   string s(getBuf);
  
   //获取http头的尾部位置
   int pos = s.find("\r\n\r\n");
   //获取http头信息
   string sout = s.substr(0,pos+4);
   cout<<sout;
   //cout<<sout<<endl;
   cout<<s.length()<<endl;
   //余下数据的长度
   int otherDataLen = len - pos -4;
   
   cout<<"otherDataLen = "<<otherDataLen<<endl;
   //获取余下数据并写入文件
   char temp[otherDataLen];
   bzero(temp, otherDataLen);
   memcpy(temp,getBuf+pos+4,otherDataLen);
   
   
   FILE *fp = fopen(filename, "w"); 
   //如果有404错误就把文件删除，并关闭socket和文件
   if(s.find("404")!=-1)
   {
	    fclose(fp);
		close(fd);
	    if(remove(filename) != 0)
		{
			perror("remove");
		}
		return -1;
   }
   if(fwrite(temp, sizeof(char), otherDataLen, fp) < otherDataLen) 
   { 

		printf("Write Failed\n"); 
		
		
		return -1;
   } 

   bzero(getBuf, HTTP_BUFFER_SIZE);
   //把socket设置为非阻塞模式，read函数的处理会不一样些
   //fcntl(fd, F_SETFL, O_NONBLOCK);
   //如何处理read的阻塞问题
   //继续从fd读取数据，并写入文件
   while(1)
   {
	   len = read(fd, getBuf,recvLen);
	   //len=recv(fd, getBuf,recvLen,0);//和read一样
	   if(len < 0)
	   {
		    // 当send收到信号时,可以继续写,但这里返回-1.
		   if(errno == EINTR)
				break;
		   // 当socket是非阻塞时,如返回此错误,表示写缓冲队列已满,
           // 在这里做延时后再重/试.
		   if(errno == EAGAIN)
		   {
			   sleep(1);
			   continue;
		   } 
	   }
	   if(fwrite(getBuf, sizeof(char), len, fp) < len) 
	   { 
			printf("temp.html Write Failed\n"); 
			break; 
	   } 
	   bzero(getBuf, HTTP_BUFFER_SIZE);
	   printf("len = %d\n",len);
	   if(len == 0)
		   break;
   }
   fclose(fp);
   close(fd);
   return 0;
}