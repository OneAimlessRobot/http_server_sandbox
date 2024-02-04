#include <stdio.h>
#include <sys/types.h>
#include "../Includes/buffSizes.h"
#include "../Includes/filePaths.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"

char* pagesArr[]={"/index2.html",
		 "/index.html",
		"/media.html",
		""};
char* audioExtArr[]={"mp3",
			""};

char* videoExtArr[]={"mp4",
		""};

char* pageExtArr[]={"html",
		""};

char* iconExtArr[]={
		"ico",
		""};


char* imageExtArr[]={"jpeg",
		"png",
		"jpg",
		"gif",
		"webp",
		""};

char* manifestExtArr[]={"webmanifest",

	""};

char* jsExtArr[]={"js",

	""};


char *args[]={"<numero de clientes>",
		""};


page mainpage={NULL,INDEX_PATH,NULL,0,0,&fillUpPageHeader},
	notfoundpage={NULL,NOTFOUND_PATH,NULL,0,0,&fillUpPageHeader};


const char* logfpath="logs.log";
