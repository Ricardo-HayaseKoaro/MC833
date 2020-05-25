#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <cjson/cJSON.h>
#include "json_handler.c"
#define MAX 4000 // Size of buffer
#define PORT 8080 
#define SA struct sockaddr

void func(int sockfd) 
{ 
	// Open JSON with movies data and copy to string
	FILE *fp;
	fp=fopen("movies.txt","r");  
	char *json = malloc(sizeof(char)*5000);
	fscanf(fp, "%[^\0]", json);

	//Text file to control id
	FILE *fp_id;
	fp_id = fopen("id.txt","r");
	int id;
	fscanf(fp_id, "%d", &id);

	//Create data structure of all movies
	cJSON *movie_list = NULL;

	//check if json is empty
	if (NULL != fp) {
		fseek (fp, 0, SEEK_END);
		int size = ftell(fp);

		// if is empty create a new array
		if (0 == size){
			movie_list = cJSON_CreateArray();		
		}else
		{
			//Create array of movies from json
			movie_list = cJSON_Parse(json);
		}
	}else
	{
		printf("Error opening file");
		return;
	}

	fclose(fp);
	fclose(fp_id);
	free(json);

	//Start chatting with client
	char buff[MAX]; 
	int n; 
	for (;;) { 
		// Reset buffer
		bzero(buff, MAX); 

		// read the message from client and copy it in buffer 
		read(sockfd, buff, sizeof(buff));
		

		// print buffer which contains the client contents 
		printf("Command from client : %s\n", buff); 
		

		// Register new movie
		if(strncmp("1",buff,1) == 0) {
			bzero(buff, MAX); 
			read(sockfd, buff, sizeof(buff)); 
			char* str_id;
  			asprintf (&str_id, "%i", id); 
			cJSON_AddItemToArray(movie_list, set_id(buff,str_id));
			strcpy(buff,str_id);
			free(str_id);
			id++;
			write(sockfd,buff,sizeof(buff));
			printf("%s", buff);
		}

		// Remove movie
		if(strncmp("2",buff,1) == 0) {
			bzero(buff, MAX); 
			read(sockfd, buff, sizeof(buff)); 
			strcpy(buff,delete_movie(movie_list, buff));
			write(sockfd,buff,sizeof(buff));
			printf("Movie deleted");
		}

		// List all movies and theaters
		if(strncmp("3",buff,1) == 0) {
			bzero(buff, MAX); 
			if(cJSON_GetArraySize(movie_list) == 0){
				strcpy(buff,"Empty");
			}
			else
				strcpy(buff, cJSON_Print(movie_list));
			write(sockfd,buff,sizeof(buff));  
			printf("%s\n",buff);
		}

		// List all movies by genre
		if(strncmp("4",buff,1) == 0) {
			bzero(buff, MAX); 
			read(sockfd, buff, sizeof(buff)); 

			cJSON *movie_list_genre = filter_genre(movie_list, buff);
			if(cJSON_GetArraySize(movie_list_genre) == 0){
				strcpy(buff,"Empty");
			}
			else
				strcpy(buff, cJSON_Print(movie_list_genre));
			write(sockfd,buff,sizeof(buff));  
    		printf("%s\n",buff);
		}

		// Return movie title by ID
		if(strncmp("5",buff,1) == 0) {
			bzero(buff, MAX); 
			read(sockfd, buff, sizeof(buff)); 
			cJSON *movie = find_movie(movie_list, buff);
			if (movie != NULL)
			{
				strcpy(buff, cJSON_Print(movie));
			}
			else
				strcpy(buff, "Invalid ID");
			write(sockfd,buff,sizeof(buff));  
    		printf("%s\n",buff);
		}
		
		// Return all movie info by ID
		if(strncmp("6",buff,4) == 0) {
			bzero(buff, MAX); 
			read(sockfd, buff, sizeof(buff)); 
			cJSON *movie = find_movie(movie_list, buff);
			if (movie != NULL)
			{
				strcpy(buff, cJSON_Print(movie));
			}
			else
				strcpy(buff, "Invalid ID");
			write(sockfd,buff,sizeof(buff));  
    		printf("%s\n",buff);
		}

		// List all movies 
		if(strncmp("7",buff,4) == 0) {
			bzero(buff, MAX); 
			if(cJSON_GetArraySize(movie_list) == 0){
				strcpy(buff,"Empty");
			}
			else
				strcpy(buff, cJSON_Print(movie_list));
			write(sockfd,buff,sizeof(buff));  
			printf("%s\n",buff);
		}

		// Command for exit, close server, save
		if (strncmp("exit", buff, 4) == 0) { 
			printf("Server Exit...\n"); 
			fp=fopen("movies.txt","w"); 
			fprintf(fp, "%s", cJSON_Print(movie_list));
			fclose(fp);

			fp_id=fopen("id.txt","w"); 
			fprintf(fp_id, "%d", id);
			fclose(fp_id);

			cJSON_Delete(movie_list);
			break; 
		} 
	} 
} 

int main() 
{ 
	int listenfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	listenfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (listenfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

		
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(listenfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(listenfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(listenfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n"); 

	// Function for movie data 
	func(connfd); 

	close(listenfd); 
} 
