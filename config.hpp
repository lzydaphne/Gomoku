#include <ctime>

#define SIZE 15
#define timeout 5
// #define DEPTH 3
#define MINF -2147483647
#define INF 2147483647
#define RANDOM_SEED time(NULL) //time()这个函数其实保存的是一个历史时间，所以需要用NULL把这个历史时间清空一下，time()就会自动保存当前时间了。你可以简单的理解为NULL就是给time()初始化。