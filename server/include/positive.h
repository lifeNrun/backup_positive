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
//超过80M的文件不放在内存池里，只记录一下基本信息
#define MAX_FILE_SIZE_IN_POOL 1024*1024*80
//如果文件大小超过2M就用线程池来处理
#define MAX_FILE_SIZE_NEED_THREAD 1024*1024*2
#define MAX_FILE_SIZE 1024*1024*600
#define HTTP_BUFFER_SIZE 1024*1024
#define HTTP_HEAD_SIZE 1024
#define ERROR_BUFFER_SIZE 
#define THREAD_NUM  100
#define POSITIVE_EPOLLIN  EPOLLIN|EPOLLET
#define POSITIVE_EPOLLOUT EPOLLOUT|EPOLLET
using namespace std;
//处理字符串的类
class stringOp
{
	public:
	//转换为大写
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
//用于存储服务端网络文件的内存池
struct positive_pool_s
{
	int length;
	char name[160];
	char *buffer;
	time_t startTime;
	time_t accessTime;//被访问时间
	int  fileState; //文件状态 0,表示可增加次数，1表示不可增加
	int  accessCount;//文件被访问次数统计
	positive_pool_t*next;
	positive_pool_t*start;
	positive_pool_t*end;
};

class PositiveServer
{
	public:
		static positive_pool_t *positive_pool;//内存池
		PositiveServer();
		~PositiveServer();
		void initDaemon();//初始化守护进程
		bool InitServer();
		static void *ListenThread(void * lpVoid);
		void Run();
		//内存池操作
		static void *fileHandler(void *lpVoid); //文件处理
		static void * poolHandler(void *lpVoid);//对整个内存池进行处理
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
		pthread_t m_ListenThreadId;//监听线程句柄
		pthread_t m_FileHandlerThreadId;//文件管理线程句柄
		pthread_t m_PoolHandlerThreadId;//内存池管理线程句柄
		epoll_event events[_MAX_SOCKFD_COUNT];
};

#endif