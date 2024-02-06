#include <stdio.h>
#include <sys/types.h>
#include "../Includes/buffSizes.h"
#include "../Includes/filePaths.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"

char* pagesArr[]={"/index2.html",
		 "/index.html",
		"/media.html",
		NULL};
char* audioExtArr[]={"mp3",
			NULL};

char* videoExtArr[]={"mp4",
		NULL};

char* pageExtArr[]={"html",
		NULL};

char* iconExtArr[]={
		"ico",
		NULL};


char* imageExtArr[]={"jpeg",
		"png",
		"jpg",
		"gif",
		"webp",
		NULL};

char* manifestExtArr[]={"webmanifest",

	NULL};

char* jsExtArr[]={"js",

	NULL};


char *args[]={"<numero de clientes>",
		NULL};


page mainpage={NULL,INDEX_PATH,NULL,0,0,&fillUpGeneralHeader},
	notfoundpage={NULL,NOTFOUND_PATH,NULL,0,0,&fillUpGeneralHeader};


const char* logfpath="logs.log";
