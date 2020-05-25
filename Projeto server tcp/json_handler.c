#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <cjson/cJSON.h>

//Return movie by ID
cJSON *find_movie(cJSON *movie_list, char *id_movie){
    cJSON *movie = NULL;
    cJSON_ArrayForEach(movie, movie_list)
    {
        cJSON *id = cJSON_GetObjectItem(movie,"id");
        if (strcmp(id->valuestring,id_movie)==0){
            return movie;
        }
    }
    return NULL;
}

//Delete movie
char *delete_movie(cJSON *movie_list, char *id_movie){
    cJSON *movie = NULL;
    int index=0;
    cJSON_ArrayForEach(movie, movie_list)
    {
        cJSON *id = cJSON_GetObjectItem(movie,"id");
        if (strcmp(id->valuestring,id_movie)==0){
            cJSON_DeleteItemFromArray(movie_list,index);
            return "Movie deleted";
        }
        index++;
    }
    return "Invalid ID";
}


//Return list of all movies filtered by genre
cJSON *filter_genre(cJSON *movie_list, char *find_genre){

    cJSON *list_genre = cJSON_CreateArray();
    cJSON *movie = NULL;
    cJSON_ArrayForEach(movie, movie_list)
    {
        cJSON *genre = cJSON_GetObjectItem(movie,"genre");
        if (strcmp(genre->valuestring,find_genre)==0){
            cJSON_AddItemToArray(list_genre,movie);
        }
    }
    return list_genre;
}

//Return cJSON of a json string and set id
cJSON *set_id(const char * const movie, char* id)
{
    cJSON *movie_json = cJSON_Parse(movie);  
    cJSON_AddStringToObject(movie_json, "id", id);

    return movie_json;
}




