#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/server_innards.h"
static socklen_t socklenpointer;

static int server_socket,numOfClients,*client_sockets,max_sd;

static struct sockaddr_in server_address, clientAddress;

static fd_set readfds;

static char peerbuff[PAGE_DATA_SIZE];

static char addressContainer[INET_ADDRSTRLEN];

static void sigint_handler(int signal){
	
	close(server_socket);
	for(int i=0;i<numOfClients;i++){
	if(client_sockets[i]){
		close(client_sockets[i]);
	}
	}
	
	free(mainpage.data);
	free(client_sockets);
	printf("Adeus! Server out...\n");
	exit(-1);

}

static void open_resource(page* p,char* ext){
		
		if(!p->pagestream){
		p->pagestream=fopen(p->pagepath,"r");
		fseek(p->pagestream,0,SEEK_END);
		p->data_size=ftell(p->pagestream);
		fseek(p->pagestream,0,SEEK_SET);

		char headerBuff[LINESIZE]={0};
		p->headerFillFunc(headerBuff,p->data_size,ext);

		p->header_size=strlen(headerBuff);
		p->data=malloc(p->header_size+p->data_size);
		memset(p->data,0,p->header_size+p->data_size);
		printf("%s %p\n",p->pagepath,p->pagestream);
		
		char buff[1024]={0};
		char * ptr= p->data;
		ptr+=snprintf(p->data,PAGE_DATA_SIZE,"%s",headerBuff);
		while(fgets(buff,1024,p->pagestream)){
			
			ptr+=snprintf(ptr,1024,"%s",buff);
		}
		fclose(p->pagestream);
		p->pagestream=NULL;
	
		}

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
                perror("accept");
                exit(EXIT_FAILURE);
        	}
		//add new socket to array of sockets
            for (int i = 0; i < numOfClients; i++)
            {
                //if position is empty
                if( client_sockets[i] == 0 )
                {
                    client_sockets[i] = client_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                    break;
                }
            }
}

}
static void handleDisconnect(int i,int sd){

    		getpeername(sd , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                    printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_sockets[i] = 0;


}

static void sendMediaData(int sd,char* buff){

	char path[LINESIZE]={0};
	char* ptr= path;
	page p;
	ptr+=snprintf(ptr,LINESIZE,"%s%s",RESOURCES_PATH,buff);
	p.data=NULL;
	p.pagestream=NULL;
	p.pagepath=malloc(LINESIZE);
	memset(p.pagepath,0,LINESIZE);
	memcpy(p.pagepath,path,strlen(path));
	char* ext=get_file_extension(p.pagepath);
	if(findInStringArr(videoExtArr,ext)>=0){
	p.headerFillFunc=&fillUpVideoHeader;
	}
	if(findInStringArr(pageExtArr,ext)>=0){
	
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
	open_resource(&p,ext);
	if(p.data){
	if(sendall(sd,p.data,p.data_size+p.header_size)!=(p.data_size+p.header_size)){
	
		perror("ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!\n");
	}
	free(p.data);
	
	}
	if(p.pagepath){
	free(p.pagepath);
	}


}

static void handleCurrentActivity(int sd){
		
        char* argv[ARGVMAX];
      memset(argv,0,ARGVMAX*sizeof(char*));
	char* pagebuffcopy=malloc(PAGE_DATA_SIZE);
	memset(pagebuffcopy,0,PAGE_DATA_SIZE);
	memcpy(pagebuffcopy,peerbuff,PAGE_DATA_SIZE);
	int argc=makeargv(peerbuff,argv);
	if(!strcmp(argv[0],"GET")){
	if(argc>1){
	
			
		if(!strcmp(argv[1],"/")){
		
		
			if(sendall(sd,mainpage.data,mainpage.data_size+mainpage.header_size+1)!=(mainpage.data_size+mainpage.header_size+1)){
	
				perror("ERRO NO SEND!!!! PEDIRAM A ROOT!!!!\n");
			}
		}
		else{
			printf("%s\n",argv[1]);
			sendMediaData(sd,argv[1]);
		}
	}
	}
	else{
		
			if(sendall(sd,mainpage.data,mainpage.data_size+mainpage.header_size+1)!=(mainpage.data_size+mainpage.header_size+1)){
	
				perror("ERRO NO SEND!!!! PEDIRAM A ROOT!!!!\n");
			}
	}
	free(pagebuffcopy);

}

static void handleCurrentConnections(int i,int sd){
 		int optval;
	socklen_t optlen = sizeof(optval);
	if (getsockopt(sd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0) {
    		if (optval == 0) {
        	// Socket is open
   	 		readall(sd,peerbuff,PAGE_DATA_SIZE);
                  	handleCurrentActivity(sd);
		
		} else {
        		handleDisconnect(i,sd);
		}
		} else {
    		// Error handling for getsockopt
		}
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
	
	numOfClients=max_quota;
	client_sockets=malloc(sizeof(int)*numOfClients);
	for(int i=0;i<numOfClients;i++){

		client_sockets[i]=0;
	}
	signal(SIGINT,sigint_handler);
	
	open_resource(&mainpage,"html");

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
	printf("Server spawnado @ %s\n",inet_ntoa(server_address.sin_addr));
	socklenpointer=sizeof(clientAddress);

	mainLoop();
}

