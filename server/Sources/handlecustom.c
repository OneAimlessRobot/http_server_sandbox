
#include "../Includes/preprocessor.h"
#include <errno.h>
#include "../Includes/buffSizes.h"
#include "../Includes/server_vars.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#include "../Includes/handlecustom.h"

typedef struct pipestruct{

	int read_end,write_end;

}pipestruct;

char* tmpOne="/tmp.html",* tmpTwo="/tmp1.html";
static int currvent=0;
char* customgetreqs[]={WRITE_VENT_REQ,SEE_FILES_REQ,NULL};

char* custompostreqs[]={WRITE_VENT_REQ,NULL};

char* dirListing[ARGVMAX];
char* tmpDir=NULL;
char* tmpDir2=NULL;

static pipestruct* redirectFd1ToFd1(int fd1,int fd2){
	dup2(fd1,fd2);

	return NULL;
}

static void generateDirListingPrimitive(char* path){
	tmpDir=malloc(PATHSIZE);
	memset(tmpDir,0,PATHSIZE);
	tmpDir2=malloc(PATHSIZE);
	memset(tmpDir2,0,PATHSIZE);
	snprintf(tmpDir,PATHSIZE,"%s/resources%s",currDir,tmpOne);
	snprintf(tmpDir2,PATHSIZE,"%s/resources%s",currDir,tmpTwo);
	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);
	char* cmd= malloc(PATHSIZE);
	memset(cmd,0,PATHSIZE);
	snprintf(cmd,PATHSIZE,"ls -1 %s/resources%s > %s",currDir,path,tmpDir);
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
	char* currListing=malloc(BUFFSIZE);
	memset(currListing,0,BUFFSIZE);
	dprintf(fd,"<!DOCTYPE html>\n<html>\n<head>\n<base href=''>\n</head>\n<body>\n<h1>DIRETORIA ROOT DO SERVER!!!!!</h1>\n<br>\n");
	
	while(fgets(currListing,BUFFSIZE,fstream)){
		currListing[strlen(currListing)-1]=0;
		dprintf(fd,"<a href='%s/%s'>%s</a><br>\n",dir,currListing,currListing);
	
	}
	
	dprintf(fd,"</body>\n</html>\n");
	remove(tmpDir);
	free(currListing);
	fclose(fstream);
	close(fd);
	free(tmpDir);
	free(tmpDir2);
	return tmpTwo;

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



void* handleCustomGetReq(char* customRequest,char targetinout[PATHSIZE]){
	void* result=NULL;
	char* targetcopy= strdup(customRequest);
	char* argv2[2];
	memset(argv2,0,2*sizeof(char*));
	splitString(targetcopy,"?",argv2);
	if(!strcmp(argv2[0],WRITE_VENT_REQ)){
		handleVentReqFd(argv2[1],targetinout);
	}
	/*else if(!strcmp(argv2[0],SEE_FILES_REQ)){
		generateDirListing("/resources");
		result=tmpTwo;
	}*/
	/*
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}*/
	free(targetcopy);
	return result;
	
}
void* handleCustomPostReq(char* target,char* contents,char targetinout[PATHSIZE]){
	void* result=NULL;
	if(!strcmp(target,WRITE_VENT_REQ)){
		handleVentReq(contents,targetinout);
		
	}
	/*else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}*/
	return result;
}
