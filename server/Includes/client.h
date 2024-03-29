#ifndef CLIENT_H
#define CLIENT_H
typedef struct client{

	int socket;
	double running_time;
	char username[FIELDSIZE];
	int isAdmin;
	int logged_in;
	struct sockaddr_in client_addr;
}client;

client** initClients(void);

void freeClients(client***clients);
#endif
