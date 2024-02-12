#ifndef IO_OPS_H
#define IO_OPS_H

int readall(int sd,char* buff,int64_t size);

int sendallchunkedfd(int sd,int clientIndex,int fd);

int sendnormalfd(int sd,int clientIndex,int fd);

int send_in_chunks_chunked_chat_gpt(int sd, int clientIndex, int fd);

#endif
