#include "positiveHttp.h"
#define PERFORMANCE_DEBUG 0
const char  positiveHttp::badRequest[]="<html><head><title>400 Bad Request</title></head><body bgcolor=\"white\"> \
		<center><h1>400 Bad Request</h1></center><hr><center>POSITIVE/1.0.0</center></body></html>";
const char  positiveHttp::notFound[]="<html><head><title>404 Not Found</title></head><body bgcolor=\"white\"> \
		<center><h1>404 Not Found</h1></center><hr><center>POSITIVE/1.0.0</center></body></html>";

bool positiveHttp::checkRequest(int client_socket,positive_http_header_t parseInfo)
{
	//检查method
	if(httpMethod.find(parseInfo.method) == -1)
	{
		//printf("Bad request\n");
		http_response[client_socket] = RES_BAD_METHOD;
		return false;
	}
	
	//检查url
	char *path;
	int length 	= parseInfo.url.length();
	//cout<<"length = "<<length<<endl;
	path = (char*)malloc((length+1)*sizeof(char));
	parseInfo.url.copy(path,length,0);
	path[length] = '\0';
	//printf("path = %s\n",path);
	//对path进行处理
	if(path[0] == '/'&&length!=1)
	{
		//把\0也拷贝了
		memmove(path,path+1,length);
		//printf("checkRequest: path = %s\n",path);
		//requestUrl[client_socket] = path;
	}
	//判断文件是否存在
	positive_pool_t *pool;
	pool = PositiveServer::positive_pool;
	//cout<<"path = "<<path<<endl;
	while(pool != NULL)
	{
		if(strcmp(path,pool->name) == 0)
		{
			//cout<<"pool->name = "<<pool->name<<endl;
			requestUrl[client_socket] = path;
			http_response[client_socket] = RES_OK;
			free(path);
			return true;
		}
		pool = pool->next;
	}
	http_response[client_socket] = RES_BAD_URL;
	requestUrl[client_socket] = "";
	return false;
}
void positiveHttp::recvHttpRequest(int client_socket, int EpollFd)
{
	 char buffer[HTTP_BUFFER_SIZE];
	 string httpRequest;
	 positive_http_header_t parseInfo;
	 memset(buffer, 0, sizeof(buffer));
     int rev_size = recv(client_socket, buffer, HTTP_BUFFER_SIZE,0);
     if(rev_size <= 0)//客户端断开连接
     {
         epoll_event event_del;
         event_del.data.fd = client_socket;
         event_del.events = 0;
         epoll_ctl(EpollFd, EPOLL_CTL_DEL, client_socket, &event_del);
     }
     else//接收客户端发送过来的消息
     {
		 httpRequest.append(buffer);
		 if(parseHttpRequest(httpRequest, &parseInfo) == 0){
			//printRequest(parseInfo);
		 }
		 else{
			 printf("parseError\n");
		 }
		 //检查解析信息
		 checkRequest(client_socket ,parseInfo);
         epoll_event ev;
         //对应文件描述符的监听事件修改为写
         ev.events = EPOLLOUT|EPOLLERR|EPOLLHUP;
         ev.data.fd = client_socket;//记录句柄
         epoll_ctl(EpollFd, EPOLL_CTL_MOD, client_socket,&ev);
     }
}
void positiveHttp::printRequest(positive_http_header_t parseInfo)
{
	cout<<"+---------------------------------+"<<endl;
	cout<<"method:"<<parseInfo.method<<endl;
	cout<<"url:"<<parseInfo.url<<endl;
	cout<<"version:"<<parseInfo.version<<endl;
	cout<<parseInfo.other_info<<endl;
	cout<<"+---------------------------------+"<<endl;
}
//解析对方的请求
int positiveHttp::parseHttpRequest(const string &request, positive_http_header_t* parseInfo)
{
	string marker1("\r\n");
	string marker2("\r\n\r\n");
	
	if(request.empty())
	{
		//printf("http request is empty\n");
		return -1;
	}
	if(NULL == parseInfo)
	{
		//printf("parseInfo is NULL\n");
		return -1;
	}
	int prev = 0, next = 0;
	next = request.find(marker1,prev);
	if(next != string::npos)
	{
		string GETPart(request.substr(prev,next-prev));
		prev = next + marker1.length();
		stringstream sstream(GETPart);
		sstream >> (parseInfo->method);
		sstream >> (parseInfo->url);
		sstream >> (parseInfo->version);
	}
	//处理method
	stringOp::transformToUpper(parseInfo->method);
	//找到
	next = request.find(marker2, prev);
	if(next != string::npos)
	{
		parseInfo->other_info= request.substr(prev,next-prev);
	}

	return 0;
}
void positiveHttp::sendError(int client_socket)
{
	char buffer[HTTP_BUFFER_SIZE];
	int length = 0;
	if(http_response[client_socket] == RES_BAD_METHOD)
	{
		strcpy(buffer,badRequest);
		length = strlen(buffer)+1;
		send(client_socket, buffer, length ,0);
		return;
	}
	else if(http_response[client_socket] == RES_BAD_URL)
	{
		strcpy(buffer,notFound);
		length = strlen(buffer)+1;
		send(client_socket, buffer, length ,0);
	}
	#if PERFORMANCE_DEBUG
		strcpy(buffer,notFound);
		length = strlen(buffer)+1;
		send(client_socket, buffer, length ,0);
	#endif 
	
}
int positiveHttp::sendHttpHead(int client_socket,int length)
{
	string head("HTTP/1.1");
	
	char temp[50];
	string contentType = "html";
	if(http_response[client_socket] == RES_BAD_METHOD)
	{
		head.append(" 400 Bad Request\r\n");
		length = strlen(badRequest);
	}
	else if(http_response[client_socket] == RES_BAD_URL)
	{
		head.append(" 404 Not Found\r\n");
		length = strlen(notFound);
	}
	else{
		//获取contentType
		int typeDotPos = requestUrl[client_socket].rfind('.',string::npos);
		if(typeDotPos != string::npos)
		{
			contentType = requestUrl[client_socket].substr(typeDotPos+1);
			//cout<<contentType<<endl;
			
		}
		head.append(" 200 OK\r\n");
	}
	head.append("Server: Positive\r\n");
	//printf(temp,"Content-Length: %d\r\n",length);
	//head.append(temp);
	head.append("Connection: Close\r\n");
	
	if(supportFiles.find(contentType) != supportFiles.end())
	{
		head.append(supportFiles[contentType]);
	}
	else{
		head.append(supportFiles["html"]);
	}
	
	//cout<<endl<<head<<endl;
	length = head.length();
	char buffer[length];
	bzero(buffer,length);
	head.copy(buffer,length,0);
	//buffer[length] = '\0';
	//发送头
	send(client_socket, buffer, length ,0);
	return 0;
}
void positiveHttp::sendHttpResponse(int client_socket, int EpollFd)
{
	//cout<<"requestUrl "<<requestUrl[client_socket]<<endl;
	char buffer[HTTP_BUFFER_SIZE];
	int sendsize = 0;
	int length = 0;
	//检查要返回的类型
	#if PERFORMANCE_DEBUG //屏蔽掉读文件过程，直接发送Error信息
		sendHttpHead(client_socket,0);
		sendError(client_socket);
	#else 
	if(http_response[client_socket] != RES_OK)
	{
		sendHttpHead(client_socket,0);
		sendError(client_socket);
	}
	else{
		bzero(buffer, HTTP_BUFFER_SIZE); 
		//length = getDataFromUrl(buffer, client_socket);
		sendHttpHead(client_socket,length);
		positive_pool_t *pool;
		pool = PositiveServer::positive_pool;
		int length = requestUrl[client_socket].length();
		char path[length+1];
		requestUrl[client_socket].copy(path,length,0);
		path[length] = '\0';
		while(pool != NULL)
		{
			if(strcmp(path,pool->name) == 0)
			{
				//cout<<"pool name "<<pool->name<<endl;
				//cout<<"pool buffer"<<pool->buffer<<endl;
				strcpy(buffer,pool->buffer);
				break;
			}
			pool = pool->next;
		}
		//cout<<"send buffer"<<buffer<<endl;
		sendsize = send(client_socket, buffer, pool->length ,0);
	}
	#endif
	if(sendsize <= 0)
	{
		struct epoll_event event_del;
		event_del.data.fd = client_socket;
		event_del.events = 0;
		epoll_ctl(EpollFd, EPOLL_CTL_DEL, client_socket, &event_del);
	}
	else
	{
		epoll_event ev;
		ev.events = EPOLLIN|EPOLLERR|EPOLLHUP;
		ev.data.fd = client_socket;
		epoll_ctl(EpollFd, EPOLL_CTL_MOD,client_socket, &ev);
	}
	//把requestUrl[client_socket]设置为空
	requestUrl[client_socket] = "";
	close(client_socket);
}

int positiveHttp::getDataFromUrl(char buffer[],int client_socket)
{
	unsigned long filesize = 0;
	struct stat statbuff;
	int length = requestUrl[client_socket].length();
	char path[length+1];
	requestUrl[client_socket].copy(path,length,0);
	path[length] = '\0';
	//printf("getDataFromUrl:path = %s\n",path);
	if(stat(path,&statbuff)<0){
		return -1;
	}
	else{
		filesize = statbuff.st_size;
	}
	FILE* fp;
	fp = fopen(path,"r");
	if(fp == NULL)
	{
		perror("fopen");
		return -1;
	}
	fread(buffer,sizeof(char),filesize,fp);
	fclose(fp);
	return filesize;
}