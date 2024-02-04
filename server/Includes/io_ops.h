#ifndef IO_OPS_H
#define IO_OPS_H

int readall(int sd,char* buff,int64_t size);

int sendall(int sd,char* buff,int64_t size);

#endif
