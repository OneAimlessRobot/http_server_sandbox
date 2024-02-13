
#include "../Includes/preprocessor.h"
#include <errno.h>
#include "../Includes/buffSizes.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#include "../Includes/load_logins.h"
#include "../Includes/handlecustom.h"
static struct sockaddr_in  clientAddress;

static socklen_t socklenpointer;

static char addressContainer[INET_ADDRSTRLEN];

typedef struct pipestruct{

	int read_end,write_end;

}pipestruct;

char* tmpOne="/.tmp.html",* tmpTwo="/.tmp1.html";
char* tmpClients="/.tmpC.html";;
static int currvent=0;
char* customgetreqs[]={WRITE_VENT_REQ,SEE_FILES_REQ,NULL};

char* custompostreqs[]={WRITE_VENT_REQ,SIGN_IN_REQ,SEE_CLIENTS_REQ,NULL};


char* tmpDir=NULL;
char* tmpDir2=NULL;
char* currSearchedDir=NULL;

static void generateDirListingPrimitive(char* path){
	tmpDir=malloc(PATHSIZE);
	memset(tmpDir,0,PATHSIZE);
	tmpDir2=malloc(PATHSIZE);
	memset(tmpDir2,0,PATHSIZE);
	currSearchedDir=malloc(PATHSIZE);
	memset(currSearchedDir,0,PATHSIZE);
	snprintf(tmpDir,PATHSIZE,"%s%s",currDir,tmpOne);
	snprintf(tmpDir2,PATHSIZE,"%s%s",currDir,tmpTwo);
	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);
	char* cmd= malloc(PATHSIZE);
	memset(cmd,0,PATHSIZE);
	snprintf(currSearchedDir,PATHSIZE,"%s%s",currDir,path);
	snprintf(cmd,PATHSIZE,"ls -1 %s > %s",currSearchedDir,tmpDir);
	system(cmd);
	free(cmd);
	close(outfd);
}
char* generateDirListing(char* dir){
	
	generateDirListingPrimitive(dir);
	int fd=	open(tmpDir2,O_TRUNC|O_WRONLY|O_CREAT,0777);
	
	if(!tmpDir||!tmpDir2){

		return NULL;
	}
	if(fd<0){
		if(logging){
		fprintf(logstream,"ERRO NAS DIRETORIAS %s\n",tmpDir2);
		}
		return NULL;
	}
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
		if(logging){
		fprintf(logstream,"ERRO NAS DIRETORIAS %s\n",tmpDir);
		}
		return NULL;
	}
	remove(tmpDir);
	int pathsize=strlen(dir);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	int noRoot=strcmp(dir,"/resources");
	char* currListing=malloc(BUFFSIZE);
	memset(currListing,0,BUFFSIZE);
	dprintf(fd,"<!DOCTYPE html>\n<html>\n<head>\n<base href=''>\n</head>\n<body>");
	if(!noRoot){

		dprintf(fd,"\n<h1>DIRETORIA ROOT DO SERVER!!!!!</h1>");
	}
	else{
		dprintf(fd,"\n<br>\n<h2>INDEX OF: %s</h2><br>\n<br>\n",dir);
		dprintf(fd,"\n<br>\n<a href='%s/..'>Prev</a><br><br><br>\n",dir);
	
	}
	while(fgets(currListing,BUFFSIZE,fstream)){
		currListing[strlen(currListing)-1]=0;
		dprintf(fd,"<a href='%s/%s'>%s</a><br>\n",dir,currListing,currListing);
	
	}
	
	dprintf(fd,"</body>\n</html>\n");
	free(currListing);
	fclose(fstream);
	close(fd);
	free(tmpDir);
	free(tmpDir2);
	free(currSearchedDir);
	return tmpTwo;

}

static void generateClientListing(char targetinout[PATHSIZE]){
	char path[PATHSIZE]={0};
	snprintf(path,PATHSIZE,"%s/resources%s",currDir,tmpClients);
	int fd=	open(path,O_TRUNC|O_WRONLY|O_CREAT,0777);
	int* clients=getClientArrCopy();
	int maxQuota=getMaxNumOfClients();
	int currQuota=getCurrNumOfClients();
	
	
	dprintf(fd,"<!DOCTYPE html>\n<html>\n<head>\n<base href=''>\n</head>\n<body>\n");
	dprintf(fd,"<br>\n<a href='%s'>Go back</a>\n<br>\n",defaultTarget);
	dprintf(fd,"<br>\n<form id='submitbutton' method='POST' action='/seeclients'>\n<input type='submit' name='button' value='REFRESH'>\n</form>\n<br>\n");
	dprintf(fd,"\n<br>\n<h1>Ocupacao: %d/%d</h1>\n<br>\n<br>\n<br>" ,currQuota,maxQuota);
  	
	for(int i=0;i<maxQuota;i++){
	if(clients[i]){
	getpeername(clients[i] , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
        dprintf(fd,"\n<p>Cliente numero: %d , ip %s , port %d</p>\n<p>\n<br>\n" ,i,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
	}
	else{
	dprintf(fd,"\n<p>Posicao %d desocupada</p>\n<br>\n" ,i);
	}
	}
	dprintf(fd,"<br>\n<");
	dprintf(fd,"</body>\n</html>\n");
	close(fd);
	free(clients);
	memcpy(targetinout,tmpClients,strlen(tmpClients));

}

static void handleVentReq(char* fieldmess,char targetinout[PATHSIZE]){
	char* pathbuff=malloc(PATHSIZE);
	memset(pathbuff,0,PATHSIZE);
	char* argv2[ARGVMAX]={0};
	snprintf(pathbuff,PATHSIZE,"%s/resources/vents/ventNumber%d",currDir,currvent);
	FILE* stream=NULL;
	if(!(stream=fopen(pathbuff,"wb+"))){

		fprintf(logstream,"ERRO A ABRIR VENT!!!!!\n%s\n",strerror(errno));
		return;
	}
	int size=makeargvdelim(fieldmess,"&",argv2,ARGVMAX);
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"venttitle")||!strcmp(pair[0],"ventcontent")){
		replaceStringCharacter(pair[1],'+',' ');
		fwrite(pair[0], 1, strlen(pair[0]),stream);
		fwrite(": ",1,2,stream);
		fwrite(pair[1],1,strlen(pair[1]),stream);
		fwrite("\n",1,1,stream);
		}
		
	}
	free(pathbuff);
	memcpy(targetinout,testScriptTarget,strlen(testScriptTarget));
	fclose(stream);
	currvent++;
}

static void handleVentReqFd(char* fieldmess,char targetinout[PATHSIZE]){
	char* pathbuff=malloc(PATHSIZE);
	memset(pathbuff,0,PATHSIZE);
	char* argv2[ARGVMAX]={0};
	snprintf(pathbuff,PATHSIZE,"%s/resources/vents/ventNumber%d",currDir,currvent);
	int stream;
	if((stream=open(pathbuff,O_CREAT|O_TRUNC|O_WRONLY,0777))<0){

		fprintf(logstream,"ERRO A ABRIR VENT!!!!!\npath: %s\nErro: %d %s\n",pathbuff,errno,strerror(errno));
		return;
	}
	int size=makeargvdelim(fieldmess,"&",argv2,ARGVMAX);
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"venttitle")||!strcmp(pair[0],"ventcontent")){
		replaceStringCharacter(pair[1],'+',' ');
		write(stream,pair[0],strlen(pair[0]));
		write(stream,": ",2);
		write(stream,pair[1],strlen(pair[1]));
		write(stream,"\n",1);
		}
		
	}
	free(pathbuff);
	memcpy(targetinout,testScriptTarget,strlen(testScriptTarget));
	close(stream);
	currvent++;
	}

static void handleLogin(char* fieldmess,char targetinout[PATHSIZE]){
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(fieldmess,"&",argv2,ARGVMAX);
	
		char* username[2]={0};
		splitString(argv2[0],"=",username);
		char* password[2]={0};
		char* correctPassword;
		splitString(argv2[1],"=",password);
		printf("Given Username: %s Password: %s\n...",username[1],password[1]);
			if(strcmp((correctPassword=find_login_pw_in_login_arr(username[1],currLogins)),"NO_SUCH_LOGIN")){
				printf("Found username!!!!\nThe password given is %s and the correct one is %s\n",password[1],correctPassword);
			
				if(!memcmp(correctPassword,password[1],strlen(password[1]))){
					printf("Correct password!!!!\n");
					memcpy(targetinout,defaultTarget,strlen(defaultTarget));
					return;
				}
				
			}
		printf("Given Username: %s Password: %s\n...",username[1],password[1]);
			if(strcmp((correctPassword=find_login_pw_in_login_arr(username[1],currAdmins)),"NO_SUCH_LOGIN")){
				printf("Found username!!!!\nThe password given is %s and the correct one is %s\n",password[1],correctPassword);
			
				if(!memcmp(correctPassword,password[1],strlen(password[1]))){
					printf("Correct password!!!!\n");
					memcpy(targetinout,defaultTargetAdmin,strlen(defaultTargetAdmin));
					return;
				}
				
			}
	
	memcpy(targetinout,incorrectLoginTarget,strlen(incorrectLoginTarget));
}

int isCustomGetReq(char* nulltermedtarget){
	int result=0;
	char* targetcopy= strdup(nulltermedtarget);
	char* argv2[2];
	memset(argv2,0,2*sizeof(char*));
	splitString(targetcopy,"?",argv2);
	result=(findInStringArr(customgetreqs,argv2[0])>=0);
	free(targetcopy);
	return result;
}

int isCustomPostReq(char* nulltermedtarget){
	int result=0;
	result=(findInStringArr(custompostreqs,nulltermedtarget)>=0);
	return result;
}



void handleCustomGetReq(char* customRequest,char targetinout[PATHSIZE]){
	void* result=NULL;
	char* targetcopy= strdup(customRequest);
	char* argv2[2];
	memset(argv2,0,2*sizeof(char*));
	splitString(targetcopy,"?",argv2);
	if(!strcmp(argv2[0],WRITE_VENT_REQ)){
		handleVentReqFd(argv2[1],targetinout);
	}
	else if(!strcmp(argv2[0],SEE_CLIENTS_REQ)){
		generateClientListing(targetinout);

	}
	/*
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}*/
	free(targetcopy);
	
}
void handleCustomPostReq(char* target,char* contents,char targetinout[PATHSIZE]){
	void* result=NULL;
	if(!strcmp(target,WRITE_VENT_REQ)){
		handleVentReq(contents,targetinout);
		
	}
	else if(!strcmp(target,SIGN_IN_REQ)){
		handleLogin(contents,targetinout);
		
	}
	else if(!strcmp(target,SEE_CLIENTS_REQ)){
		generateClientListing(targetinout);

	}
	/*else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}*/
}

void deleteDirListingHTML(void){
char buff[PATHSIZE]={0};
snprintf(buff,PATHSIZE,"%s/resources%s",currDir,tmpTwo);
remove(buff);

}
void deleteClientListingHTML(void){
char buff[PATHSIZE]={0};
snprintf(buff,PATHSIZE,"%s%s",currDir,tmpClients);
remove(buff);

}
