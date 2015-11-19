#ifndef POSITIVE_H
#define POSITIVE_H
#include <sys/epoll.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#define HTTP_PORT 80
#define MAXNUM 6
#define _MAX_SOCKFD_COUNT 100000
#define BUFFER_SIZE 512
//����80M���ļ��������ڴ���ֻ��¼һ�»�����Ϣ
#define MAX_FILE_SIZE 1024*1024*80
#define HTTP_BUFFER_SIZE 1024*80
#define THREAD_NUM  20
using namespace std;
//�����ַ�������
class stringOp
{
	public:
	//ת��Ϊ��д
	static void transformToUpper(string &s)
	{
		if(s.empty())
			return;
		int len = s.length();
		for(int i = 0; i < len;++i)
		{
			if(s[i]>='a'&&s[i]<='z')
				s[i] = 'A' + s[i] - 'a';
		}
	}
};
typedef struct positive_pool_s  positive_pool_t;
//���ڴ洢����������ļ����ڴ��
struct positive_pool_s
{
	//char url[160];
	int length;
	char name[160];
	char *buffer;
	time_t accessTime;//������ʱ��
	int  fileState; //�ļ�״̬
	int  accessCount;//�ļ������ʴ���ͳ��
	positive_pool_t*next;
	positive_pool_t*start;
	positive_pool_t*end;
};

class PositiveServer
{
	public:
	    static positive_pool_t *positive_pool;
		PositiveServer();
		~PositiveServer();
		static void *clientEventReadHandler(void * lpVoid);
		static void *clientEventWriteHandler(void * lpVoid);
		bool InitServer(int iPort);
		static void *ListenThread(void * lpVoid);
		static void *fileHandler(void *lpVoid);
		void initFiles();
		void Run();
	private:
		static int   m_iEpollFd;
		int	  m_iSock;
		pthread_t m_ListenThreadId;//�����߳̾��
		pthread_t m_ClientHandlerThreadId;//�����߳̾��
		//static struct epoll_event events[_MAX_SOCKFD_COUNT];
};

#endif