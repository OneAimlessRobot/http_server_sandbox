#ifndef SERVER_INNARDS_H
#define SERVER_INNARDS_H

void initializeServer(int max_quota);

int testOpenResource(int fd,int clientIndex,char* resourceTarget,char* mimetype);

int testOpenResourcefd(int fd,int clientIndex,char* resourceTarget,char* mimetype);


#endif

