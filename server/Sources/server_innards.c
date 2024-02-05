#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/server_innards.h"
#include <errno.h>
#define READ_FUNC_TO_USE readall
#define SEND_FUNC_TO_USE sendall
static socklen_t socklenpointer;
static int server_socket,numOfClients,*client_sockets,max_sd;

static struct sockaddr_in server_address, clientAddress;

static fd_set readfds;

static char peerbuff[PAGE_DATA_SIZE];

static char peerbuffcopy[PAGE_DATA_SIZE];

static char addressContainer[INET_ADDRSTRLEN];

static char* argv[ARGVMAX];
      	
FILE* logstream;
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
	
	close(server_socket);
	for(int i=0;i<numOfClients;i++){
	if(client_sockets[i]){
		close(client_sockets[i]);
	}
	}
	
	free(mainpage.data);
	free(notfoundpage.data);
	free(client_sockets);
	fprintf(logstream,"Adeus! Server out...\n");
	fclose(logstream);
	printf("Adeus! Server out...\n");
	exit(-1);

}

static int open_resource(page* p,char* ext){
		
		if(!p->pagestream){
		if(!(p->pagestream=fopen(p->pagepath,"r"))){

			fprintf(logstream,"Invalid filepath: %s\n%s\n",p->pagepath,strerror(errno));
			return -1;
		}
		fseek(p->pagestream,0,SEEK_END);
		p->data_size=ftell(p->pagestream);
		fseek(p->pagestream,0,SEEK_SET);

		char headerBuff[LINESIZE]={0};
		p->headerFillFunc(headerBuff,p->data_size,ext);

		p->header_size=strlen(headerBuff);
		p->data=malloc(p->header_size+p->data_size+1);
		memset(p->data,0,p->header_size+p->data_size+1);
		fprintf(logstream,"%s %p\n",p->pagepath,p->pagestream);
		
		char buff[LINESIZE]={0};
		char * ptr= p->data;
		ptr+=snprintf(p->data,PAGE_DATA_SIZE,"%s",headerBuff);
		while(fgets(buff,LINESIZE,p->pagestream)){
			
			ptr+=snprintf(ptr,LINESIZE,"%s",buff);
		}
		fclose(p->pagestream);
		p->pagestream=NULL;
		return 0;
		}
	return -1;
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
            if(sd > 0)   
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
                fprintf(logstream,"accept error!!!!: %s\n",strerror(errno));
        	raise(SIGINT);
		
        	}
		setNonBlocking(client_socket);
		getpeername(client_socket , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                fprintf(logstream,"Client connected , ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
                		//add new socket to array of sockets
            for (int i = 0; i < numOfClients; i++)
            {
                //if position is empty
                if( client_sockets[i] == 0 )
                {
                    client_sockets[i] = client_socket;
                    fprintf(logstream,"Adding to list of sockets as %d\n" , i);
                    break;
                }
            }
}

}
static void handleDisconnect(int i,int sd){

    		getpeername(sd , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                    fprintf(logstream,"Host disconnected , ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_sockets[i] = 0;


}

static void sendMediaData(int sd,char* buff){

	page p;
	memset(p.pagepath,0,LINESIZE);
	char* ptr= p.pagepath;
	ptr+=snprintf(ptr,LINESIZE,"%s%s",RESOURCES_PATH,buff);
	p.data=NULL;
	p.pagestream=NULL;
	char* ext=get_file_extension(p.pagepath);
	if(findInStringArr(videoExtArr,ext)>=0){
	p.headerFillFunc=&fillUpVideoHeader;
	}
	else if(findInStringArr(jsExtArr,ext)>=0){
	p.headerFillFunc=&fillUpJsHeader;
	}
	else if(findInStringArr(pageExtArr,ext)>=0){
	
	p.headerFillFunc=&fillUpPageHeader;
	}
	else if(findInStringArr(audioExtArr,ext)>=0){
	p.headerFillFunc=&fillUpAudioHeader;
	}
	else if(findInStringArr(imageExtArr,ext)>=0){
	p.headerFillFunc=&fillUpImageHeader;
	}
	else if(findInStringArr(manifestExtArr,ext)>=0){
	p.headerFillFunc=&fillUpManifestHeader;
	}
	else if(findInStringArr(iconExtArr,ext)>=0){
	p.headerFillFunc=&fillUpIconHeader;
	}
	if(!open_resource(&p,ext)){
	if(p.data){
	if(SEND_FUNC_TO_USE(sd,p.data,p.data_size+p.header_size)!=(p.data_size+p.header_size)){
	
		fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
	}
	free(p.data);
	}
	}
	else{
	if(SEND_FUNC_TO_USE(sd,notfoundpage.data,notfoundpage.data_size+notfoundpage.header_size)!=(notfoundpage.data_size+notfoundpage.header_size)){
	
		fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO QUE N EXISTE NO SERVER!!!!:\n%s\n",strerror(errno));
	}
	}


}

static void handleCurrentActivity(int sd){
		
        memset(argv,0,ARGVMAX*sizeof(char*));
	int argc=makeargv(peerbuff,argv);
	if(!strcmp(argv[0],"GET")){
	if(argc>1){
	
			
		if(!strcmp(argv[1],"/")){
		
		
			if(SEND_FUNC_TO_USE(sd,mainpage.data,mainpage.data_size+mainpage.header_size+1)!=(mainpage.data_size+mainpage.header_size+1)){
	
				fprintf(logstream,"ERRO NO SEND!!!! PEDIRAM A ROOT!!!!\n:%s\n",strerror(errno));
			}
		}
		else{
			fprintf(logstream,"%s\n",argv[1]);
			sendMediaData(sd,argv[1]);
		}
	}
	}
	else{
		
			if(SEND_FUNC_TO_USE(sd,mainpage.data,mainpage.data_size+mainpage.header_size+1)!=(mainpage.data_size+mainpage.header_size+1)){
	
				fprintf(logstream,"ERRO NO SEND!!!! PEDIRAM A ROOT!!!!:\n%s\n",strerror(errno));
			}
	}

}

static void handleCurrentConnections(int i,int sd){
 		
			//if(sd){
			if(READ_FUNC_TO_USE(sd,peerbuff,PAGE_DATA_SIZE)!=-2){
                  	if(errno == ECONNRESET){
				handleDisconnect(i,sd);
			}
			else{
				handleCurrentActivity(sd);
			}
			}
			else{
			
			handleDisconnect(i,sd);
		
			}
			//}
		
}

static void handleActivityInSockets(void){
	
	for (int i = 0; i < numOfClients; i++)
        {
            int sd = client_sockets[i];
    	
	if (FD_ISSET( sd , &readfds))
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

void initializeServer(int max_quota){
	
	/*if(!(logstream=fopen(logfpath,"w"))){
	
		perror("logs will be made to stdout!!!! could not create log file\n");
		logstream=stdout;
	}*/
	logstream=stdout;

	numOfClients=max_quota;
	client_sockets=malloc(sizeof(int)*numOfClients);
	for(int i=0;i<numOfClients;i++){

		client_sockets[i]=0;
	}
	signal(SIGINT,sigint_handler);
	
	open_resource(&mainpage,"html");
	open_resource(&notfoundpage,"html");

	server_socket= socket(AF_INET,SOCK_STREAM,0);
        int ptr=1;
        setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&ptr,sizeof(ptr));
        if(server_socket==-1){
                raise(SIGINT);

        }

	
        server_address.sin_family=AF_INET;
        server_address.sin_port= htons(PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;
        socklen_t socklength=sizeof(server_address);
        
	bind(server_socket,(struct sockaddr*)(&server_address),socklength);

        listen(server_socket,MAX_CLIENTS_HARD_LIMIT);
	
        struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&server_address;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        
	inet_ntop( AF_INET, &ipAddr, addressContainer, INET_ADDRSTRLEN );
	fprintf(logstream,"Server spawnado @ %s\n",inet_ntoa(server_address.sin_addr));
	printf("Server spawnado @ %s\n",inet_ntoa(server_address.sin_addr));
	socklenpointer=sizeof(clientAddress);

	mainLoop();
}

