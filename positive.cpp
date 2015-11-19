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
	//��������
    m_iEpollFd = epoll_create1(0);
	int keepAlive = 1;//����keepalive����
	int keepIdle = 60;//���60����û���κ����������������̽��
	int keepInterval = 6;//̽��ʱ������ʱ����Ϊ6��
	int keepCount = 3;//̽�Ⳣ�ԵĴ����������һ��̽������յ���Ӧ���Ժ��2�ξͲ��ٷ�
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

    int ireuseadd_on = 1;//֧�ֶ˿ڸ���
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
        printf("����˼�����...\n");
    }

    //�����̣߳����̸߳�����տͻ������ӣ����뵽epoll��
    if(-1 == pthread_create(&m_ListenThreadId, 0, ListenThread, this))
    {
        perror("pthread_create");
        exit(-1);
    }
    return true;
}
//�����߳�
void *PositiveServer::ListenThread(void* lpVoid)
{
    PositiveServer *pTerminalServer = (PositiveServer*)lpVoid;
    sockaddr_in remote_addr;
    int len = sizeof(remote_addr);

    while(true)//��������
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
            //��ʼ����ֻ������
            ev.events = EPOLLIN|EPOLLERR|EPOLLHUP;
            ev.data.fd = client_socket;
            epoll_ctl(pTerminalServer->m_iEpollFd, EPOLL_CTL_ADD, client_socket, &ev);
        }
    }
}
//�߼����Ϻ����е����⣬�����̴߳�������http���󰡡�
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
			if(events[i].events & EPOLLIN)//���������¼�
			{
				http.recvHttpRequest(client_socket, m_iEpollFd);
			}
			//д�¼���������
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
			if(events[i].events & EPOLLIN)//���������¼���������
			{
				http.recvHttpRequest(client_socket, m_iEpollFd);
			}
			//д�¼�����
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
//�������
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
			if(events[i].events & EPOLLIN)//���������¼���������
			{
				http.recvHttpRequest(client_socket, m_iEpollFd);
			}
			//д�¼�����
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
//��files���뵽�ڴ��
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
	//free(temp);//ɾ����ʱ�ڵ�
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
//���Ŀ¼�µ��ļ��Ƿ������Ķ�
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
						if(statbuf.st_size != temp->length)//�ļ��Ѿ��޸ģ���Ҫ���µ����ڴ�
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
							flag = 1;//�ļ�����Ҫ�޸�
							break;
						}
					}
					temp = temp->next;
				}
				if(0 == flag)//����µ��ļ������ڴ��
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
					positive_pool->end = temp;//����end�ڵ�
					temp->next = NULL;
				}
			}
		}
		//cout<<"end"<<endl;
		//ÿ������һ��
		sleep(2);
	}
}
positive_pool_t*  sortLink(positive_pool_t *head)
{
	positive_pool_t *pfirst;      /* ���к��������ı�ͷָ�� */  
    positive_pool_t *ptail;       /* ���к��������ı�βָ�� */  
    positive_pool_t *pminBefore;  /* ������ֵ��С�Ľڵ��ǰ���ڵ��ָ�� */  
    positive_pool_t *pmin;        /* �洢��С�ڵ�   */  
    positive_pool_t *p;           /* ��ǰ�ȽϵĽڵ� */ 
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
    while (head != NULL)         /*���������Ҽ�ֵ��С�Ľڵ㡣*/  
    {  
    /* ע�⣺����for����������ѡ������˼��ĵط� */  
        for (p = head, pmin = head; p->next != NULL; p = p->next) /*ѭ�����������еĽڵ㣬�ҳ���ʱ��С�Ľڵ㡣*/  
        {  
            if (p->next->accessCount > pmin->accessCount) /*�ҵ�һ���ȵ�ǰminС�Ľڵ㡣*/  
            {  
                pminBefore = p;           /*�����ҵ��ڵ��ǰ���ڵ㣺��Ȼp->next��ǰ���ڵ���p��*/  
                pmin       = p->next;     /*�����ֵ��С�Ľڵ㡣*/  
            }  
        }  
    /*����for�������󣬾�Ҫ�������£�һ�ǰ����������������У����Ǹ�����Ӧ�������жϣ��������뿪ԭ��������*/  
      
        /*��һ����*/  
        if (pfirst == NULL)     /* �����������Ŀǰ����һ��������                      */  
        {  
            pfirst = pmin;      /* ��һ���ҵ���ֵ��С�Ľڵ㡣                          */  
            ptail  = pmin;      /* ע�⣺βָ������ָ������һ���ڵ㡣                */  
        }  
        else                    /* �����������Ѿ��нڵ�                                */  
        {  
            ptail->next = pmin; /* �Ѹ��ҵ�����С�ڵ�ŵ���󣬼���βָ���nextָ������*/  
            ptail = pmin;       /* βָ��ҲҪָ������                                  */  
        }  
  
        /*�ڶ�����*/  
        if (pmin == head)        /* ����ҵ�����С�ڵ���ǵ�һ���ڵ�                    */  
        {  
            head = head->next;   /* ��Ȼ��headָ��ԭhead->next,���ڶ����ڵ㣬��OK       */  
        }  
        else /*������ǵ�һ���ڵ�*/  
        {  
            pminBefore->next = pmin->next; /*ǰ����С�ڵ��nextָ��ǰpmin��next,��������pmin�뿪��ԭ����*/  
        }  
    }  
  
    if (pfirst != NULL)     /*ѭ�������õ���������first                */  
    {  
        ptail->next = NULL; /*������������һ���ڵ��nextӦ��ָ��NULL */   
    }  
    //head = pfirst; 
#if 0	
    positive_pool_t * pReversedHead = NULL; // ��ת���������ͷָ�룬��ʼΪNULL  
    positive_pool_t * pCurrent = pfirst->next;  
    while(pCurrent != NULL)  
    {  
        positive_pool_t * pTemp = pCurrent;  
        pCurrent = pCurrent->next;  
        pTemp->next = pReversedHead; // ����ǰ���ժ�£��������������ǰ��  
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
		if((now - temp->accessTime)>10);//����10����û�б����ʣ���ô���ʴ������룬��Ӧ�����ڴ�ص�λ��Ҳ�῿��
			temp->accessCount = temp->accessCount/2;
		temp = temp->next;
	}
}
void* PositiveServer::poolHandler(void *lpVoid)
{
	int accessCount = 0;
	int flag = 0;
	
	//����
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