#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/load_logins.h"
#include "../Includes/server_innards.h"
#include "../Includes/sock_ops.h"
#include "../Includes/session_ops.h"
#include <time.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#define READ_FUNC_TO_USE readall
#define SEND_SOCK_BUFF_SIZE 10000000
static socklen_t socklenpointer;
static int server_socket,numOfClients,max_sd,currNumOfClients;

static struct sockaddr_in server_address, clientAddress;

static fd_set readfds;

static char* peerbuff=NULL;

static char* peerbuffcopy=NULL;

static char addressContainer[INET_ADDRSTRLEN];

FILE* logstream;
static double running_time=0.0;

static int sendMediaData(client*c,char* buff,char* mimetype){

	return sendResource(c,buff,mimetype,USEFD);
}
client* getClientArrCopy(void){
	
	client* result= malloc(currNumOfClients*sizeof(client));
	for(int i=0;i<currNumOfClients;i++){
		
		memset(result[i].username,0,FIELDSIZE);
		result[i].socket=clients[i].socket;
		result[i].isAdmin=clients[i].isAdmin;
		result[i].logged_in=clients[i].logged_in;
		result[i].running_time=clients[i].running_time;
		strncpy(result[i].username,clients[i].username,FIELDSIZE);
		memcpy(&result[i].client_addr,&clients[i].client_addr,sizeof(clients[i].client_addr));
	}
	return result;

}

client* getFullClientArrCopy(void){
	
	client* result= malloc(numOfClients*sizeof(client));
	for(int i=0;i<numOfClients;i++){
		
		memset(result[i].username,0,FIELDSIZE);
		result[i].socket=clients[i].socket;
		result[i].isAdmin=clients[i].isAdmin;
		result[i].logged_in=clients[i].logged_in;
		result[i].running_time=clients[i].running_time;
		strncpy(result[i].username,clients[i].username,FIELDSIZE);
		memcpy(&result[i].client_addr,&clients[i].client_addr,sizeof(clients[i].client_addr));
	}
	return result;

}

int getCurrNumOfClients(void){
	
	return currNumOfClients;


}
int getMaxNumOfClients(void){
	
	return numOfClients;


}
static void dropConnection(int sd){
getpeername(sd , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                    if(logging){
			fprintf(logstream,"Host disconnected. Cliente com ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
                   
                    }
                        //Close the socket and mark as 0 in lis>

                    close(sd);
}
static void handleDisconnect(client* c){
		dropConnection(c->socket);
		//Close the socket and mark as 0 in list for reuse
               
                    c->socket = 0;
		    c->running_time=0.0;
		    currNumOfClients--;
}
static void sigint_handler(int signal){
	signal=0;
	close(server_socket);
	for(int i=0;i<numOfClients;i++){
	if(clients[i].socket>0){
		close(clients[i].socket);
	}
	}
	
	free(clients);
	if(logging){
	fprintf(logstream,"Adeus! Server out...\n");
	}
	fclose(logstream);
	if(peerbuff){

	free(peerbuff);

	}
	if(peerbuffcopy){
	
	free(peerbuffcopy);

	}
	if(currLogins){
	
	freeLogins(&currLogins);


	}
	if(currAdmins){
	
	freeLogins(&currAdmins);


	}
	printf("Adeus! Server out...\n");
	exit(-1+signal);

}
static void sigpipe_handler(int signal){
	signal=0;
	if(logging){
	fprintf(logstream,"SIGPIPE!!!!!\n");
	}
	handleDisconnect(clientToDrop+signal);
	
}
static void initializeClients(void){

FD_ZERO(&readfds);
		FD_SET(server_socket,&readfds);
		max_sd=server_socket;
	for (int i = 0 ; i < numOfClients ; i++)   
        {   
        if(clients[i].logged_in){
	double client_run_time=clients[i].running_time;
        clients[i].running_time=client_run_time+running_time;	
	if(clients[i].running_time>SESSION_TIME_USECS){

		clients[i].logged_in=0;
	}
	}
	    //socket descriptor  
         int sd = clients[i].socket;   
        
            //if valid socket descriptor then add to read list  
            if(sd >= 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)
                max_sd = sd;
        }

}

static void handleIncommingConnections(void){

	int client_socket;
	int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }
	if(FD_ISSET(server_socket,&readfds)){

		
		if ((client_socket = accept(server_socket,(struct sockaddr*)(&clientAddress),&socklenpointer))<0)
		{
		if(logging){
                fprintf(logstream,"accept error!!!!: %s\n",strerror(errno));
        	}
		raise(SIGINT);
		
        	}
		currNumOfClients++;
		if(currNumOfClients==numOfClients){
		dropConnection(client_socket);
		
		}
		setNonBlocking(client_socket);
		//setLinger(client_socket,0,0);
		getpeername(client_socket , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                if(logging){
		fprintf(logstream,"Client connected , ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
        	}
		//setSocketSendBuffSize(client_socket,SEND_SOCK_BUFF_SIZE);
	    int i=0;		//add new socket to array of sockets
            for (; i < numOfClients; i++)
            {
                //if position is empty
                if( !clients[i].socket )
                {
                    clients[i].socket = client_socket;
		    memset(clients[i].username,0,FIELDSIZE);
		    memcpy(&clients[i].client_addr,&clientAddress,sizeof(struct sockaddr_in));
		    if(logging){
		    fprintf(logstream,"Adding to list of sockets as %d\n" , i);
                    }
		    break;
                }
            }

				
}

}
static void handleCurrentActivity(client*c,http_request req){
	
	http_header header=*(req.header);
	/*
	if(!clientIsLoggedIn(c)&&!stringsAreEqual(header.target,SIGN_IN_REQ)){
		char buff[PATHSIZE]={0};
		handleLogout(c,buff);
		sendMediaData(c,buff,defaultMimetype);
		printf("Cliente naaaaaaaoooo esta signed in!!!!\n");
		//sendMediaData(c,defaultLoginTarget,defaultMimetype);
		return;
	}
	printf("Cliente esta signed in!!!!\n");*/
	if(!strcmp(header.target,"/")){

		strcpy(header.target,defaultLoginTarget);
	}
	if(logging){
		fprintf(logstream,"%s\n",header.target);
	}
	switch(header.type){
	case GET:
			if(isCustomGetReq(header.target)){

				char targetinout[PATHSIZE]={0};

				handleCustomGetReq(c,header.target,req.data,targetinout);
				int result=sendMediaData(c,targetinout,defaultMimetype);
				if(result<0){

					sendMediaData(c,notFoundTarget,defaultMimetype);

				}
				deleteClientListingHTML();
			}
			else{
				int isDir=sendMediaData(c,header.target,header.mimetype);
				if(isDir<0){
					sendMediaData(c,notFoundTarget,defaultMimetype);
				
				}
				else if(isDir>0){
					sendMediaData(c,generateDirListing(header.target),defaultMimetype);
					deleteDirListingHTML();
				}
			}
	break;
	case POST:
			if(isCustomPostReq(header.target)){

				char targetinout[PATHSIZE]={0};
				handleCustomPostReq(c,header.target,req.data,targetinout);
				int result=sendMediaData(c,targetinout,defaultMimetype);
				if(result<0){

					sendMediaData(c,notFoundTarget,defaultMimetype);

				}
				deleteClientListingHTML();
			}
			else{
				int result=sendMediaData(c,header.target,header.mimetype);
				if(result<0){
					sendMediaData(c,notFoundTarget,defaultMimetype);
				}
				else if(result>0){
					sendMediaData(c,generateDirListing(header.target),defaultMimetype);
					deleteDirListingHTML();
				}
			}
	break;
	default:
		
		sendMediaData(c,header.target,defaultMimetype);
	break;
	}
	
}

static void handleCurrentConnections(client* c){
 			memset(peerbuff,0,PAGE_DATA_SIZE);
			memset(peerbuffcopy,0,PAGE_DATA_SIZE);
			if(READ_FUNC_TO_USE(c->socket,peerbuff,PAGE_DATA_SIZE-1)!=-2){
                  	if(errno == ECONNRESET){
				handleDisconnect(c);
			}
			else if(strlen(peerbuff)){
				memcpy(peerbuffcopy,peerbuff,PAGE_DATA_SIZE);
				http_request* req=spawnHTTPRequest(peerbuff);
				if(logging){
				fprintf(logstream,"Recebemos request!!!: \n");
				print_http_req(logstream,*req);
				}
				if(!strlen(req->data)&&(req->header->content_length>0)){
				free(req->data);
				req->data=malloc(req->header->content_length+1);
				memset(req->data,0,req->header->content_length+1);
				if(READ_FUNC_TO_USE(c->socket,req->data,req->header->content_length)!=-2){

				

					fprintf(logstream,"Recebemos dados!!!: %s\n",req->data);
				
				}
				}
				handleCurrentActivity(c,*req);
				free(req->data);
				free(req->header);
				free(req);
			}
			}
			else{
			
			handleDisconnect(c);
		
			}
			
}


static void handleActivityInSockets(void){
	
	for (int i = 0; i < numOfClients; i++)
        {
	int sd = clients[i].socket;
    	
	if (FD_ISSET(sd, &readfds))
            {
		
		handleCurrentConnections(&(clients[i]));
                
            }

	}

}
static void mainLoop(void){
	peerbuff=malloc(PAGE_DATA_SIZE);
 	peerbuffcopy=malloc(PAGE_DATA_SIZE);
	
	
	struct timeval start, end;
   	long secs_used;
	while(1){
	
	gettimeofday(&start, NULL);
	
	initializeClients();
	handleIncommingConnections();
	handleActivityInSockets();
	
    	gettimeofday(&end, NULL);
	secs_used=(end.tv_sec - start.tv_sec);
	running_time=(double) (((secs_used*1000000.0) + end.tv_usec) -(double) (start.tv_usec));
	}
	free(peerbuff);
	free(peerbuffcopy);

}

void initializeServer(int max_quota,int logs){
	
/*	if(!(logstream=fopen(logfpath,"w"))){
	
		perror("logs will be made to stdout!!!! could not create log file\n");
		logstream=stdout;
	}
*/	
	logstream=stdout;
	
	logging=logs;
	beeping=0;
	currNumOfClients=0;
	numOfClients=max_quota;
	clients=malloc(sizeof(client)*numOfClients);
	for(int i=0;i<numOfClients;i++){

		clients[i].socket=0;
		clients[i].logged_in=0;
		clients[i].isAdmin=0;
		clients[i].running_time=0;
		memset(clients[i].username,0,FIELDSIZE);
		memset(&clients[i].client_addr,0,sizeof(struct sockaddr_in));
		
	}
	signal(SIGINT,sigint_handler);
	
	signal(SIGPIPE,sigpipe_handler);
	
	
	server_socket= socket(AF_INET,SOCK_STREAM,0);
        int ptr=1;
        setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&ptr,sizeof(ptr));
        if(server_socket==-1){
                raise(SIGINT);

        }

	
	memset(currDir,0,PATHSIZE);
	
	getcwd(currDir,PATHSIZE);

        server_address.sin_family=AF_INET;
        server_address.sin_port= htons(PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;
        socklen_t socklength=sizeof(server_address);
        
	bind(server_socket,(struct sockaddr*)(&server_address),socklength);

        listen(server_socket,MAX_CLIENTS_HARD_LIMIT);
	
        struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&server_address;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        
	inet_ntop( AF_INET, &ipAddr, addressContainer, INET_ADDRSTRLEN );
	if(logging){
	fprintf(logstream,"Server spawnado @ %s\n",inet_ntoa(server_address.sin_addr));
	}
	socklenpointer=sizeof(clientAddress);
	loadLogins();
	loadAdmins();
	mainLoop();
	freeLogins(&currLogins);
}

