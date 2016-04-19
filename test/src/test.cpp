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
#include <pthread.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>        // 提供属性操作函数  
#include <sys/types.h>         // 提供mode_t 类型  
#include<vector>
#include<unistd.h>
using namespace std;

#define HTTP_BUFFER_SIZE 1024*800
void testMap()
{
	string s = "jpp";
	map<string,string> supportFiles;
	supportFiles["jpg"] = "image/jpg";
	supportFiles["jpeg"] = "image/jpeg";
	supportFiles["gif"] = "image/gif";
	supportFiles["txt"] = "text/plain";
	supportFiles["html"] = "text/html";
	if(supportFiles.find("jpp") == supportFiles.end())
		cout<<"not found"<<endl;
	exit(0);
}
void testfOpen()
{
	
	exit(0);
}
void transformToUpper(string &s)
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
void testStringFind()
{
	
	string s("GET HEAD POST");
	s[1] = 'P';
	cout<<"s = "<<s<<endl;
	string s1("get1");
	char p[10];
	char name[] = "\r\n\r\n\r\n";
	printf("length of name = %d\n",strlen(name));
    //s1.copy(p,s1.length(),0);
	transformToUpper(s1);
	cout<<s1<<endl;
	s1.clear();
	s1 = "GET";
	cout<<"clear s1 len = "<<s1.length()<<endl;
	//s.copy(p,10,0);
	int pos = s.rfind("POST",3);
	printf("pos=%d\n",pos);
	exit(0);
}
int getContentLength(string s)
{
	int pos1 = 0;
	int pos2 = 0;
	int length = 0;
	char num[33];
	int titleLength = strlen("Content-Length: ");
	pos1 = s.find("Content-Length:",0);
	pos2 = s.find("\r\n");
	s.copy(num,pos2-titleLength-pos1,pos1+titleLength);
	length = atoi(num);
	
    return length;
}
struct temp{
	char name[10];
	int id;
};

void testCharOp()
{
   
   char *name = (char*)malloc(10);
   char str[] = "test";
   int  testNum = 100;
   strcpy(name,"yuan");
   cout<<"name is "<<name<<endl; 
   //说明对name这样的字符串也可以取地址计算长度
   cout<<"strlen is "<<strlen(name)<<endl; 
   void *p = name;
   cout<<"1 *p = "<<*(char*)p<<endl;//*(char*)p相当于name[0],(char*)p此时相当于name
   p = &str[1];//指向字符串的指针
   cout<<"2 *p = "<<(char*)p<<endl;//*(char*)p相当于str[0],(char*)p此时相当于str
   p = &testNum;
   cout<<"3 *p = "<< *(int*)p<<endl;// *(int*)p==100,p是指向testNum的指针
   memset(name,0,10);// 不能写为memset(&name,0,10)
   cout<<"after memset strlen = "<<strlen(name)<<endl; 
   free(name);
   name = NULL;
   int num[5] = {1,2,3,4,5};
   void *s1 = num;//指向起始地址
   cout<<"*s1 = "<<*((int*)s1+1)<<endl;
   void *s = &num[1];//s指向num[1]
   cout<<"void *s = "<<*(int*)s<<endl;//*(int*)s转换为对应的指针类型，在寻址取值
   memset(&num[1],0,4*sizeof(int));// void *s = &num[1];从这个地址开始全设置为0
   memset(num,0,5*sizeof(int));//==memset(&num,0,5*sizeof(int)),传递数组地址
   for(int i = 0; i < 5; ++i)
	   cout<<num[i]<<"";
   cout<<endl;
   temp t;
   strcpy(t.name,"jy");
   cout<<t.name<<endl;
   cout<<t.id<<endl;
   t.id = 230972;
   memset(&t,0,sizeof(t));//不能写为memset(t,0,sizeof(temp));第一个参数需要传递地址
   cout<<t.name<<endl;
   cout<<t.id<<endl;
   exit(0);
}
#define ACME "216.27.178.28"
#define BAIDU "180.97.33.108"
#define PICTURE "220.189.220.80"
#define LOCAL "192.168.27.122"
//不过要设置一下timeout,可参考wget里面的run_with_timeout
void testGetHostByName()
{
	
	 hostent *host;
	 const char *ip;
	 string s;
	 char tempStr[32];
	 host = gethostbyname("cnlyml.cnblogs.com");
	 if(host == NULL)
		 exit(-1);
	 //inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
	 if(host->h_addrtype == AF_INET)
	 {
		 cout<<host->h_length<<endl;
		 int i = 0;
		 while(host->h_addr_list[i] != NULL)
		 {
			 ip = inet_ntop(AF_INET, host->h_addr_list[i], tempStr, sizeof(tempStr));
			 cout<<ip<<endl;
			 i++;
		 }
	 }
	 exit(0);
}

void testTime_t()
{
	time_t t;
	t = time(NULL);
	cout<<t<<endl;
	exit(0);
}
typedef struct worker worker_thread;
typedef struct pool thread_pool;
struct worker
{
	void *(*process) (void* arg);
	void *arg;
	worker_thread *next;
};
struct pool
{
	pthread_mutex_t queue_lock;
	pthread_cond_t queue_ready;
	worker_thread *queue_head;
	int shutdown;
	pthread_t *threadid;
	int max_thread_num;
	int cur_queue_size;
};

int pool_add_worker(void *(*process)(void *arg),void *arg);
//void* thread_routing(void *arg);
static thread_pool *pos_thread_pool = NULL;

void* thread_routine(void *arg)
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
		
		(*(worker->process))(worker->arg);
		free(worker);
		worker = NULL;
	}
	pthread_exit(NULL);
}
void pool_init(int max_thread_num)
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

int pool_add_worker(void*(*process)(void*arg), void*arg)
{
	worker_thread *worker = (worker_thread*)malloc(sizeof(worker_thread));
	worker->process = process;
	worker->arg = arg;
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


int pool_destroy()
{
	if(pos_thread_pool->shutdown)
		return -1;
	pos_thread_pool->shutdown = 1;
	pthread_cond_broadcast(&(pos_thread_pool->queue_ready));
	for(int i = 0; i < pos_thread_pool->max_thread_num;++i)
		pthread_join(pos_thread_pool->threadid[i], NULL);
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
void *process (void *arg){
	printf ("threadid is 0x%x, working on task %d\n", pthread_self (),*(int *) arg);  
	sleep(1);
	return NULL;
}
void testThreadPool()
{
	pool_init(3);
	int *workingNum = (int*)malloc(sizeof(int)*10);
	for(int i = 0; i < 10; i++)
	{
		workingNum[i] = i;
		pool_add_worker(process,&workingNum[i]);
	}
	sleep(5);
	exit(0);
}
void scan_dir(string basePath, vector<string>files,bool searchSubDir)   // 定义目录扫描函数  
{  
   DIR *dir;
   struct dirent *ptr;
   if((dir = opendir(basePath.c_str())) == NULL)
	   return;
   //获取文件列表
   while((ptr = readdir(dir)) != NULL){
	   if(strcmp(ptr->d_name,".") == 0||strcmp(ptr->d_name,"..") == 0){
		   continue;
	   }
	   string curFilePath;
	   curFilePath = basePath + "/" + ptr->d_name;
	   if(ptr->d_type == 4 && searchSubDir){
		   scan_dir(curFilePath,files,searchSubDir);
	   }
	   cout<<curFilePath<<endl;
   }
   closedir(dir);
}  

void testGetDir()
{
	vector<string>files;
    scan_dir("intermediate",files,true);
    exit(0);
}
int main()
{
   testGetDir();

	//testMap();
   //testfOpen();
   //testStringFind();
   testThreadPool();
   //testTime_t();
   int fd = 0, res = 0, len = 0;
   const unsigned int server_port = 80;
   const char *server_ip = LOCAL;
   char getBuf[HTTP_BUFFER_SIZE] = "";
   struct sockaddr_in server;
   
   server.sin_family = AF_INET;
   server.sin_port = htons(server_port);
   inet_pton(AF_INET,server_ip,&server.sin_addr.s_addr);
   
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
   char sendBuf[] = "GET /doc/1.jpg HTTP/1.0\r\nUser-Agent: test\r\nAccept:*/*\r\nHost: 192.168.27.122\r\nConnection: Keep-Alive\r\n\r\n";
   char  sendBuf2[] = "GET /show/1/9/7491202kee3a48b1.html HTTP/1.0\r\nUser-Agent: test\r\nAccept:*/*\r\nHost: www.nipic.com\r\nConnection: Keep-Alive\r\n\r\n";
   send(fd, sendBuf, strlen(sendBuf),0);
   
   int recvLen = HTTP_BUFFER_SIZE;
   bzero(getBuf, HTTP_BUFFER_SIZE);
   len=read(fd, getBuf,recvLen);
   cout<<"read len "<<len<<endl;
   string s(getBuf);
   cout<<s<<endl;
   exit(0);
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
   char path[]="one.jpg";
   FILE *fp = fopen(path, "w"); 
   //如果有404错误就把文件删除，并关闭socket和文件
   if(s.find("404")!=-1)
   {
	    fclose(fp);
		close(fd);
	    if(remove(path) != 0)
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
