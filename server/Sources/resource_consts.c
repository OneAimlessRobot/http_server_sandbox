#include <stdio.h>
#include <sys/types.h>
#include "../Includes/buffSizes.h"
#include "../Includes/filePaths.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"



char *args[]={"<numero de clientes>","<permites logging? (1 ou 0)>",NULL};
//char *args[]={"<path de resource>,<path de destino (relativa ao wd do exec)>",NULL};
//char *args[]={"<path a listar (relativa ao wd do exec)>",NULL};



char* logfpath="logs.log";
char* defaultTarget="/resources/index.html";
char* defaultTargetAdmin="/admin/index_admin.html";
char* defaultLoginTarget="/index_login.html";
char* incorrectLoginTarget="/index_login_incorrect.html";
char* notFoundTarget="/resources/pages/notfound.html";
char* testScriptTarget="/resources/pages/testscript.html";
char* defaultVersion="HTTP/1.1";
char* defaultMimetype="*/*";

