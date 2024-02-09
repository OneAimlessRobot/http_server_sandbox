#include <stdio.h>
#include <sys/types.h>
#include "../Includes/buffSizes.h"
#include "../Includes/filePaths.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"



char *args[]={"<numero de clientes>","<permites logging? (1 ou 0)>",NULL};
//char *args[]={"<path de resource>,<path de destino (relativa ao wd do exec)>",NULL};



char* logfpath="logs.log";
char* defaultTarget="/pages/index.html";
char* notFoundTarget="/pages/notfound.html";
char* testScriptTarget="/pages/testscript.html";
char* defaultVersion="HTTP/1.1";
char* defaultMimetype="*/*";

