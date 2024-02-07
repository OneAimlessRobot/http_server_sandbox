#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../Includes/buffSizes.h"
#include "../Includes/http_response_aux.h"
const char* generalHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: %s\r\n"
			  "Transfer-Encoding: chunked\r\n"
                          "\r\n";
void fillUpGeneralHeader(char headerBuff[PATHSIZE],u_int64_t size,char* mimetype){

	snprintf(headerBuff,PAGE_DATA_SIZE,generalHeader,mimetype);

}
