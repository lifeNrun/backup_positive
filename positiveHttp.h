#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <map>
#include <errno.h>
#include <assert.h>
#include <sys/sendfile.h>
#include "positive.h"
using namespace std;
//保存从http request解析下来的值
typedef struct positive_http_header_t
{
	string 		method;
	string 		url;
	string		version;
	string      other_info;
}positive_http_header_t;
enum httpResponse
{
	RES_OK = 0,
	RES_BAD_METHOD,
	RES_BAD_URL
};
typedef struct worker worker_thread;
typedef struct pool thread_pool;
struct worker
{
	void *(*positive_process) (void* arg1, void*arg2);
	void *arg1;
	void *arg2;
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

class positiveHttp
{
	public:
	    positiveHttp(){
			httpMethod = ("GET HEAD POST");
			bzero(http_response,_MAX_SOCKFD_COUNT);
			supportFiles["jpg"] = "Content-Type: image/jpg\r\n\r\n";
			supportFiles["jpeg"] = "Content-Type: image/jpeg\r\n\r\n";
			supportFiles["gif"] = "Content-Type: image/gif\r\n\r\n";
			supportFiles["txt"] = "Content-Type: text/plain\r\n\r\n";
			supportFiles["html"] = "Content-Type: text/html; charset=utf-8\r\n\r\n";
			serverName = "Server: Positive 1.0\r\n";
		};
		int sendHttpHead(int client_socket,int length);
		void sendError(int client_socket);
		bool checkRequest(int client_socket,positive_http_header_t parseInfo);
		int getDataFromUrl(char buffer[],int client_socket);
		void printRequest(positive_http_header_t parseInfo);
		int parseHttpRequest(const string &request, positive_http_header_t* parseInfo);
		void recvHttpRequest(int client_socket, int EpollFd);
		void sendHttpResponse(int client_socket, int EpollFd);
		~positiveHttp(){};
		//线程池操作
		void pool_init(int max_thread_num);
		static void* thread_routine(void *arg);
		static void* positive_process (void *arg1, void*arg2);
		int pool_add_worker(void*(*process)(void*arg1,void*arg2), void*arg1,void *arg2);
	private:
		char buffer[HTTP_HEAD_SIZE];//用于接受或者发送数据的缓存数组
		string serverName;
		static thread_pool *pos_thread_pool;
		static const char badRequest[];
		static const char notFound[];
		static const char notFoundHead[];
		string configPath;//配置文件路径
		string docPath;//服务器文件路径
		string domainName;//域名
		map<string,string> supportFiles;//支持的文件类型
		string httpMethod;
		int http_response[_MAX_SOCKFD_COUNT];
		string requestUrl[_MAX_SOCKFD_COUNT];
};
