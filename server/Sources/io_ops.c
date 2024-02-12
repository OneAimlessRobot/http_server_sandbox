#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
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
#define SEND_FUNC_TO_USE write
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
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
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

int timedreadall(int sd,char* buff,int64_t size){
int counter=0;
int iResult=0;
	while(counter<=MAXTRIES){
		counter++;
		int iResult;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                struct timeval tv;
                tv.tv_sec=BIGTIMEOUTSECS;
                tv.tv_usec=BIGTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
 	        if(iResult>0){

                return readall(sd,buff,size);
                }
		else if(iResult<0){

		break;
		}
		if(logging){
		fprintf(logstream,"A tentar read! Tentativa %d de %d falhou!\n",counter,MAXTRIES);
		}
	}
		
		if(!iResult){
		if(logging){
		fprintf(logstream,"socket deu timeout!!\navisando server para dropar cliente!\n");
		}
		return -2;
		}
		else{
		if(logging){
		fprintf(logstream,"socket deu erro!!\navisando server para dropar cliente!:\n%s\n",strerror(errno));
		}
		}
		return -2;
}
int readall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
	char* ptr= buff;
	char bufftmp[READBUFFSIZE+1]={0};
while(total<size){
        len=readsome(sd,bufftmp,READBUFFSIZE);
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
	printf("Li %ld bytes!!!!\n",len);
	ptr+=snprintf(ptr,READBUFFSIZE,"%s",bufftmp);
	memset(bufftmp,0,READBUFFSIZE+1);
        
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
		return -2;

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

int sendallchunked(int sd,int clientIndex,FILE* stream){

char buff[BUFFSIZE];
char chunkbuff[2 * BUFFSIZE + 10];  // Additional space for size header and CRLF
int numread;

while ((numread = fread(buff, 1, BUFFSIZE, stream)) > 0) {
    int truesize = snprintf(chunkbuff, sizeof(chunkbuff), "%x\r\n", numread);
    memcpy(chunkbuff + truesize, buff, numread);
    memcpy(chunkbuff + truesize + numread, "\r\n", 2);

    int totalsent = 0;
    while (totalsent < truesize + numread + 2) {
        int sent = SEND_FUNC_TO_USE(sd, chunkbuff + totalsent, truesize + numread + 2 - totalsent);
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
		//continue
	
	}
        }
	else{
	
	if(logging){
	fprintf(logstream,"send de %d bytes feito!!!!!\n",sent);
	}
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

// Send final zero-sized chunk
send(sd, "0\r\n\r\n", 5, 0);
return 0;
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
        sent = SEND_FUNC_TO_USE(sd, chunkbuff + totalsent, truesize + numread + 2 - totalsent);
        if(!sent||sent==-2){
		fprintf(logstream,"Timeout no reading!!!!:errno: %d %s\nsocket %d\n",errno,strerror(errno),sd);
                break;
	}
	else if(sent<0){
	
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
	else if(errno){
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
	else if(sent==-2){
		if(logging){
		fprintf(logstream,"Exiting due to timeout!!!!: %s\nsocket %d\n",strerror(errno),sd);
                }
		break;

	}
}

// Send final zero-sized chunk
send(sd, "0\r\n\r\n", 5, 0);
return 0;
}


int sendnormal(int sd,int clientIndex,FILE* stream){

char buff[BUFFSIZE];
int numread;

while ((numread = fread(buff, 1, BUFFSIZE, stream)) > 0) {

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
