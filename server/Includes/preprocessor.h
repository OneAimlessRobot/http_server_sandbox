#define MAXTIMEOUTSECS 300
#define MAXTIMEOUTUSECS 0
#define PAGE_DATA_SIZE 300000
#define PAGE_RESPONSE_SIZE 10500000
#define ARGVMAX 100
#define LINESIZE 1024
#define PORT 8001
#define MAX_CLIENTS_HARD_LIMIT 1000
#include <ncurses.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "auxFuncs.h"
#define INDEX_PATH "/home/oar_X_I/http_server_sandbox/server/resources/index.html"
#define RESOURCES_PATH "/home/oar_X_I/http_server_sandbox/server/resources"
#define INDEX2_PATH "/home/oar_X_I/http_server_sandbox/server/resources/index2.html"

