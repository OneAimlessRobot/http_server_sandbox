#ifndef HANDLECUSTOM_H
#define HANDLECUSTOM_H
//custom requests
#define WRITE_VENT_REQ "/writevent"

extern char* customgetreqs[];

extern char* custompostreqs[];

int isCustomGetReq(char* nulltermedtarget);

int isCustomPostReq(char* nulltermedtarget);

void handleCustomGetReq(char* customRequest,char targetinout[PATHSIZE]);

void handleCustomPostReq(char* nulltermedtarget,char* contents,char targetinout[PATHSIZE]);

#endif
