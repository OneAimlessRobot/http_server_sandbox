#ifndef HTTP_RESPONSE_AUX_H
#define HTTP_RESPONSE_AUX_H

#define HTTP_OK_HEADER "HTTP/2 200 OK\r\n\n"
extern const char* generalHeader;

void fillUpGeneralHeader(char headerBuff[PATHSIZE],u_int64_t size,char* mimetype);
#endif
