#ifndef IO_OPS_H
#define IO_OPS_H

int readall(int sd,char* buff,int64_t size);

int sendallchunked(int sd,int clientIndex,FILE* stream);

int sendallchunkedfd(int sd,int clientIndex,int fd);

int timedreadall(int sd,char* buff,int64_t size);

#endif
