#include "../Includes/buffSizes.h"
#include <arpa/inet.h>
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
int logging,beeping;

client* clientToDrop;
client* clients;
char currDir[PATHSIZE];
