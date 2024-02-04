#include <stdio.h>
#include <sys/types.h>
#include "../Includes/buffSizes.h"
#include "../Includes/http_response_aux.h"
const char* videoHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: video/%s\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

const char* audioHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: audio/%s\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

const char *manifestHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/manifest+json\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

 const char *imageHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: image/%s\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

const char *htmlHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

const char *iconHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: image/x-icon\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";


void fillUpVideoHeader(char headerBuff[LINESIZE],u_int64_t size,char* ext){

	snprintf(headerBuff,PAGE_DATA_SIZE,videoHeader,ext,size);

}
void fillUpManifestHeader(char headerBuff[LINESIZE],u_int64_t size,char* ext){

	snprintf(headerBuff,PAGE_DATA_SIZE,manifestHeader,size);

}
void fillUpImageHeader(char headerBuff[LINESIZE],u_int64_t size,char* ext){

	snprintf(headerBuff,PAGE_DATA_SIZE,imageHeader,ext,size);

}
void fillUpAudioHeader(char headerBuff[LINESIZE],u_int64_t size,char* ext){

	snprintf(headerBuff,PAGE_DATA_SIZE,audioHeader,ext,size);

}
void fillUpPageHeader(char headerBuff[LINESIZE],u_int64_t size,char* ext){

	snprintf(headerBuff,PAGE_DATA_SIZE,htmlHeader,size);
	//snprintf(headerBuff,PAGE_DATA_SIZE,"1");

}
void fillUpIconHeader(char headerBuff[LINESIZE],u_int64_t size,char* ext){

	snprintf(headerBuff,PAGE_DATA_SIZE,iconHeader,size);

}
