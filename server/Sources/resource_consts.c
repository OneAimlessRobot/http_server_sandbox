#include <stdio.h>
#include <sys/types.h>
#include "../Includes/buffSizes.h"
#include "../Includes/filePaths.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"



char *args[]={"<numero de clientes>",NULL};
//char *args[]={"<path de resource>,<path de destino (relativa ao wd do exec)>",NULL};



char* logfpath="logs.log";
char* defaultTarget="/home.html";
char* notFoundTarget="/notfound.html";
char* defaultVersion="HTTP/1.1";
char* defaultMimetype="*/*";

