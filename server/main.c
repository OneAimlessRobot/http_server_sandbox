#include "Includes/preprocessor.h"
#define HTTP_OK_HEADER "HTTP/1.1 200 OK\r\n\n"
const char* videoHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: video/%s\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

const char* audioHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: audio/%s\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

const char *manifestHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/manifest+json\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

 const char *imageHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: image/%s\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";

const char *htmlHeader = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: %lu\r\n"
                          "\r\n";


char *get_file_extension(const char *path) {
    char *extension = strrchr(path, '.');

    if (extension != NULL) {
        // Move the pointer to the character after the dot
        return extension + 1;
    }

    // No file extension found
    return NULL;
}

static int makeargv(char *s, char *argv[ARGVMAX]) {
    int ntokens = 0;

    if (s == NULL || argv == NULL || ARGVMAX == 0)
        return -1;
    argv[ntokens] = strtok(s, " \t\n");
    while ((argv[ntokens] != NULL) && (ntokens < ARGVMAX)) {
        ntokens++;
        argv[ntokens] = strtok(NULL, " \t\n");
    }
    argv[ntokens] = NULL; // it must terminate with NULL
    return ntokens;
}


typedef struct page{

	FILE* pagestream;
	char *  pagepath;
	char* data;
	u_int64_t size;
}page;
char* pagesArr[]={"/index2.html",
		 "/index.html",
		"/media.html",
		""};
char* audioExtArr[]={"mp3",
			""};

char* videoExtArr[]={"mp4",
		""};

char* pageExtArr[]={"html",
		""};


char* imageExtArr[]={"jpeg",
		"png",
		"jpg",
		"gif",
		""};

char* manifestExtArr[]={"webmanifest",

	""};


char *args[]={"<numero de clientes>",
		""};


page mainpage={NULL,INDEX_PATH,NULL};

page pages[]={{NULL,INDEX_PATH,NULL},
		{NULL,INDEX2_PATH,NULL},
		{NULL,"",NULL}};


socklen_t socklenpointer;
int server_socket,numOfClients,*client_sockets,max_sd;

struct sockaddr_in server_address, clientAddress;

fd_set readfds;

char* http_header=NULL;
char currResourcePath[LINESIZE];
char peerbuff[PAGE_DATA_SIZE];

char addressContainer[INET_ADDRSTRLEN];

int findInStringArr(char* arr[],char* nullTermStr){

	
	for (int i=0;arr[i][0];i++){
		if(!strcmp(nullTermStr,arr[i])){

			return i;
		}
		
	}
	return -1;

}

int get_string_arr_size(char*args[]){

	int i;
	for (i=0;args[i][0];i++);
	return i+1;

}
void print_string_arr(FILE* fstream,char * args[]){

	for (int i=0;args[i][0];i++){
		
		fprintf(fstream,"%s ",args[i]);

	}

}
void open_page(page* p){
		
		if(!p->pagestream){
		p->pagestream=fopen(p->pagepath,"r");
		fseek(p->pagestream,0,SEEK_END);
		p->size=ftell(p->pagestream);
		fseek(p->pagestream,0,SEEK_SET);
		p->data=malloc(p->size+PAGE_DATA_SIZE+1);
		memset(p->data,0,p->size+PAGE_DATA_SIZE+1);
		printf("%s %p\n",p->pagepath,p->pagestream);
		char buff[1024]={0};
		char headerBuff[1024]={0};
		char * ptr= p->data;
		while(fgets(buff,1024,p->pagestream)){
			
			ptr+=snprintf(ptr,1024,"%s",buff);
		}
		fclose(p->pagestream);
		p->pagestream=NULL;
	
		}

}
void open_video(page* p,char* ext){
		
		if(!p->pagestream){
		p->pagestream=fopen(p->pagepath,"r");
		fseek(p->pagestream,0,SEEK_END);
		p->size=ftell(p->pagestream);
		fseek(p->pagestream,0,SEEK_SET);
		p->data=malloc(PAGE_DATA_SIZE+p->size+1);
		memset(p->data,0,PAGE_RESPONSE_SIZE+p->size+1);
		printf("%s %p\n",p->pagepath,p->pagestream);
		char buff[1024]={0};
		char headerBuff[1024]={0};
		char * ptr= p->data;
		
		snprintf(headerBuff,PAGE_DATA_SIZE,videoHeader,ext,p->size);
		ptr+=snprintf(p->data,PAGE_DATA_SIZE,"%s",headerBuff);
		while(fgets(buff,1024,p->pagestream)){
			
			ptr+=snprintf(ptr,1024,"%s",buff);
		}
		fclose(p->pagestream);
		p->pagestream=NULL;
	
		}

}
void open_audio(page* p,char* ext){
		
		if(!p->pagestream){
		p->pagestream=fopen(p->pagepath,"r");
		fseek(p->pagestream,0,SEEK_END);
		p->size=ftell(p->pagestream);
		fseek(p->pagestream,0,SEEK_SET);
		p->data=malloc(PAGE_DATA_SIZE+p->size+1);
		memset(p->data,0,PAGE_DATA_SIZE+p->size+1);
		printf("%s %p\n",p->pagepath,p->pagestream);
		char buff[1024]={0};
		char headerBuff[1024]={0};
		char * ptr= p->data;
		snprintf(headerBuff,PAGE_DATA_SIZE,audioHeader,ext,p->size);
		ptr+=snprintf(p->data,PAGE_DATA_SIZE,"%s",headerBuff);
		while(fgets(buff,1024,p->pagestream)){
			
			ptr+=snprintf(ptr,1024,"%s",buff);
		}
		fclose(p->pagestream);
		p->pagestream=NULL;
	
		}

}
void open_manifest(page* p){
		
		if(!p->pagestream){
		p->pagestream=fopen(p->pagepath,"r");
		fseek(p->pagestream,0,SEEK_END);
		p->size=ftell(p->pagestream);
		fseek(p->pagestream,0,SEEK_SET);
		p->data=malloc(PAGE_DATA_SIZE+p->size+1);
		memset(p->data,0,PAGE_DATA_SIZE+p->size+1);
		printf("%s %p\n",p->pagepath,p->pagestream);
		char buff[1024]={0};
		char headerBuff[1024]={0};
		char * ptr= p->data;
		snprintf(headerBuff,PAGE_DATA_SIZE,manifestHeader,p->size);
		ptr+=snprintf(p->data,PAGE_DATA_SIZE,"%s",headerBuff);
		while(fgets(buff,1024,p->pagestream)){
			
			ptr+=snprintf(ptr,1024,"%s",buff);
		}
		fclose(p->pagestream);
		p->pagestream=NULL;
	
		}

}
void open_image(page* p,char* ext){
		
		if(!p->pagestream){
		p->pagestream=fopen(p->pagepath,"r");
		fseek(p->pagestream,0,SEEK_END);
		p->size=ftell(p->pagestream);
		fseek(p->pagestream,0,SEEK_SET);
		p->data=malloc(PAGE_DATA_SIZE+p->size+1);
		memset(p->data,0,PAGE_DATA_SIZE+p->size+1);
		printf("%s %p\n",p->pagepath,p->pagestream);
		char buff[1024]={0};
		char headerBuff[1024]={0};
		char * ptr= p->data;
		snprintf(headerBuff,PAGE_DATA_SIZE,imageHeader,ext,p->size);
		ptr+=snprintf(p->data,PAGE_DATA_SIZE,"%s",headerBuff);
		while(fgets(buff,1024,p->pagestream)){
			
			ptr+=snprintf(ptr,1024,"%s",buff);
		}
		fclose(p->pagestream);
		p->pagestream=NULL;

		}

}
void sigint_handler(int signal){
	
	close(server_socket);
	for(int i=0;i<numOfClients;i++){
	if(client_sockets[i]){
		close(client_sockets[i]);
	}
	}
	
	free(mainpage.data);
	free(http_header);
	free(client_sockets);
	exit(-1);

}
void initializeClients(void){

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
void handleIncommingConnections(void){

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
		/*
		int flags= fcntl(client_socket,F_GETFL);
		flags |= O_NONBLOCK;
	        fcntl(client_socket,F_SETFL,flags);
		*/
		if( send(client_socket, HTTP_OK_HEADER, strlen(HTTP_OK_HEADER), 0) != strlen(HTTP_OK_HEADER) )   
            	{
                perror("send");
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
void handleDisconnect(int i,int sd){

    		getpeername(sd , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                    printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_sockets[i] = 0;


}

void sendMediaData(int sd,char* buff){

	char path[LINESIZE]={0};
	char* ptr= path;
	ptr+=snprintf(ptr,LINESIZE,"%s%s",RESOURCES_PATH,buff);
	page p;
	p.data=NULL;
	p.pagestream=NULL;
	p.pagepath=malloc(LINESIZE);
	memset(p.pagepath,0,LINESIZE);
	memset(currResourcePath,0,LINESIZE);
	memcpy(currResourcePath,path,strlen(path));
	memcpy(p.pagepath,currResourcePath,strlen(currResourcePath));
	char* ext=get_file_extension(p.pagepath);
	/*if(findInStringArr(videoExtArr,ext)>=0){
	open_video(&p,ext);
	}*/
	if(findInStringArr(audioExtArr,ext)>=0){
	open_audio(&p,ext);
	}
	else if(findInStringArr(imageExtArr,ext)>=0){
	open_image(&p,ext);
	}
	else if(findInStringArr(manifestExtArr,ext)>=0){
	open_manifest(&p);
	}
	else if(findInStringArr(pageExtArr,ext)>=0){
	open_page(&p);
	}
	//timedsendall(sd,p.data,strlen(p.data));
	if(p.data){
	if(send(sd,p.data,p.size+PAGE_DATA_SIZE,0)!=(p.size+PAGE_DATA_SIZE)){
	
		perror("ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!\n");
	}
	free(p.data);
	
	}
	if(p.pagepath){
	free(p.pagepath);
	}


}

void handleCurrentActivity(int sd){
		
        char* argv[ARGVMAX];
      memset(argv,0,ARGVMAX*sizeof(char*));
	char* pagebuffcopy=malloc(PAGE_DATA_SIZE);
	memset(pagebuffcopy,0,PAGE_DATA_SIZE);
	memcpy(pagebuffcopy,peerbuff,PAGE_DATA_SIZE);
	int argc=makeargv(peerbuff,argv);
	//printf("Conexão de %s\n A enviar cenas\n(isto: %s)\n",inet_ntoa(clientAddress.sin_addr),pagebuffcopy);
	if(!strcmp(argv[0],"GET")){
	if(argc>1){
	
			
		if(!strcmp(argv[1],"/")){
		
			open_page(&mainpage);
			//timedsendall(sd,mainpage.data,strlen(mainpage.data));
		
			if(send(sd,mainpage.data,mainpage.size+PAGE_DATA_SIZE,0)!=(mainpage.size+PAGE_DATA_SIZE)){
	
				perror("ERRO NO SEND!!!! PEDIRAM A ROOT!!!!\n");
			}
		}
		else{
			sendMediaData(sd,argv[1]);
		}
	}
	else{
	
		open_page(&mainpage);
			//timedsendall(sd,mainpage.data,strlen(mainpage.data));
		
			if(send(sd,mainpage.data,mainpage.size+PAGE_DATA_SIZE,0)!=(mainpage.size+PAGE_DATA_SIZE)){
	
				perror("ERRO NO SEND!!!! PEDIRAM A ROOT!!!!\n");
			}
		
	}
	}
	else if(!strcmp(argv[0],"POST")){

	open_page(&pages[1]);
	send(sd,pages[1].data,strlen(pages[1].data),0);
	printf("Conexão de %s\n A enviar cenas\n(isto: %s)\n",inet_ntoa(clientAddress.sin_addr),pages[1].data);
	
	}
	free(pagebuffcopy);	

}

int readsome(int sd,char* buff,int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=MAXTIMEOUTSECS;
                tv.tv_usec=MAXTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return recv(sd,buff,size,0);

                }
                return -2;
}


int readall(int sd,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
	char* ptr= buff;
	char bufftmp[1024]={0};
while((len=read(sd,bufftmp,1024))>0){ /* Watch out for buffer overflow */
        printf("Li!!!!\n");
	ptr+=snprintf(ptr,1024,"%s",bufftmp);
	memset(bufftmp,0,1024);
        total+=len;
	if(total>=size){
	break;
	}
	if(len<1024){
	break;
	}
}
        return total;

}

int timedreadall(int sd,char* buff, int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=MAXTIMEOUTSECS;
                tv.tv_usec=MAXTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return readall(sd,buff,size);

                }
                return -2;



}
void handleCurrentConnections(int i,int sd){
	int numread;
 		if ((numread = readall(sd,peerbuff,PAGE_DATA_SIZE)) == 0)
                {
                    //Somebody disconnected , get his details and prints
                	handleDisconnect(i,sd);
		}

                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                 	handleCurrentActivity(sd);
		}
}

void handleActivityInSockets(void){
	
	for (int i = 0; i < numOfClients; i++)
        {
            int sd = client_sockets[i];
    	
	if (FD_ISSET( sd , &readfds))
            {
		handleCurrentConnections(i,sd);
                
            }

	}

}
void mainLoop(void){

	while(1){
	
	initializeClients();
	handleIncommingConnections();
	handleActivityInSockets();
    }

}

void initializeServer(void){
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

int main(int argc, char ** argv){

	if(argc !=get_string_arr_size(args)){

		fprintf(stderr,"Precisas de mais argumentos:");
		print_string_arr(stderr,args);
		fprintf(stderr,"\n");
		exit(-1);
	}
	numOfClients=atoi(argv[1]);
	client_sockets=malloc(sizeof(int)*numOfClients);
	for(int i=0;i<numOfClients;i++){

		client_sockets[i]=0;
	}
	open_page(&mainpage);
	signal(SIGINT,sigint_handler);
	
	initializeServer();

	return 0;
}

 
   
































/*
int sendsome(int sd,char* buff,int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=MAXTIMEOUTSECS;
                tv.tv_usec=MAXTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return send(sd,buff,size,0);

                }
                return -1;
}


int sendall(int sd,char* buff,int64_t size){
        int64_t len=0;
        int64_t total=0;

for (; total<size;) { // Watch out for buffer overflow
        len=sendsome(sd,buff+total,min(strlen(buff+total),1024));
        printf("Enviamos %ld agora. Faltam %ld de %ld\n",len,size-total-len,size);
	if(len<=0){
	break;
	}
	total+=len;
}
        return total;

}

int timedsendall(int sd,char* buff,int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=MAXTIMEOUTSECS;
                tv.tv_usec=MAXTIMEOUTUSECS;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return sendall(sd,buff,size);

                }
                return -2;



}
*/
