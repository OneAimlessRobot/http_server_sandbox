#include "Includes/preprocessor.h"
#include "Includes/auxFuncs.h"
#include "Includes/server_innards.h"

int main(int argc, char ** argv){

	if(argc !=get_string_arr_size(argv)){

		fprintf(stderr,"Precisas de mais argumentos:");
		print_string_arr(stderr,argv);
		fprintf(stderr,"\n");
		exit(-1);
	}
	
	
	initializeServer(atoi(argv[1]));

	return 0;
}
