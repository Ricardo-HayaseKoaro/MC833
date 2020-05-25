#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <cjson/cJSON.h>
#define MAX 4000
#define PORT 8080 
#define SA struct sockaddr 

//Return cJSON movie
cJSON *create_movie(void)
{
	char title[256];
    printf("Title: ");
    scanf(" %[^\n]s",title);


	char genre[256];
    printf("Genre: ");
	scanf(" %[^\n]s",genre);


	char synopsis[1024];
    printf("Synopsis: ");
    scanf(" %[^\n]s",synopsis);


    char str_n_rooms[3];
	int n_rooms;
    printf("Number of rooms: ");
	scanf("%d%*c",&n_rooms);

	//Create object movie
	cJSON *movie = cJSON_CreateObject();

	cJSON_AddStringToObject(movie, "title", title);
    cJSON_AddStringToObject(movie, "genre", genre);
    cJSON_AddStringToObject(movie, "synopsis", synopsis);

	cJSON *theater = NULL;
    theater = cJSON_AddArrayToObject(movie, "theaters");

	for (size_t i = 0; i < n_rooms; i++)
	{
		int theater_room;
        printf("Room number: ");
        scanf("%d%*c", &theater_room);
		cJSON *room = cJSON_CreateObject();
        cJSON_AddNumberToObject(room, "room", theater_room);
        cJSON_AddItemToArray(theater,room);
	}

    return movie;
}

//Print all movies and their theaters
void print_theaters(cJSON *movies)
{
	cJSON *movie = NULL;
	printf("-------------------------------------------------\n");	
	cJSON_ArrayForEach(movie, movies)
    {
        cJSON *title = cJSON_GetObjectItem(movie,"title");
		printf("%s\n", title->valuestring);
		cJSON *theaters = cJSON_GetObjectItem(movie,"theaters");
		cJSON *room = NULL;
		printf("Rooms: ");
		cJSON_ArrayForEach(room, theaters)
    	{
			cJSON *room_id = cJSON_GetObjectItem(room,"room");
			printf("%d ",room_id->valueint);
		}
		printf("\n");
		printf("-------------------------------------------------\n");
    }
}

//Print all movies
void print_movies(cJSON *movies)
{
	cJSON *movie = NULL;
	printf("-------------------------------------------------\n");	
	cJSON_ArrayForEach(movie, movies)
    {
		cJSON *id = cJSON_GetObjectItem(movie,"id");
        cJSON *title = cJSON_GetObjectItem(movie,"title");
		cJSON *genre = cJSON_GetObjectItem(movie,"genre");
        cJSON *synopsis = cJSON_GetObjectItem(movie,"synopsis");
		printf("ID: %s\n", id->valuestring);
		printf("Title: %s\n", title->valuestring);
		printf("Genre: %s\n", genre->valuestring);
		printf("Synopsis: %s\n", synopsis->valuestring);
		cJSON *theaters = cJSON_GetObjectItem(movie,"theaters");
		cJSON *room = NULL;
		printf("Rooms: ");
		cJSON_ArrayForEach(room, theaters)
    	{
			cJSON *room_id = cJSON_GetObjectItem(room,"room");
			printf("%d ",room_id->valueint);
		}
		printf("\n");
		printf("-------------------------------------------------\n");
    }
}

//Print movie
void print_movie(cJSON *movie)
{
	printf("-------------------------------------------------\n");	
	cJSON *id = cJSON_GetObjectItem(movie,"id");
    cJSON *title = cJSON_GetObjectItem(movie,"title");
	cJSON *genre = cJSON_GetObjectItem(movie,"genre");
    cJSON *synopsis = cJSON_GetObjectItem(movie,"synopsis");
	printf("ID: %s\n", id->valuestring);
	printf("Title: %s\n", title->valuestring);
	printf("Genre: %s\n", genre->valuestring);
	printf("Synopsis: %s\n", synopsis->valuestring);
	cJSON *theaters = cJSON_GetObjectItem(movie,"theaters");
	cJSON *room = NULL;
	printf("Rooms: ");
	cJSON_ArrayForEach(room, theaters)
    {
		cJSON *room_id = cJSON_GetObjectItem(room,"room");
		printf("%d ",room_id->valueint);
	}
	printf("\n");
}

//Print movie title
void print_title(cJSON *movie)
{
	printf("-------------------------------------------------\n");	
	cJSON *id = cJSON_GetObjectItem(movie,"id");
    cJSON *title = cJSON_GetObjectItem(movie,"title");
	printf("ID: %s\n", id->valuestring);
	printf("Title: %s\n", title->valuestring);
}

void func(int sockfd) 
{  
	char buff[MAX]; 
	int n; 
	printf("1. Register new movie\n");
	printf("2. Remove movie\n");
	printf("3. List all movies (Theater rooms)\n");
	printf("4. List all movies by genre\n");
	printf("5. Return movie title by ID\n");
	printf("6. Return movie info by ID\n");
	printf("7. List all movies (all infos)\n");

	for (;;) { 
		bzero(buff, sizeof(buff)); 

		printf("Type a command: "); 
		scanf(" %[^\n]s",buff);

		// Send command to server
		write(sockfd, buff, sizeof(buff)); 

		//Commands
		//New movie
		if ((strncmp(buff, "1", 1)) == 0){
			bzero(buff, sizeof(buff)); 
			cJSON *new = create_movie();
			strcpy(buff, cJSON_Print(new));
			cJSON_Delete(new);
			write(sockfd, buff, sizeof(buff)); 
			read(sockfd,buff,sizeof(buff));
			printf("Movie %s registered\n", buff);		
		}// Remove by id
		else if ((strncmp(buff, "2", 1)) == 0){
			bzero(buff, sizeof(buff)); 
			printf("Remove movie id: ");
			scanf(" %[^\n]s",buff);
			write(sockfd, buff, sizeof(buff)); 
			read(sockfd,buff,sizeof(buff));
			printf("%s\n", buff);		
		}// List all movies (theaters)
		else if ((strncmp(buff, "3", 1)) == 0){
			read(sockfd,buff,sizeof(buff));
			if((strcmp(buff,"Empty")) == 0)
			{
				printf("%s\n",buff);
			}
			else
			{
				cJSON *movies = cJSON_Parse(buff);
				print_theaters(movies);
			}
		}// Filter by genre
		else if ((strncmp(buff, "4", 1)) == 0){
			bzero(buff, sizeof(buff)); 
			printf("Filter by genre: ");
			scanf(" %[^\n]s",buff);
			write(sockfd, buff, sizeof(buff)); 
			read(sockfd,buff,sizeof(buff));
			if((strcmp(buff,"Empty")) == 0)
			{
				printf("%s\n",buff);
			}
			else
			{
				cJSON *movies = cJSON_Parse(buff);
				print_movies(movies);
			}
		}// Return movie title
		else if ((strncmp(buff, "5", 1)) == 0){
			bzero(buff, sizeof(buff)); 
			printf("Movie id: ");
			scanf(" %[^\n]s",buff);
			write(sockfd, buff, sizeof(buff)); 
			read(sockfd,buff,sizeof(buff));
			if((strcmp(buff,"Invalid ID")) == 0)
			{
				printf("%s\n",buff);
			}
			else
			{
				cJSON *movie = cJSON_Parse(buff);
				print_title(movie);
			}
		}
		// Return movie info
		else if ((strncmp(buff, "6", 1)) == 0){
			bzero(buff, sizeof(buff)); 
			printf("Movie id: ");
			scanf(" %[^\n]s",buff);
			write(sockfd, buff, sizeof(buff)); 
			read(sockfd,buff,sizeof(buff));
			if((strncmp(buff,"Invalid ID",10)) == 0)
			{
				printf("%s\n",buff);
			}
			else
			{
				cJSON *movie = cJSON_Parse(buff);
				print_movie(movie);
			}
		}// Return all movies
		else if ((strncmp(buff, "7", 1)) == 0){
			read(sockfd,buff,sizeof(buff));
			if((strncmp(buff,"Empty",4)) == 0)
			{
				printf("%s\n",buff);
			}
			else
			{
				cJSON *movies = cJSON_Parse(buff);
				print_movies(movies);
			}
		}
		else if ((strncmp(buff, "exit", 4)) == 0) { 
			break; 
		} 
	} 
} 

int main() 
{ 
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 

	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server..\n"); 

	// function for chat 
	func(sockfd); 

	// close the socket 
	close(sockfd); 
} 
