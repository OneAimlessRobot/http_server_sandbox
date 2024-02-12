#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include "../Includes/buffSizes.h"
#include "../Includes/server_vars.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#define SEND_FUNC_TO_USE sendsome
//#define SEND_FUNC_TO_USE write
int readsome(int sd,char buff[],u_int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=SMALLTIMEOUTSECS;
                tv.tv_usec=SMALLTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return recv(sd,buff,size,0);

                }
		else if(!iResult){
               	return -2;
		}
		else{
		if(logging){

		fprintf(logstream, "SELECT ERROR!!!!!\n");
		}
		return -1;
		}
}

int sendsome(int sd,char buff[],u_int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=SMALLTIMEOUTSECS;
                tv.tv_usec=SMALLTIMEOUTUSECS;
                iResult=select(sd+1,(fd_set*)0,&rfds,(fd_set*)0,&tv);
                if(iResult>0){

                return send(sd,buff,size,0);
                }
		else if(!iResult){
               	return -2;
		}
		else{
		if(logging){

		fprintf(logstream, "SELECT ERROR!!!!!\n");
		}
		return -1;
		}
}
int readsome_chat_gpt(int sd, char *buff, size_t size) {
    int iResult;
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sd, &rfds);
    tv.tv_sec = SMALLTIMEOUTSECS;
    tv.tv_usec = SMALLTIMEOUTUSECS;
	printf("%d\n",sd+1);
    iResult = select(sd + 1, &rfds,(fd_set*)0, (fd_set*)0, &tv);
    if (iResult > 0) {
        return recv(sd, buff, size, 0);
    } else if (!iResult) {
        return -2; // Timeout
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -1; // Socket would block
        } else {
            return -1; // Other error
        }
    }
}
int sendsome_chat_gpt(int sd, char *buff, size_t size) {
    int iResult;
    struct timeval tv;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sd, &rfds);
    tv.tv_sec = SMALLTIMEOUTSECS;
    tv.tv_usec = SMALLTIMEOUTUSECS;
    iResult = select(sd + 1, NULL, &rfds, NULL, &tv);
    if (iResult > 0) {
        return send(sd, buff, size, 0);
    } else if (!iResult) {
        return -2; // Timeout
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -1; // Socket would block
        } else {
            return -1; // Other error
        }
    }
}


int readall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
while(total<size){
        len=readsome(sd,buff+total,total-size);
	if(!len||len==-2){
		//fprintf(logstream,"Timeout no reading!!!!: %s\nsocket %d\n",strerror(errno),sd);
                break;
	}
	else if(len<0){
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if(logging){
		//fprintf(logstream,"Block no sending!!!!: %s\nsocket %d\n",strerror(errno),sd);
                }
		break;
		//continue;
	
        }
	else if(errno){
		if(logging){
                fprintf(logstream,"Outro erro qualquer!!!!!:%d %s\n",errno,strerror(errno));
                }
		break;
		//continue
	
	}
	}
	else{
	//printf("Li %ld bytes!!!!\n",len); 
	total+=len;
	}
	if(total==size){
	break;
	}
}
	
	if(len<0){
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
        	if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall bem sucedido!! A socket e %d\n",total,sd);
		}
	}
	else if(errno==ENOTCONN){
		if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall saiu com erro!!!!!:\nAvisando server para desconectar!\n%s\n",total,strerror(errno));
		}

	}
	else if(len==-2){
		if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall saiu depois de timeout!!!!!:\n",total);
		}
	}
	else {
		if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall saiu com erro!!!!!:\n%s\n",total,strerror(errno));
		}
	}
	
	}
        
        return total;

}

int send_in_chunks_chunked_chat_gpt(int sd, int clientIndex, int fd) {
    char chunk[BUFFSIZE];
    size_t numread;
    int ret;

    while ((numread = read(fd,chunk,BUFFSIZE)) > 0) {
        // Send chunk size in hex
        char chunk_size_str[20];
        sprintf(chunk_size_str, "%zx\r\n", numread);
        ret = sendsome(sd, chunk_size_str, strlen(chunk_size_str));
        if (ret < 0) {
            fprintf(stderr, "Error sending chunk size: %s\n", strerror(errno));
            return -1;
        }

        // Send chunk data
        ret = sendsome(sd, chunk, numread);
        if (ret < 0) {
            fprintf(stderr, "Error sending chunk data: %s\n", strerror(errno));
            return -1;
        }

        // Send chunk delimiter
        ret = sendsome(sd, "\r\n", 2);
        if (ret < 0) {
            fprintf(stderr, "Error sending chunk delimiter: %s\n", strerror(errno));
            return -1;
        }
    }

    // Send final chunk
    ret = sendsome(sd, "0\r\n\r\n", 5);
    if (ret < 0) {
        fprintf(stderr, "Error sending final chunk: %s\n", strerror(errno));
        return -1;
    }

    return 0; // All data sent successfully
}

int sendallchunkedfd(int sd,int clientIndex,int fd){

char buff[BUFFSIZE];
char chunkbuff[2 * BUFFSIZE + 10];  // Additional space for size header and CRLF
int numread;
int sent=0;
while ((numread = read(fd,buff, BUFFSIZE)) > 0) {
    int truesize = snprintf(chunkbuff, sizeof(chunkbuff), "%x\r\n", numread);
    memcpy(chunkbuff + truesize, buff, numread);
    memcpy(chunkbuff + truesize + numread, "\r\n", 2);

    int totalsent = 0;
    while (totalsent < truesize + numread + 2) {
        errno=0;
	sent = SEND_FUNC_TO_USE(sd, chunkbuff + totalsent, truesize + numread + 2 - totalsent);
	
	if(sent<0){
	
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if(logging){
		fprintf(logstream,"Block no sending!!!!: %s\nsocket %d\n",strerror(errno),sd);
                }
		break;
		//continue;
	
        }

        else if(errno == ECONNRESET){
		if(logging){
                fprintf(logstream,"Conexão largada!!\nSIGPIPE!!!!!: %s\n",strerror(errno));
                }
		peerToDrop=clientIndex;
		socketToClose=sd;
		//raise(SIGPIPE);
		return 0;
		//continue
        }
	else {
		if(logging){
                fprintf(logstream,"Outro erro qualquer!!!!!: %d %s\n",errno,strerror(errno));
                }
		break;
		//continue;
	
	}
        }
	else{
	if(logging){
	fprintf(logstream,"send de %d bytes feito!!!!!\n",sent);
	}
	totalsent += sent;
    	}
	}
}

// Send final zero-sized chunk
send(sd, "0\r\n\r\n", 5, 0);
return 0;
}

int sendnormalfd(int sd,int clientIndex,int fd){

char buff[BUFFSIZE];
int numread;

while ((numread = read(fd,buff, BUFFSIZE)) > 0) {
	int totalsent = 0;
    while (totalsent < numread) {
        int sent = SEND_FUNC_TO_USE(sd, buff+totalsent, numread - totalsent);
        if(sent<0){
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if(logging){
		fprintf(logstream,"Block no sending!!!!: %s\nsocket %d\n",strerror(errno),sd);
                }
		break;
		//continue;
	
        }

        else if(errno == ECONNRESET){
		if(logging){
                fprintf(logstream,"Conexão largada!!\nSIGPIPE!!!!!: %s\n",strerror(errno));
                }
		peerToDrop=clientIndex;
		socketToClose=sd;
		//raise(SIGINT);
		return 0;
		//continue
        }
	else{
		if(logging){
                fprintf(logstream,"Outro erro qualquer!!!!!: %s\n",strerror(errno));
                }
		break;
		//continue;
	
	}
        }
	else{
	totalsent += sent;
    	}
	}
        if(errno){
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if(logging){
		fprintf(logstream,"Exiting due to block!!!!: %s\nsocket %d\n",strerror(errno),sd);
                }
		//break;
		continue;
        }
	else{
		if(logging){
		fprintf(logstream,"Exiting due to some other error!!!!: %s\nsocket %d\n",strerror(errno),sd);
                }
		break;
		
	}
	}
}
send(sd, "\r\n\r\n", 5, 0);
return 0;
}
