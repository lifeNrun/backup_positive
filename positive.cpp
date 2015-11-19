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
	free(temp);//ɾ����ʱ�ڵ�
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
int main(int argc, char** argv)
{
    PositiveServer server;
	server.initFiles();
    server.InitServer(HTTP_PORT);
    server.Run();
    return 0;
}