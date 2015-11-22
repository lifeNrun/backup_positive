#include "positiveHttp.h"
#define PERFORMANCE_DEBUG 0
#define HIGH_PERFORMANCE 0
#define THREAD_POOL 1
const char  positiveHttp::badRequest[]="<html><head><title>400 Bad Request</title></head><body bgcolor=\"white\"> \
		<center><h1>400 Bad Request</h1></center><hr><center>POSITIVE/1.0.0</center></body></html>";
const char  positiveHttp::notFound[]="<html><head><title>404 Not Found</title></head><body bgcolor=\"white\"> \
		<center><h1>404 Not Found</h1></center><hr><center>POSITIVE/1.0.0</center></body></html>";
const char  positiveHttp::notFoundHead[]="HTTP/1.1 302 not Found\r\nServer: Positive\r\nContent-Length: 222\r\nConnection: Close \
\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
//void* thread_routing(void *arg);
thread_pool * positiveHttp::pos_thread_pool = NULL;
void* positiveHttp::thread_routine(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&(pos_thread_pool->queue_lock));
		while((pos_thread_pool->cur_queue_size == 0)&& (!pos_thread_pool->shutdown))
		{
			pthread_cond_wait(&(pos_thread_pool->queue_ready),&(pos_thread_pool->queue_lock));
		}
		
		if(pos_thread_pool->shutdown)
		{
			pthread_mutex_unlock(&(pos_thread_pool->queue_lock));
			pthread_exit(NULL);
		}
		
		assert(pos_thread_pool->cur_queue_size != 0);
		assert(pos_thread_pool->queue_head != NULL);
		
		pos_thread_pool->cur_queue_size--;
		worker_thread *worker = pos_thread_pool->queue_head;
		pos_thread_pool->queue_head = worker->next;
		pthread_mutex_unlock(&(pos_thread_pool->queue_lock));
		//执行process
		(*(worker->process))(worker->arg1,worker->arg2);
		//执行完毕后，释放worker
		free(worker);
		worker = NULL;
	}
	pthread_exit(NULL);
}
void positiveHttp::pool_init(int max_thread_num)
{
	pos_thread_pool = (thread_pool*)malloc(sizeof(thread_pool));
	pthread_mutex_init(&(pos_thread_pool->queue_lock),NULL);
	pthread_cond_init(&(pos_thread_pool->queue_ready),NULL);
	
	pos_thread_pool->queue_head = NULL;
	pos_thread_pool->max_thread_num = max_thread_num;
	pos_thread_pool->cur_queue_size = 0;
	pos_thread_pool->shutdown = 0;
	pos_thread_pool->threadid = (pthread_t*)malloc(max_thread_num*sizeof(pthread_t));
	for(int i = 0; i < max_thread_num; ++i)
	{
		pthread_create(pos_thread_pool->threadid,NULL,thread_routine,NULL);
	}
}

int positiveHttp::pool_add_worker(void*(*process)(void*arg1,void*arg2), void*arg1,void *arg2)
{
	worker_thread *worker = (worker_thread*)malloc(sizeof(worker_thread));
	worker->process = process;
	worker->arg1 = arg1;
	worker->arg2 = arg2;
	worker->next = NULL;
	pthread_mutex_lock(&(pos_thread_pool->queue_lock));
	worker_thread *member = pos_thread_pool->queue_head;
	if(member != NULL)
	{
		while(member->next != NULL)
			member = member->next;
		member->next = worker;
	}
	else{
		pos_thread_pool->queue_head = worker;
	}
	assert(pos_thread_pool->queue_head!=NULL);
	pos_thread_pool->cur_queue_size++;
	pthread_mutex_unlock(&(pos_thread_pool->queue_lock));
	pthread_cond_signal(&(pos_thread_pool->queue_ready));
	return 0;
}
void* positiveHttp::process (void *arg1, void*arg2){
	positive_pool_t *memPool = (positive_pool_t*)arg1;
	long  client_socket = (long)arg2;
	//从epoll队列里把client_socket清理掉
	//struct epoll_event event_del;
	//event_del.data.fd = client_socket;
	//event_del.events = 0;
	//epoll_ctl(EpollFd, EPOLL_CTL_DEL, client_socket, &event_del);
	
	int length = HTTP_BUFFER_SIZE;
	int left = memPool->length;
	int i = 0;
	int sendsize = 0;
	//cout<<"client "<<client_socket<<endl;
	while(1){
		if(sendsize == -1)
		{
			cout<<"client "<<client_socket<<" -1"<<endl;
			perror("send");
		}
		if(sendsize != -1)
		{
			i = i + sendsize;
			left = left - sendsize;
			if(left <= 0)
				break;
			if(left < HTTP_BUFFER_SIZE)
				length = left;
		}
		sendsize = send(client_socket, memPool->buffer+i, length,0);
	}
	close(client_socket);
	return NULL;
}
int positiveHttp::pool_destroy()
{
	if(pos_thread_pool->shutdown)
		return -1;
	pos_thread_pool->shutdown = 1;
	pthread_cond_broadcast(&(pos_thread_pool->queue_ready));
	for(int i = 0; i < pos_thread_pool->max_thread_num;++i)
		pthread_join(pos_thread_pool->threadid[i], NULL);
	free(pos_thread_pool->threadid);
	worker_thread *head = NULL;
	while(pos_thread_pool->queue_head != NULL)
	{
		head = pos_thread_pool->queue_head;
		pos_thread_pool->queue_head = pos_thread_pool->queue_head->next;
		free(head);
	}
	pthread_mutex_destroy(&(pos_thread_pool->queue_lock));
	pthread_cond_destroy(&(pos_thread_pool->queue_ready));
	free(pos_thread_pool);
	pos_thread_pool = NULL;
	return 0;
}
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
	free(path);
	http_response[client_socket] = RES_BAD_URL;
	requestUrl[client_socket] = "";
	return false;
}
void positiveHttp::recvHttpRequest(int client_socket, int EpollFd)
{
	 #if HIGH_PERFORMANCE
	 //高性能处理
	 char buffer[HTTP_HEAD_SIZE];
	 string httpRequest;
	 positive_http_header_t parseInfo;
	 memset(buffer, 0, sizeof(buffer));
	 int rev_size = recv(client_socket, buffer, HTTP_HEAD_SIZE,0);
	 epoll_event ev;
	 	//对应文件描述符的监听事件修改为写
	 ev.events = POSITIVE_EPOLLOUT;
	 ev.data.fd = client_socket;//记录句柄
	 epoll_ctl(EpollFd, EPOLL_CTL_MOD, client_socket,&ev);
	#else
		
	 char buffer[HTTP_HEAD_SIZE];
	 string httpRequest;
	 positive_http_header_t parseInfo;
	 memset(buffer, 0, sizeof(buffer));
     int rev_size = recv(client_socket, buffer, HTTP_HEAD_SIZE,0);
     if(rev_size <= 0)//客户端断开连接
     {
         epoll_event event_del;
         event_del.data.fd = client_socket;
         event_del.events = 0;
         epoll_ctl(EpollFd, EPOLL_CTL_DEL, client_socket, &event_del);
		 close(client_socket);
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
         ev.events = POSITIVE_EPOLLOUT;
         ev.data.fd = client_socket;//记录句柄
         epoll_ctl(EpollFd, EPOLL_CTL_MOD, client_socket,&ev);
     }
	 #endif 
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
	
	if(http_response[client_socket] == RES_BAD_METHOD)
	{
		send(client_socket, badRequest, sizeof(badRequest),0);
		return;
	}
	else if(http_response[client_socket] == RES_BAD_URL)
	{
		send(client_socket, notFound, sizeof(notFound) ,0);
	}
	#if PERFORMANCE_DEBUG
		send(client_socket, notFound, sizeof(notFound) ,0);
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
	int sendsize = 0;
	int length = 0;
	#if HIGH_PERFORMANCE
	sendsize = send(client_socket, notFoundHead, sizeof(notFoundHead),0);
	sendsize = send(client_socket, notFound, sizeof(notFound),0);
	close(client_socket);
	struct epoll_event event_del;
	event_del.data.fd = client_socket;
	event_del.events = 0;
	epoll_ctl(EpollFd, EPOLL_CTL_DEL, client_socket, &event_del);
	return;
	#endif
	
	//检查要返回的类型
	#if PERFORMANCE_DEBUG //屏蔽掉读文件过程，直接发送Error信息
		sendHttpHead(client_socket,0);
		sendError(client_socket);
		close(client_socket);
		
	#else 
	if(http_response[client_socket] != RES_OK)
	{
		sendHttpHead(client_socket,0);
		sendError(client_socket);
		close(client_socket);
	}
	else{
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
				if(pool->fileState == 0)
				{
					pool->accessCount++;//被访问次数加1
					pool->accessTime = time(NULL);//被访问的时间更新
				}
				break;
			}
			pool = pool->next;
		}
		
		//如果文件超过最大值，就用从线程池唤醒一个线程来发送数据
	#if THREAD_POOL
		if(pool->length >= MAX_FILE_SIZE)
		{
			//int *arg2 = &client_socket;
			//void*(*ptr)(void*arg1,void*arg2);
			//ptr = process;
			//ptr = process;
			//process(2,3);
			//process必须变为静态的成员变量
			//cout<<"client socket "<<client_socket<<endl;
			//cout<<"mempool length "<<pool->length<<endl;
			pool_add_worker(process,pool,(void*)client_socket);
		}
		else{
			length = HTTP_BUFFER_SIZE;
			int left = pool->length;
			int i = 0;
			//char tempbuffer[HTTP_BUFFER_SIZE];
			//bzero(tempbuffer,sizeof(tempbuffer));
			sendsize = 0;
			//void *ptr;
			while(1){
				if(sendsize != -1)
				{
					i = i + sendsize;
					left = left - sendsize;
					if(left <= 0)
						break;
					if(left < HTTP_BUFFER_SIZE)
						length = left;
				}
				sendsize = send(client_socket, pool->buffer+i, length,0);
			}
			close(client_socket);
		}
	#else
		length = HTTP_BUFFER_SIZE;
		int left = pool->length;
		int i = 0;
		//char tempbuffer[HTTP_BUFFER_SIZE];
		//bzero(tempbuffer,sizeof(tempbuffer));
		sendsize = 0;
		//void *ptr;
		while(1){
			if(sendsize != -1)
			{
				i = i + sendsize;
				left = left - sendsize;
				if(left <= 0)
					break;
				if(left < HTTP_BUFFER_SIZE)
					length = left;
			}
			sendsize = send(client_socket, pool->buffer+i, length,0);
		}
		close(client_socket);
	#endif
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
		ev.events = POSITIVE_EPOLLIN;
		ev.data.fd = client_socket;
		epoll_ctl(EpollFd, EPOLL_CTL_MOD,client_socket, &ev);
	}
	//把requestUrl[client_socket]设置为空
	requestUrl[client_socket] = "";
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