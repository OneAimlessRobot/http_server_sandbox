#ifndef HTTP_REQ_PARSER_H
#define HTTP_REQ_PARSER_H
typedef enum {GET,POST,INFO,INVALID} http_req_type;

typedef struct http_header_field{

	char* field[2];

}http_header_field;

typedef struct http_header{
	
	http_req_type type;
	char target[FIELDSIZE],
		version[FIELDSIZE],
		host[FIELDSIZE],
		mimetype[FIELDSIZE],
		os[FIELDSIZE];

}http_header;

typedef struct http_request{

	http_header* header;
	u_int64_t data_size;
	char* data;
	


}http_request;

http_header_field* make_http_header_field_arr(char *strarr[ARGVMAX]);

void print_field_arr(FILE* fstream,http_header_field* fields);

http_header* spawnHTTPHeader(char *buff);

http_request* spawnHTTPRequest(char *buff);

void print_http_req_header(FILE* fstream,http_header header);

void print_http_req(FILE* fstream,http_request req);

#endif
