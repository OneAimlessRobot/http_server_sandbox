#ifndef RESOURCE_CONSTS_H
#define RESOURCE_CONSTS_H
typedef void (*header_fill_func_type)(char headerBuff[PATHSIZE], u_int64_t size,char* ext);

typedef struct page{

	int pagefd;
	FILE* pagestream;
	char  pagepath[PATHSIZE];
	int header_size,data_size;
	header_fill_func_type headerFillFunc;
}page;

extern page mainpage,notfoundpage;

extern char* logfpath,* defaultTarget,* defaultVersion,* defaultMimetype,*notFoundTarget,*testScriptTarget;

extern FILE* logstream;

#endif
