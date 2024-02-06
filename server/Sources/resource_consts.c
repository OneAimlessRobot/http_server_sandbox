#include <stdio.h>
#include <sys/types.h>
#include "../Includes/buffSizes.h"
#include "../Includes/filePaths.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"



char *args[]={"<numero de clientes>",
		NULL};


page mainpage={NULL,INDEX_PATH,NULL,0,0,&fillUpGeneralHeader},
	notfoundpage={NULL,NOTFOUND_PATH,NULL,0,0,&fillUpGeneralHeader};


char* logfpath="logs.log";
char* defaultTarget="/index.html";
char* defaultVersion="HTTP/1.1";
char* defaultMimetype="*/*";

