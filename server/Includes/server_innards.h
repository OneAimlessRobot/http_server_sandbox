#ifndef SERVER_INNARDS_H
#define SERVER_INNARDS_H

void initializeServer(int max_quota,int logs);

int* getClientArrCopy(void);

int getMaxNumOfClients(void);

int getCurrNumOfClients(void);

#endif

