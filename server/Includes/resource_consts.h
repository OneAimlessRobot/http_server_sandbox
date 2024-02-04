#ifndef RESOURCE_CONSTS_H
#define RESOURCE_CONSTS_H
typedef void (*header_fill_func_type)(char headerBuff[LINESIZE], u_int64_t size,char* ext);

typedef struct page{

	FILE* pagestream;
	char  pagepath[LINESIZE];
	char* data;
	int header_size,data_size;
	header_fill_func_type headerFillFunc;
}page;

extern char* pagesArr[];
extern char* audioExtArr[];
extern char* videoExtArr[];
extern char* pageExtArr[];
extern char* iconExtArr[];
extern char* imageExtArr[];
extern char* manifestExtArr[];
extern char* jsExtArr[];
extern char *args[];

extern page mainpage,notfoundpage;

extern const char* logfpath;

extern FILE* logstream;

#endif
