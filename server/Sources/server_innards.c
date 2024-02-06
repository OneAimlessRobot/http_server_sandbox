#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
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

static char currDir[PATHSIZE];
FILE* logstream;

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
	
	close(server_socket);
	for(int i=0;i<numOfClients;i++){
	if(client_sockets[i]>=0){
		close(client_sockets[i]);
	}
	}
	
	free(client_sockets);
	fprintf(logstream,"Adeus! Server out...\n");
	fclose(logstream);
	printf("Adeus! Server out...\n");
	exit(-1);

}
static void sigpipe_handler(int signal){
	perror("SIGPIPE!!!!!\n");
	raise(SIGINT);
}

int testOpenResource(int sd,char* resourceTarget,char* mimetype){

	page p;
	memset(p.pagepath,0,PATHSIZE);
	char* ptr= p.pagepath;
	p.headerFillFunc=&fillUpGeneralHeader;
	ptr+=snprintf(ptr,PATHSIZE,"%s/resources%s",currDir,resourceTarget);
	printf("%s\n",p.pagepath);
	p.pagestream=NULL;
	if(!p.pagestream){
		if(!(p.pagestream=fopen(p.pagepath,"r"))){

			fprintf(logstream,"Invalid filepath: %s\n%s\n",p.pagepath,strerror(errno));
			return -1;
		}
		fseek(p.pagestream,0,SEEK_END);
		p.data_size=ftell(p.pagestream);
		fseek(p.pagestream,0,SEEK_SET);
		
		char headerBuff[PATHSIZE]={0};
		p.headerFillFunc(headerBuff,p.data_size,mimetype);

		p.header_size=strlen(headerBuff);
		char buff[BUFFSIZE+1]={0};
		int numread=0;
		
			if(SEND_FUNC_TO_USE(sd,headerBuff,p.header_size)!=(p.header_size)){
	
				fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
			}
			
			memset(buff,0,BUFFSIZE+1);
		
		while(numread=fread(buff,1,BUFFSIZE+1,p.pagestream)){
			
			if(SEND_FUNC_TO_USE(sd,buff,numread)!=(numread)){
	
				fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
			}
			
			memset(buff,0,BUFFSIZE+1);
		}
		fclose(p.pagestream);
		p.pagestream=NULL;
		return 0;
		}
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
		setLinger(client_socket,0,0);
		getpeername(client_socket , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                fprintf(logstream,"Client connected , ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
                		//add new socket to array of sockets
            for (int i = 0; i < numOfClients; i++)
            {
                //if position is empty
                if( client_sockets[i] >= 0 )
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
                    client_sockets[i] = -1;


}

static int sendMediaData(int sd,char* buff,char* mimetype){

	return testOpenResource(sd,buff,mimetype);
}

static void handleCurrentActivity(int sd,http_header header){
	
	switch(header.type){
	case GET:
	
			
		if(!strcmp(header.target,"/")){
		
			sendMediaData(sd,defaultTarget,defaultMimetype);
		
		}
		else{
			fprintf(logstream,"%s\n",header.target);
			char* string=header.target;
			char* argv2[2];
			memset(argv2,0,2*sizeof(char*));
			splitString(string,"?",argv2);
			sendMediaData(sd,argv2[0],header.mimetype);
		}
	break;
	default:
		
		sendMediaData(sd,defaultTarget,defaultMimetype);
	break;
	}
}

static void handleCurrentConnections(int i,int sd){
 			
			memset(peerbuff,0,PAGE_DATA_SIZE);
			if(READ_FUNC_TO_USE(sd,peerbuff,PAGE_DATA_SIZE)!=2){
                  	if(errno == ECONNRESET){
				handleDisconnect(i,sd);
			}
			else if(strlen(peerbuff)){
				
				fprintf(logstream,"O Request basicamente foi:\n");
				http_header header=spawnHTTPRequest(peerbuff).header;
				print_http_req_header(logstream,header);
				handleCurrentActivity(sd,header);
			}
			}
			else{
			
			handleDisconnect(i,sd);
		
			}
		
}

static int dataIsAvailable(int sockfd) {
    
	struct timeval tv;
        tv.tv_sec=0;
        tv.tv_usec=0;
                
    // Check if data is available
    int result = select(sockfd + 1, &readfds, NULL, NULL, &tv);

    if (result < 0) {
        perror("select");
	raise(SIGINT);
	}

    return FD_ISSET(sockfd, &readfds);
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
	fprintf(logstream,"Server spawnado @ %s\n",inet_ntoa(server_address.sin_addr));
	printf("Server spawnado @ %s\n",inet_ntoa(server_address.sin_addr));
	socklenpointer=sizeof(clientAddress);

	mainLoop();
}

