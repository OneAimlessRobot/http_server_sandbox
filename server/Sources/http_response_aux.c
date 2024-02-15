#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../Includes/buffSizes.h"
#include "../Includes/http_response_aux.h"
const char* chunkedHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: %s\r\n"
			  "Transfer-Encoding: chunked\r\n"
                          "\r\n";
const char* normalHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: %s\r\n"
			  "Content-Length: %d\r\n"
 			  "\r\n";

const char* redirectHeader= "HTTP/1.1 301 See Other\r\n"
				"Location: %s\r\n"
				"Content-Length: 0\r\n"
				"\r\n";

void fillUpRedirectHeader(char headerBuff[PATHSIZE],char* path){

	snprintf(headerBuff,PATHSIZE,redirectHeader,path);

}
void fillUpChunkedHeader(char headerBuff[PATHSIZE],char* headerTemplate,u_int64_t size,char* mimetype){

	snprintf(headerBuff,PATHSIZE,headerTemplate,mimetype);

}
void fillUpNormalHeader(char headerBuff[PATHSIZE],char* headerTemplate,u_int64_t size,char* mimetype){

	snprintf(headerBuff,PATHSIZE,headerTemplate,mimetype,size);

}
