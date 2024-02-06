#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include "../Includes/buffSizes.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"

static int readsome(int sd,char buff[],u_int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=SMALLTIMEOUTSECS;
                tv.tv_usec=SMALLTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return read(sd,buff,size);

                }
               return -2;
}

static int sendsome(int sd,char buff[],u_int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=SMALLTIMEOUTSECS;
                tv.tv_usec=SMALLTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return write(sd,buff,size);
                }
               return -2;
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
		fprintf(logstream,"A tentar read! Tentativa %d de %d falhou!\n",counter,MAXTRIES);
		
	}
		
		if(!iResult){

		fprintf(logstream,"socket deu timeout!!\navisando server para dropar cliente!\n");
		return -2;
		}
		else{

		fprintf(logstream,"socket deu erro!!\navisando server para dropar cliente!:\n%s\n",strerror(errno));
		
		}
		return -2;
}
int timedsendall(int sd,char* buff,int64_t size){

int counter=0;
int iResult=0;
	while(counter<=MAXTRIES){
		counter++;
		int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=BIGTIMEOUTSECS;
                tv.tv_usec=BIGTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
 	        if(iResult>0){

                return sendall(sd,buff,size);
                }
		else if(iResult<0){

		break;
		}
		fprintf(logstream,"A tentar send! Tentativa %d de %d falhou!\n",counter,MAXTRIES);
	}
		
		if(!iResult){

		fprintf(logstream,"socket deu timeout!!\navisando server para dropar cliente!\n");
		return -2;
		}
		else{

		fprintf(logstream,"socket deu erro!!\navisando server para dropar cliente!:\n%s\n",strerror(errno));
		
		}
		return -2;
}
int readall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
	char* ptr= buff;
	char bufftmp[BUFFSIZE]={0};
while((len=readsome(sd,bufftmp,BUFFSIZE))>0){
        //printf("Li!!!!\n");
	ptr+=snprintf(ptr,BUFFSIZE,"%s",bufftmp);
	memset(bufftmp,0,BUFFSIZE);
        if (len < 0) {
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//fprintf(logstream,"Block no sendall!!!!\n");
	}
	else{
	break;
	}
	}
	total+=len;
	if(!len){
	break;
	}
	if(total==size){
	break;
	}
}
	
	if(len<0){
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
        	//fprintf(logstream,"Li %ld ao todo!!!! readall bem sucedido!! A socket e %d\n",total,sd);
	}
	else if(errno==ENOTCONN){

		fprintf(logstream,"Li %ld ao todo!!!! readall saiu com erro!!!!!:\nAvisando server para desconectar!\n%s\n",total,strerror(errno));
		return -2;

	}
	else {
		fprintf(logstream,"Li %ld ao todo!!!! readall saiu com erro!!!!!:\n%s\n",total,strerror(errno));
	}
	
	}
	else if(len){
		if(total==size){
			fprintf(logstream,"Li %ld ao todo de %ld!!!! readall bem sucedido!!!!!:\nBuffer cheio!\n",total,size);
		}
		else {
			fprintf(logstream,"Li %ld ao todo!!!! readall bem sucedido!!!!!:\n",total);
		}
	}
        
        return total;

}
int sendall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;

while(1){
	//min(BUFFSIZE,size-total)
	//size
	len=send(sd,buff+total,min(BUFFSIZE,size-total),0);
	if(len<0){
	if(len==-2){
		fprintf(logstream,"Timeout no sendall!!!!\n");
		
		continue;
	}
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		fprintf(logstream,"Block no sendall!!!!\n");
		break;
		//continue;
	}
	else{
	break;
	}
	}
	total+=len;
	if(!len){
	break;
	}
	if(total==size){
	break;
	}
}
	
	if(len<0){
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
        	//fprintf(logstream,"Li %ld ao todo!!!! readall bem sucedido!! A socket e %d\n",total,sd);
	}
	else if(errno==ENOTCONN){

		fprintf(logstream,"Enviei %ld ao todo de %ld!!!! sendall saiu com erro!!!!!:\nAvisando server para desconectar!\n%s\n",total,size,strerror(errno));
		return -2;

	}
	else {
		fprintf(logstream,"Enviei %ld ao todo de %ld!!!! sendall saiu com erro!!!!!:\n%s\n",total,size,strerror(errno));
	}
	
	}
	else{
		fprintf(logstream,"Enviei %ld ao todo de %ld!!!!sendall bem sucedido !!!!!:\n",total,size);
	}
        return total;

}
