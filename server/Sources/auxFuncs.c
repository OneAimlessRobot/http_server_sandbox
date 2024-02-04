#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"

double genRanddouble(double min, double max){

    if (max < min) {
        fprintf(stderr, "Error: max must be greater than or equal to min\n");
        return 0.0f;
    }

    // Generate a random double between 0 and 1
    double random = ((double)rand() / RAND_MAX);

    // Scale and shift the random value to fit within the desired range
    return (random * (max - min)) + min;

}

int genRandInt(int min, int max) {
    if (max < min) {
        fprintf(stderr, "Error: max must be greater than or equal to min\n");
        return 0;
        }

    // Generate a random integer between min and max
    return (rand() % (max - min + 1)) + min;
}


char* randStr(int size){

        char* result= malloc(size+1);
        memset(result,0,size+1);
        for(int i=0;i<size;i++){

                result[i]=(char)genRandInt((int)97,(int)112);

        }
	result[size]=0;
        return result;



}

char** randStrArr(int sizeOfStrs,int size){

	char** result= malloc(sizeof(char*)*size);

	for(int i=0;i<size;i++){

		result[i]=randStr(sizeOfStrs);

	}

	return result;


}

void freeStrArr(char** arr,int size){
	if(!arr){

		return;

	}
	
	for(int i=0;i<size;i++){
		if(arr[i]){
		free(arr[i]);
		}
	}

	free(arr);


}

int* getRandIntArr(int min,int max,int size){

	int* result= malloc(sizeof(int)*size);
	for(int i=0;i<size;i++){

		result[i]=genRandInt(min,max);
	}
	return result;

}

void swap(void** var1, void** var2){

void* tmp=*var1;
*var1=*var2;
*var2=tmp;




}


int64_t min(int64_t arg1, int64_t arg2){

	if(arg1<arg2){

		return arg1;

	}
	return arg2;


}
char *get_file_extension(const char *path) {
    char *extension = strrchr(path, '.');

    if (extension != NULL) {
        // Move the pointer to the character after the dot
        return extension + 1;
    }

    // No file extension found
    return NULL;
}

int makeargv(char *s, char *argv[ARGVMAX]) {
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
