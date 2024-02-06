#ifndef HTTP_RESPONSE_AUX_H
#define HTTP_RESPONSE_AUX_H

#define HTTP_OK_HEADER "HTTP/1.1 200 OK\r\n\n"
extern const char* generalHeader;

void fillUpGeneralHeader(char headerBuff[PATHSIZE],u_int64_t size,char* mimetype);
/*
extern const char* videoHeader;
extern const char* audioHeader;
extern const char *manifestHeader;
extern const char *jsHeader;
extern const char *imageHeader;
extern const char *htmlHeader;
extern const char *iconHeader;

void fillUpVideoHeader(char headerBuff[PATHSIZE],u_int64_t size,char* ext);

void fillUpManifestHeader(char headerBuff[PATHSIZE],u_int64_t size,char* ext);

void fillUpJsHeader(char headerBuff[PATHSIZE],u_int64_t size,char* ext);

void fillUpImageHeader(char headerBuff[PATHSIZE],u_int64_t size,char* ext);


void fillUpAudioHeader(char headerBuff[PATHSIZE],u_int64_t size,char* ext);


void fillUpPageHeader(char headerBuff[PATHSIZE],u_int64_t size,char* ext);

void fillUpIconHeader(char headerBuff[PATHSIZE],u_int64_t size,char* ext);

*/
#endif
