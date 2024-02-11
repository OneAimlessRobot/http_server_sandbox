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
void fillUpChunkedHeader(char headerBuff[PATHSIZE],char* headerTemplate,u_int64_t size,char* mimetype){

	snprintf(headerBuff,PAGE_DATA_SIZE,headerTemplate,mimetype);

}
void fillUpNormalHeader(char headerBuff[PATHSIZE],char* headerTemplate,u_int64_t size,char* mimetype){

	snprintf(headerBuff,PAGE_DATA_SIZE,headerTemplate,mimetype,size);

}
