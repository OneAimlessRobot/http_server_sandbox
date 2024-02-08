
#include "../Includes/preprocessor.h"
#include <errno.h>
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/handlecustom.h"

static int currvent=0;
char* customgetreqs[]={WRITE_VENT_REQ,NULL};

char* custompostreqs[]={WRITE_VENT_REQ,NULL};

static void handleVentReq(char* fieldmess,char targetinout[PATHSIZE]){
	char* pathbuff=malloc(PATHSIZE);
	memset(pathbuff,0,PATHSIZE);
	char* argv2[ARGVMAX]={0};
	printf("cheguei!!!!\n");
	snprintf(pathbuff,PATHSIZE,"%s/ventNumber%d",VENTS_PATH,currvent);
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
	printf("cheguei!!!!\n");	
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
	
	char* targetcopy= strdup(customRequest);
	char* argv2[2];
	memset(argv2,0,2*sizeof(char*));
	splitString(targetcopy,"?",argv2);
	if(!strcmp(argv2[0],WRITE_VENT_REQ)){
		printf("cheguei!!!!\n");
		handleVentReq(argv2[1],targetinout);
		currvent++;
	}
	/*else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}*/
	free(targetcopy);
	
}
void handleCustomPostReq(char* target,char* contents,char targetinout[PATHSIZE]){
	
	if(!strcmp(target,WRITE_VENT_REQ)){
		printf("cheguei!!!!\n");
		handleVentReq(contents,targetinout);
		currvent++;
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
