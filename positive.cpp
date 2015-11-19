#include "positiveHttp.h"
using namespace std;
pthread_mutex_t mutex;
void printLink(positive_pool_t *head);
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
	time_t now;
	positive_pool = (positive_pool_t*)malloc(sizeof(positive_pool_t));
	positive_pool->start = positive_pool;
	positive_pool->accessCount = 0;
	now = time(NULL);
	positive_pool->startTime  = now;
	positive_pool->accessTime = now;
	
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
			temp->startTime  = now;
			temp->accessTime = now;
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
	positive_pool->end->next = NULL;
	//free(temp);//删除临时节点
	//temp = NULL;
	printLink(positive_pool);
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
	

	if(-1 == pthread_create(&m_ClientHandlerThreadId,0,poolHandler,NULL))
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
					temp->accessCount = 0;
					temp->startTime  = time(NULL);
					temp->accessTime = temp->startTime;
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
positive_pool_t*  sortLink(positive_pool_t *head)
{
	positive_pool_t *pfirst;      /* 排列后有序链的表头指针 */  
    positive_pool_t *ptail;       /* 排列后有序链的表尾指针 */  
    positive_pool_t *pminBefore;  /* 保留键值更小的节点的前驱节点的指针 */  
    positive_pool_t *pmin;        /* 存储最小节点   */  
    positive_pool_t *p;           /* 当前比较的节点 */ 
	positive_pool_t *res = head;
	positive_pool_t *temp = head;
    pfirst = NULL;  
	int flag = 0;
	//printLink(head);
	while(temp->next != NULL)
	{
		if(temp->accessCount < temp->next->accessCount)
		{
			flag = 1;
			break;
		}
		temp = temp->next;
	}
	if(flag == 0)
		return res;
    while (head != NULL)         /*在链表中找键值最小的节点。*/  
    {  
    /* 注意：这里for语句就是体现选择排序思想的地方 */  
        for (p = head, pmin = head; p->next != NULL; p = p->next) /*循环遍历链表中的节点，找出此时最小的节点。*/  
        {  
            if (p->next->accessCount > pmin->accessCount) /*找到一个比当前min小的节点。*/  
            {  
                pminBefore = p;           /*保存找到节点的前驱节点：显然p->next的前驱节点是p。*/  
                pmin       = p->next;     /*保存键值更小的节点。*/  
            }  
        }  
    /*上面for语句结束后，就要做两件事；一是把它放入有序链表中；二是根据相应的条件判断，安排它离开原来的链表。*/  
      
        /*第一件事*/  
        if (pfirst == NULL)     /* 如果有序链表目前还是一个空链表                      */  
        {  
            pfirst = pmin;      /* 第一次找到键值最小的节点。                          */  
            ptail  = pmin;      /* 注意：尾指针让它指向最后的一个节点。                */  
        }  
        else                    /* 有序链表中已经有节点                                */  
        {  
            ptail->next = pmin; /* 把刚找到的最小节点放到最后，即让尾指针的next指向它。*/  
            ptail = pmin;       /* 尾指针也要指向它。                                  */  
        }  
  
        /*第二件事*/  
        if (pmin == head)        /* 如果找到的最小节点就是第一个节点                    */  
        {  
            head = head->next;   /* 显然让head指向原head->next,即第二个节点，就OK       */  
        }  
        else /*如果不是第一个节点*/  
        {  
            pminBefore->next = pmin->next; /*前次最小节点的next指向当前pmin的next,这样就让pmin离开了原链表。*/  
        }  
    }  
  
    if (pfirst != NULL)     /*循环结束得到有序链表first                */  
    {  
        ptail->next = NULL; /*单向链表的最后一个节点的next应该指向NULL */   
    }  
    //head = pfirst; 
#if 0	
    positive_pool_t * pReversedHead = NULL; // 反转后的新链表头指针，初始为NULL  
    positive_pool_t * pCurrent = pfirst->next;  
    while(pCurrent != NULL)  
    {  
        positive_pool_t * pTemp = pCurrent;  
        pCurrent = pCurrent->next;  
        pTemp->next = pReversedHead; // 将当前结点摘下，插入新链表的最前端  
        pReversedHead = pTemp;  
    }   
    return pReversedHead;  
#endif
//printLink(pfirst);
return pfirst;
}
void printLink(positive_pool_t *head)
{
	positive_pool_t *temp = head;
	cout<<"\n+---------------------------+"<<endl;
	while(temp != NULL)
	{
		cout<<temp->name<<":"<<temp->accessCount<<":"<<temp->fileState<<endl;
		temp = temp->next;
	}
	cout<<"+---------------------------+"<<endl;
}
void setState(positive_pool_t *head, int state)
{
	positive_pool_t *temp = head;
	time_t now;
	
	now = time(NULL);
	while(temp != NULL)
	{
		temp->fileState = state;
		if((now - temp->accessTime)>10);//超过10分钟没有被访问，那么访问次数减半，相应的在内存池的位置也会靠后
			temp->accessCount = temp->accessCount/2;
		temp = temp->next;
	}
}
void* PositiveServer::poolHandler(void *lpVoid)
{
	int accessCount = 0;
	int flag = 0;
	
	//排序
	for(;;)
	{
		//printLink(positive_pool->start);
		setState(positive_pool,1);
		positive_pool = sortLink(positive_pool);
		setState(positive_pool,0);
		positive_pool->start = positive_pool;
		printLink(positive_pool->start);
		sleep(20);
		//positive_pool->next->next->next->accessCount = 1;
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