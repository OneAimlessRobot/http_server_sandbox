#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include "../Includes/buffSizes.h"
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
int counter=1;
int iResult;
	while(counter<=MAXTRIES){
		fprintf(logstream,"A tentar read! Tentativa %d de %d\n",counter,MAXTRIES);
		int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=BIGTIMEOUTSECS;
                tv.tv_usec=BIGTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
 	        if(iResult>0){

                return readall(sd,buff,size);
                }
		else if(iResult<0){

		break;
		}
		counter++;
	}
		
		if(!iResult){

		fprintf(logstream,"socket deu timeout!!\navisando server para dropar cliente!\n");
		return -2;
		}
		else{

		fprintf(logstream,"socket deu erro!!\navisando server para dropar cliente!:\n&s\n",strerror(errno));
		
		}
		return -2;
}
int timedsendall(int sd,char* buff,int64_t size){

int counter=1;
int iResult;
	while(counter<=MAXTRIES){
		fprintf(logstream,"A tentar send! Tentativa %d de %d\n",counter,MAXTRIES);
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
		counter++;
	}
		
		if(!iResult){

		fprintf(logstream,"socket deu timeout!!\navisando server para dropar cliente!\n");
		return -2;
		}
		else{

		fprintf(logstream,"socket deu erro!!\navisando server para dropar cliente!:\n&s\n",strerror(errno));
		
		}
		return -2;
}
int readall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
	char* ptr= buff;
	char bufftmp[LINESIZE]={0};
while((len=read(sd,bufftmp,LINESIZE))>0){
        printf("Li!!!!\n");
	ptr+=snprintf(ptr,LINESIZE,"%s",bufftmp);
	memset(bufftmp,0,LINESIZE);
        if (len <= 0) {
	break;
	}
	total+=len;
	if(total==size){
	break;
	}
}

	if(len<=0){
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
	else{
		fprintf(logstream,"Li %ld ao todo!!!! readall bem sucedido!!!!!:\nBuffer cheio!\n%s\n",total,strerror(errno));
	
	}
        
        return total;

}
int sendall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
while((len=write(sd,buff+total,min(LINESIZE,size-total)))>0){ 
	if(len<=0){
	break;
	}
	total+=len;
	if(total>=size){
	break;
	}
}
	
	if(len<=0){

	fprintf(logstream,"Enviei %ld ao todo de %ld!!!! sendall saiu com erro!!!!!:\n%s\n",total,size,strerror(errno));
	}
	else{

	fprintf(logstream,"Enviei %ld ao todo de %ld!!!! sendall bem sucedido!!\n",total,size);
	}
        return total;

}
