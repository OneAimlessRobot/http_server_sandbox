#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include <errno.h>
#define READ_FUNC_TO_USE readall
static socklen_t socklenpointer;
static int server_socket,numOfClients,*client_sockets,max_sd;

static struct sockaddr_in server_address, clientAddress;

static fd_set readfds;

static char* peerbuff=NULL;

static char* peerbuffcopy=NULL;

static char addressContainer[INET_ADDRSTRLEN];

FILE* logstream;
static void* beepnotify(void* args){
	
	char buff[PATHSIZE]={0};
	snprintf(buff,PATHSIZE,"%s %d %d",CADENCE_BEEPER_PATH,BEEP_TIMES,BEEP_INTERVAL);
	system(buff);
	return args;

}


static void handleDisconnect(int i,int sd){
		if(beeping){
			pthread_t beeper;
			pthread_create(&beeper,NULL,&beepnotify,NULL);
			pthread_detach(beeper);
		}
    		getpeername(sd , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                    if(logging){
			fprintf(logstream,"Host disconnected. Cliente numero: %d , ip %s , port %d \n" ,i,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
                    }
			//Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_sockets[i] = 0;


}

static void setLinger(int socket,int onoff,int time){

struct linger so_linger;
so_linger.l_onoff = onoff; // Enable linger option
so_linger.l_linger = time; // Linger time, set to 0

if (setsockopt(socket, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
    perror("setsockopt");
    // Handle error
}

}
static void setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        fprintf(logstream,"erro a atribuir flags a uma socket de cliente: %s\n",strerror(errno));
        return;
    }

    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        fprintf(logstream,"erro a atribuir flags a uma socket de cliente: %s\n",strerror(errno));
    }
}

static void sigint_handler(int signal){
	signal=0;
	close(server_socket);
	for(int i=0;i<numOfClients;i++){
	if(client_sockets[i]>0){
		close(client_sockets[i]);
	}
	}
	
	free(client_sockets);
	if(logging){
	fprintf(logstream,"Adeus! Server out...\n");
	}
	fclose(logstream);
	printf("Adeus! Server out...\n");
	exit(-1+signal);

}
static void sigpipe_handler(int signal){
	signal=0;
	if(logging){
	fprintf(logstream,"SIGPIPE!!!!!\n");
	}
	handleDisconnect(peerToDrop,socketToClose+signal);
	
}
int testOpenResource(int sd,int clientIndex,char* resourceTarget,char* mimetype){

	page p;
	memset(p.pagepath,0,PATHSIZE);
	char* ptr= p.pagepath;
	p.headerFillFunc=&fillUpGeneralHeader;
	ptr+=snprintf(ptr,PATHSIZE,"%s/resources%s",currDir,resourceTarget);
	if(logging){
	fprintf(logstream,"Fetching %s...\n",p.pagepath);
	}
	if(!(p.pagestream=fopen(p.pagepath,"r"))){
			if(logging){
			fprintf(logstream,"Invalid filepath: %s\n%s\n",p.pagepath,strerror(errno));
			}
			return -1;
	}
	if(logging){
	fprintf(logstream,"Done!\n");
	}
		fseek(p.pagestream,0,SEEK_END);
		p.data_size=ftell(p.pagestream)+1;
		fseek(p.pagestream,0,SEEK_SET);
		
		char headerBuff[PATHSIZE]={0};
		p.headerFillFunc(headerBuff,p.data_size,mimetype);

		p.header_size=strlen(headerBuff);
		
			if(send(sd,headerBuff,p.header_size,0)!=(p.header_size)){
				if(logging){
				fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
				}
			}
		sendallchunked(sd,clientIndex,p.pagestream);
		
		fclose(p.pagestream);
		return 0;
}
int testOpenResourcefd(int sd,int clientIndex,char* resourceTarget,char* mimetype){

	page p;
	memset(p.pagepath,0,PATHSIZE);
	char* ptr= p.pagepath;
	p.headerFillFunc=&fillUpGeneralHeader;
	ptr+=snprintf(ptr,PATHSIZE,"%s/resources%s",currDir,resourceTarget);
	if(logging){
	fprintf(logstream,"Fetching %s...\n",p.pagepath);
	}
	if((p.pagefd=open(p.pagepath,O_RDONLY,0777))<0){
			if(logging){
			fprintf(logstream,"Invalid filepath: %s\n%s\n",p.pagepath,strerror(errno));
			}
			return -1;
	}
	if(logging){
	fprintf(logstream,"Done!\n");
	}
		lseek(p.pagefd,0,SEEK_END);
		p.data_size=lseek(p.pagefd,0,SEEK_CUR)+1;
		lseek(p.pagefd,0,SEEK_SET);
		
		char headerBuff[PATHSIZE]={0};
		p.headerFillFunc(headerBuff,p.data_size,mimetype);

		p.header_size=strlen(headerBuff);
		
			if(send(sd,headerBuff,p.header_size,0)!=(p.header_size)){
				if(logging){
				fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
				}
			}
		sendallchunkedfd(sd,clientIndex,p.pagefd);
		
		close(p.pagefd);
		return 0;
}

static void initializeClients(void){

FD_ZERO(&readfds);
		FD_SET(server_socket,&readfds);
		max_sd=server_socket;
	for (int i = 0 ; i < numOfClients ; i++)   
        {   
            //socket descriptor  
         int sd = client_sockets[i];   
                 
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
		setNonBlocking(client_socket);
		setLinger(client_socket,0,0);
		getpeername(client_socket , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                if(logging){
		fprintf(logstream,"Client connected , ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
        	}
			//add new socket to array of sockets
            for (int i = 0; i < numOfClients; i++)
            {
                //if position is empty
                if( !client_sockets[i] )
                {
                    client_sockets[i] = client_socket;
                    if(logging){
		    fprintf(logstream,"Adding to list of sockets as %d\n" , i);
                    }
		    break;
                }
            }
}

}
static int sendMediaData(int sd,int clientIndex,char* buff,char* mimetype){

	return testOpenResourcefd(sd,clientIndex,buff,mimetype);
}

static void handleCurrentActivity(int sd,int clientIndex,http_request req){
	http_header header=*(req.header);
	switch(header.type){
	case GET:
	
			
		if(!strcmp(header.target,"/")){
		
			sendMediaData(sd,clientIndex,defaultTarget,defaultMimetype);
		
		}
		else{
			if(logging){
			fprintf(logstream,"%s\n",header.target);
			}
			char* string=header.target;
			if(isCustomGetReq(string)){
				
				char targetinout[PATHSIZE]={0};
				handleCustomGetReq(string,targetinout);
				
				if(sendMediaData(sd,clientIndex,targetinout,defaultMimetype)<0){
					
					sendMediaData(sd,clientIndex,notFoundTarget,defaultMimetype);

				}
			
			}
			else{
				if(sendMediaData(sd,clientIndex,header.target,header.mimetype)<0){
					
					sendMediaData(sd,clientIndex,notFoundTarget,defaultMimetype);

				}
			}
		}
	break;
	case POST:
		
		if(!strcmp(header.target,"/")){
		
			sendMediaData(sd,clientIndex,defaultTarget,defaultMimetype);
		
		}
		else{
			if(logging){
			fprintf(logstream,"%s\n",header.target);
			}
			char* string=header.target;
			if(isCustomPostReq(string)){

				char targetinout[PATHSIZE]={0};
				
				handleCustomPostReq(string,req.data,targetinout);
				if(sendMediaData(sd,clientIndex,targetinout,defaultMimetype)<0){
					
					sendMediaData(sd,clientIndex,notFoundTarget,defaultMimetype);

				}

			}
			else{

				
				if(sendMediaData(sd,clientIndex,header.target,header.mimetype)<0){
					
					sendMediaData(sd,clientIndex,notFoundTarget,defaultMimetype);

				}
			}
		}
	break;
	default:
		
		sendMediaData(sd,clientIndex,defaultTarget,defaultMimetype);
	break;
	}
}

static void handleCurrentConnections(int i,int sd){
 			peerbuff=malloc(PAGE_DATA_SIZE);
 			peerbuffcopy=malloc(PAGE_DATA_SIZE);
			memset(peerbuff,0,PAGE_DATA_SIZE);
			memset(peerbuffcopy,0,PAGE_DATA_SIZE);
			if(READ_FUNC_TO_USE(sd,peerbuff,PAGE_DATA_SIZE)!=2){
                  	if(errno == ECONNRESET){
				handleDisconnect(i,sd);
			}
			else if(strlen(peerbuff)){
				memcpy(peerbuffcopy,peerbuff,PAGE_DATA_SIZE);
				http_request* req=spawnHTTPRequest(peerbuff);
				if(logging){
				fprintf(logstream,"Recebemos request!!!:\s%s\n",peerbuffcopy);
				print_http_req(logstream,*req);
				}
				handleCurrentActivity(sd,i,*req);
				free(req->data);
				free(req->header);
				free(req);
			}
			}
			else{
			
			handleDisconnect(i,sd);
		
			}
			free(peerbuff);
			free(peerbuffcopy);
		
}


static void handleActivityInSockets(void){
	
	for (int i = 0; i < numOfClients; i++)
        {
            int sd = client_sockets[i];
    	
	if (FD_ISSET(sd, &readfds))
            {
		handleCurrentConnections(i,sd);
                
            }

	}

}
static void mainLoop(void){

	while(1){
	
	initializeClients();
	handleIncommingConnections();
	handleActivityInSockets();
    }

}

void initializeServer(int max_quota,int logs){
	/*
	if(!(logstream=fopen(logfpath,"w"))){
	
		perror("logs will be made to stdout!!!! could not create log file\n");
		logstream=stdout;
	}
	*/
	logstream=stdout;
	
	logging=logs;
	beeping=0;
	numOfClients=max_quota;
	client_sockets=malloc(sizeof(int)*numOfClients);
	for(int i=0;i<numOfClients;i++){

		client_sockets[i]=0;
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

	mainLoop();
}

