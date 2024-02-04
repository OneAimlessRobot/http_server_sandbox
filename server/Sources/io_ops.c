#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "../Includes/buffSizes.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/io_ops.h"
int readall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
	char* ptr= buff;
	char bufftmp[LINESIZE]={0};
while((len=read(sd,bufftmp,LINESIZE))>0){
        printf("Li!!!!\n");
	ptr+=snprintf(ptr,LINESIZE,"%s",bufftmp);
	memset(bufftmp,0,LINESIZE);
        total+=len;
	if(total>=size){
	break;
	}
	if(len<LINESIZE){
	break;
	}
}
        return total;

}
int sendall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
while((len=write(sd,buff+total,min(LINESIZE,size-total)))>0){ 
	//printf("Escrevi %ld!!!!\n",len);
	total+=len;
	if(total>=size){
	break;
	}
}
        return total;

}
