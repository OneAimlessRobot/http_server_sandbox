#ifndef SEND_RESOURCE_FUNC_H
#define SEND_RESOURCE_FUNC_H
#define USEFD 1
int sendResource(int sd,int clientIndex,char* resourceTarget,char* mimetype,int usefd);
#endif
