#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
using namespace;
typedef struct worker worker_thread;
struct worker
{
	void *(*process) (void* arg);
	void *arg;
	worker_thread *next;
};