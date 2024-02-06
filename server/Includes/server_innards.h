#ifndef SERVER_INNARDS_H
#define SERVER_INNARDS_H

void initializeServer(int max_quota);

int testOpenResource(int fd,char* resourceTarget,char* mimetype);


#endif

