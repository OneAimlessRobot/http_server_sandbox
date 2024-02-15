#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../Includes/buffSizes.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include "../Includes/load_logins.h"
#include "../Includes/resource_consts.h"

static int clientIsRepeated(client*c){
	client* arr=getClientArrCopy();
	int quota= getMaxNumOfClients();
	for(int i=0;i<quota;i++){
		if(!strncmp(arr[i].username,c->username,FIELDSIZE)&&strlen(arr[i].username)){

			free(arr);
			return 1;
		}

	}
	free(arr);
	return 0;
}

void handleLogin(client* c,char* fieldmess,char targetinout[PATHSIZE]){
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(fieldmess,"&",argv2,ARGVMAX);
	
		char* username[2]={0};
		splitString(argv2[0],"=",username);
		char* password[2]={0};
		char* correctPassword;
		splitString(argv2[1],"=",password);
		memset(c->username,0,FIELDSIZE);
			if(strcmp((correctPassword=find_login_pw_in_login_arr(username[1],currLogins)),"NO_SUCH_LOGIN")){
				
				if(!memcmp(correctPassword,password[1],strlen(correctPassword))&&!clientIsRepeated(c)){
					memcpy(targetinout,defaultTarget,strlen(defaultTarget));
					strncpy(c->username,username[1],FIELDSIZE);
					c->logged_in=1;
					printf("Bem vindo %s\n",c->username);
					return;
				}
				
			}
			if(strcmp((correctPassword=find_login_pw_in_login_arr(username[1],currAdmins)),"NO_SUCH_LOGIN")){
				
				if(!memcmp(correctPassword,password[1],strlen(correctPassword))&&!clientIsRepeated(c)){
					memcpy(targetinout,defaultTargetAdmin,strlen(defaultTargetAdmin));
					strncpy(c->username,username[1],FIELDSIZE);
					c->logged_in=1;
					c->isAdmin=1;
					printf("Bem vindo %s\n",c->username);
					return;
				}
				
			}
	if(clientIsRepeated(c)){
		memset(c->username,0,FIELDSIZE);
		memcpy(targetinout,doubleSessionTarget,strlen(doubleSessionTarget));
		return;
	}
	else{
	memcpy(targetinout,incorrectLoginTarget,strlen(incorrectLoginTarget));
	}
}
void handleLogout(client* c,char targetinout[PATHSIZE]){

	
        memset(c->username,0,FIELDSIZE);
	c->isAdmin=0;
	c->running_time=0.0;
	c->logged_in=0;
	memcpy(targetinout,defaultLoginTarget,strlen(defaultLoginTarget));
}
