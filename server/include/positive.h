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
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>
#include "iniparser.h"
#include "dictionary.h"
#include <vector>
#define HTTP_PORT 8088
#define _MAX_SOCKFD_COUNT 50000
//����80M���ļ��������ڴ���ֻ��¼һ�»�����Ϣ
#define MAX_FILE_SIZE_IN_POOL 1024*1024*80
//����ļ���С����2M�����̳߳�������
#define MAX_FILE_SIZE_NEED_THREAD 1024*1024*2
#define MAX_FILE_SIZE 1024*1024*600
#define HTTP_BUFFER_SIZE 1024*1024
#define HTTP_HEAD_SIZE 1024
#define ERROR_BUFFER_SIZE 
#define THREAD_NUM  100
#define POSITIVE_EPOLLIN  EPOLLIN|EPOLLET
#define POSITIVE_EPOLLOUT EPOLLOUT|EPOLLET
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
	int length;
	char name[160];
	char *buffer;
	time_t startTime;
	time_t accessTime;//������ʱ��
	int  fileState; //�ļ�״̬ 0,��ʾ�����Ӵ�����1��ʾ��������
	int  accessCount;//�ļ������ʴ���ͳ��
	positive_pool_t*next;
	positive_pool_t*start;
	positive_pool_t*end;
};

class PositiveServer
{
	public:
		static positive_pool_t *positive_pool;//�ڴ��
		PositiveServer();
		~PositiveServer();
		void initDaemon();//��ʼ���ػ�����
		bool InitServer();
		static void *ListenThread(void * lpVoid);
		void Run();
		//�ڴ�ز���
		static void *fileHandler(void *lpVoid); //�ļ�����
		static void * poolHandler(void *lpVoid);//�������ڴ�ؽ��д���
		void initFiles();
		void loadConfig();
		void scan_dir(string basePath, vector<string>&files,bool searchSubDir);
		static positive_pool_t*  sortLink(positive_pool_t *head);
		static string m_web_root;
	private:
		vector<string> m_webfiles;
		int m_http_port;
		int m_max_socket_num;
		int m_max_file_size_in_pool;
		int m_http_buffer_size;
		int m_http_head_size;
		int m_thread_num;
		static int   m_iEpollFd;
		int	  m_iSock;
		pthread_t m_ListenThreadId;//�����߳̾��
		pthread_t m_FileHandlerThreadId;//�ļ������߳̾��
		pthread_t m_PoolHandlerThreadId;//�ڴ�ع����߳̾��
		epoll_event events[_MAX_SOCKFD_COUNT];
};

#endif