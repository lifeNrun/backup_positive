#include "positiveHttp.h"
using namespace std;
pthread_mutex_t mutex;

PositiveServer::PositiveServer()
{
}

PositiveServer::~PositiveServer()
{
    close(m_iSock);
}
struct epoll_event events[_MAX_SOCKFD_COUNT];
int PositiveServer::m_iEpollFd;
positive_pool_t* PositiveServer::positive_pool;

bool PositiveServer::InitServer(int iPort)
{
	//监听创建
    m_iEpollFd = epoll_create1(0);
	int keepAlive = 1;//开启keepalive属性
	int keepIdle = 60;//如果60秒内没有任何数据往来，则进行探测
	int keepInterval = 6;//探测时发包的时间间隔为6秒
	int keepCount = 3;//探测尝试的次数，如果第一次探测包就收到相应，以后的2次就不再发
    /* Non-blocking I/O.  */
    if(fcntl(m_iEpollFd, F_SETFL, O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(-1);
    }

    m_iSock = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > m_iSock)
    {
        perror("socket");
        exit(1);
    }


    sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(iPort);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int ireuseadd_on = 1;//支持端口复用
    //
    setsockopt(m_iSock, SOL_SOCKET, SO_REUSEADDR, &ireuseadd_on, sizeof(ireuseadd_on));
	//keepalive
	setsockopt(m_iSock,SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(int));
	setsockopt(m_iSock,SOL_TCP,TCP_KEEPIDLE,(void*)&keepIdle, sizeof(int));
	setsockopt(m_iSock,SOL_TCP, TCP_KEEPINTVL,(void*)&keepInterval,sizeof(int));
	setsockopt(m_iSock,SOL_TCP, TCP_KEEPCNT,(void*)&keepCount,sizeof(int));
	
    if(-1 == bind(m_iSock, (sockaddr*)&listen_addr,sizeof(listen_addr)))
    {
        perror("bind");
        exit(-1);
    }

    if(-1 == listen(m_iSock, MAXNUM))
    {
        perror("listen");
        exit(-1);
    }
    else
    {
        printf("服务端监听中...\n");
    }

    //监听线程，此线程负责接收客户端连接，加入到epoll中
    if(-1 == pthread_create(&m_ListenThreadId, 0, ListenThread, this))
    {
        perror("pthread_create");
        exit(-1);
    }
    return true;
}
//监听线程
void *PositiveServer::ListenThread(void* lpVoid)
{
    PositiveServer *pTerminalServer = (PositiveServer*)lpVoid;
    sockaddr_in remote_addr;
    int len = sizeof(remote_addr);

    while(true)//监听连接
    {
        int client_socket = accept(pTerminalServer->m_iSock,(sockaddr*)&remote_addr,(socklen_t*)&len);
        if(client_socket < 0)
        {
            //perror("accept");
            continue;
        }
        else
        {
            struct epoll_event ev;
            //起始设置只监听读
            ev.events = EPOLLIN|EPOLLERR|EPOLLHUP;
            ev.data.fd = client_socket;
            epoll_ctl(pTerminalServer->m_iEpollFd, EPOLL_CTL_ADD, client_socket, &ev);
        }
    }
}
//逻辑辑上好像有点问题，两个线程存在两个http对象啊。
void *PositiveServer::clientEventReadHandler(void* lpVoid)
{
	positiveHttp http;
	int client_socket = 0;
	int nfds = 0;
	while(true)
	{
		pthread_mutex_lock(&mutex);//get lock
		nfds = epoll_wait(m_iEpollFd, events, _MAX_SOCKFD_COUNT, -1);
		//pthread_mutex_unlock(&mutex);//release lock
		//printf("nfds : %d\n",nfds);
		//pthread_mutex_lock(&mutex);//get lock
		for(int i = 0; i < nfds; ++i)
		{
			client_socket = events[i].data.fd;
			//cout << "client  " << events[i].data.fd <<" events : "<<events[i].events<< endl;
			if(events[i].events & EPOLLIN)//监听到读事件
			{
				http.recvHttpRequest(client_socket, m_iEpollFd);
			}
			//写事件不做处理
			else if(events[i].events &EPOLLOUT)
			{
			
				http.sendHttpResponse(client_socket, m_iEpollFd);
			}
			else
			{
				cout << "EPOLL ERROR"<<endl;
				epoll_ctl(m_iEpollFd, EPOLL_CTL_DEL, client_socket, &events[i]);
			}
			
		}
		pthread_mutex_unlock(&mutex);//release lock
		
		if(nfds == 0)
		{
			//printf("===cross===\n");
		}
	}
}

void *PositiveServer::clientEventWriteHandler(void* lpVoid)
{
	positiveHttp http;
	int client_socket = 0;
	int nfds = 0;
	while(true)
	{
		pthread_mutex_lock(&mutex);//get lock
		nfds = epoll_wait(m_iEpollFd, events, _MAX_SOCKFD_COUNT, -1);
		//printf("nfds : %d\n",nfds);
		
		for(int i = 0; i < nfds; ++i)
		{
			client_socket = events[i].data.fd;
			//cout << "client  " << events[i].data.fd <<" events : "<<events[i].events<< endl;
			if(events[i].events & EPOLLIN)//监听到读事件不做处理
			{
				http.recvHttpRequest(client_socket, m_iEpollFd);
			}
			//写事件处理
			else if(events[i].events &EPOLLOUT)
			{
				http.sendHttpResponse(client_socket, m_iEpollFd);
			}
			else
			{
				cout << "EPOLL ERROR"<<endl;
				epoll_ctl(m_iEpollFd, EPOLL_CTL_DEL, client_socket, &events[i]);
			}
			
		}
		pthread_mutex_unlock(&mutex);//release lock
		
		
		if(nfds == 0)
		{
			//printf("===cross===\n");
		}
	}
}
//处理程序
void PositiveServer::Run()
{
    //int client_socket;
    printf("run Server\n");
	pthread_mutex_init(&mutex, NULL);//init mutex
	#if 0
	if(-1 == pthread_create(&m_ClientHandlerThreadId, 0, clientEventReadHandler, NULL))
	{
		perror("pthread_create");
		exit(-1);
	}
	
	if(-1 == pthread_create(&m_ClientHandlerThreadId, 0, clientEventWriteHandler, NULL))
	{
		perror("pthread_create");
		exit(-1);
	}
	#endif
	//pthread_exit(NULL);
	positiveHttp http;
	int client_socket = 0;
	int nfds = 0;
	for(;;)
	{
		//pthread_mutex_lock(&mutex);
		//epoll_wait(m_iEpollFd, events, _MAX_SOCKFD_COUNT, -1);
		//pthread_mutex_unlock(&mutex);//release lock
		nfds = epoll_wait(m_iEpollFd, events, _MAX_SOCKFD_COUNT, -1);
		for(int i = 0; i < nfds; ++i)
		{
			client_socket = events[i].data.fd;
			//cout << "client  " << events[i].data.fd <<" events : "<<events[i].events<< endl;
			if(events[i].events & EPOLLIN)//监听到读事件不做处理
			{
				http.recvHttpRequest(client_socket, m_iEpollFd);
			}
			//写事件处理
			else if(events[i].events &EPOLLOUT)
			{
				http.sendHttpResponse(client_socket, m_iEpollFd);
			}
			else
			{
				cout << "EPOLL ERROR"<<endl;
				epoll_ctl(m_iEpollFd, EPOLL_CTL_DEL, client_socket, &events[i]);
			}
			
		}
	}
}
//把files加入到内存池
void PositiveServer::initFiles()
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if((dp = opendir("html")) == NULL)
	{
		perror("opendir html");
		exit(-1);
	}
	chdir("html"); 
	cout<<"open dir html success"<<endl;
	FILE *fp;
	positive_pool_t* temp;
	positive_pool = (positive_pool_t*)malloc(sizeof(positive_pool_t));
	positive_pool->start = positive_pool;
	if(positive_pool == NULL)
	{
		perror("malloc positive_pool");
		exit(-1);
	}
	temp =  positive_pool;
	while((entry = readdir(dp)) != NULL)
	{
		lstat(entry->d_name,&statbuf);
		if(S_ISREG(statbuf.st_mode))
		{
			strcpy(temp->name,entry->d_name);
			cout<<temp->name<<endl;
			fp = fopen(temp->name,"r");
			if(NULL == fp)
			{
				perror("fopen");
			}
			temp->length = statbuf.st_size;
			temp->buffer = (char*)malloc(statbuf.st_size*sizeof(char));
			fread(temp->buffer,sizeof(char),statbuf.st_size,fp);
			fclose(fp);
			temp->next = (positive_pool_t*)malloc(sizeof(positive_pool_t));
			if(temp->next == NULL)
			{
				perror("malloc temp");
				exit(-1);
			}
			positive_pool->end = temp;
			temp = temp->next;
		}
	}
	free(temp);//删除临时节点
	temp = NULL;
	#if 0
	while(positive_pool != NULL)
	{
		cout<<positive_pool->name<<endl;
		cout<<positive_pool->buffer<<endl;
		positive_pool = positive_pool->next;
	}
	#endif
	if(-1 == pthread_create(&m_ClientHandlerThreadId,0,fileHandler,NULL))
	{
		perror("pthread_create");
		exit(-1);
	}
	//exit(0);
	
}
//检查目录下的文件是否有所改动
void* PositiveServer::fileHandler(void *lpVoid)
{
	positive_pool_t* temp;
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	int flag = 0;
	FILE *fp;
	while(1)
	{
		if((dp = opendir(".")) == NULL)
		{
			perror("opendir html");
			exit(-1);
		}
		while((entry = readdir(dp)) != NULL)
		{
			if(lstat(entry->d_name,&statbuf) == -1)
			{
				perror("lstat");
				exit(-1);
			}
			
			if(S_ISREG(statbuf.st_mode))
			{
				temp =  positive_pool->start;
				flag = 0;
				while(temp != NULL)
				{
					if(strcmp(entry->d_name, temp->name) == 0)
					{
						if(statbuf.st_size != temp->length)//文件已经修改，需要重新导入内存
						{
							cout<<entry->d_name<<endl;
							flag = 2;
							fp = fopen(temp->name,"r");
							if(NULL == fp)
							{
								perror("fopen");
								break;
							}
							temp->length = statbuf.st_size;
							//temp->buffer
							cout<<"free temp buffer"<<endl;
							free(temp->buffer);
							temp->buffer = NULL;
							temp->buffer = (char*)malloc(statbuf.st_size*sizeof(char));
							memset(temp->buffer,0,statbuf.st_size*sizeof(char));
							fread(temp->buffer,sizeof(char),statbuf.st_size,fp);
							fclose(fp);
							//cout<<temp->buffer<<endl;
							break;
						}
						else
						{
							flag = 1;//文件不需要修改
							break;
						}
					}
					temp = temp->next;
				}
				if(0 == flag)//添加新的文件进入内存池
				{
					temp = positive_pool->end;
					temp->next = (positive_pool_t*)malloc(sizeof(positive_pool_t));
					temp = temp->next;
					if(temp == NULL)
					{
						perror("malloc");
						exit(-1);
					}
					strcpy(temp->name,entry->d_name);
					cout<<temp->name<<endl;
					fp = fopen(temp->name,"r");
					if(NULL == fp)
					{
						perror("fopen");
					}
					temp->length = statbuf.st_size;
					temp->buffer = (char*)malloc(statbuf.st_size*sizeof(char));
					fread(temp->buffer,sizeof(char),statbuf.st_size,fp);
					fclose(fp);
					positive_pool->end = temp;//调整end节点
					temp->next = NULL;
				}
			}
		}
		//cout<<"end"<<endl;
		//每两秒检查一次
		sleep(2);
	}
}
int main(int argc, char** argv)
{
    PositiveServer server;
	server.initFiles();
    server.InitServer(HTTP_PORT);
    server.Run();
    return 0;
}