#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



typedef struct film{ //movie struct

    char *title;
    char *year;
    char *language;
    char *rating;

    struct film *next; //pointer to next movie

}movie;


int count = 0;

//setting list head and tail pointers to NULL
movie *head = NULL;
movie *tail = NULL;


movie* make_movie(char *line){ //movie tokens

    char *ptr;

    movie *curr = malloc(sizeof(movie));

    char *tok = strtok_r(line, ",", &ptr);

    curr->title = calloc(strlen(tok)+1, sizeof(char));

    strcpy(curr->title, tok);


    tok = strtok_r(NULL, ",", &ptr);

    curr->year = calloc(strlen(tok)+1, sizeof(char));

    strcpy(curr->year, tok);


    tok = strtok_r(NULL, ",", &ptr);

    curr->language = calloc(strlen(tok)+1, sizeof(char));

    strcpy(curr->language, tok);
    

    tok = strtok_r(NULL, ",", &ptr);

    curr->rating = calloc(strlen(tok)+1, sizeof(char));

    strcpy(curr->rating, tok);

    curr->next = NULL;


    return curr;

}



movie* create_structs(char *filePath){

    char *line = NULL;

    char *tok;

    size_t length = 0;

    FILE *moviefile = fopen(filePath, "r");

    size_t read;

    
    read = getline(&line, &length, moviefile);

   while(read = getline(&line, &length, moviefile) != -1)
    {

        movie *new_node = make_movie(line); //adds to linked list

        count++;

        if(head == NULL){

            head = new_node;

            tail = new_node;
        }
            else{

                tail->next = new_node;

                tail = new_node;

                tail->next = NULL;          //sets last node as NULL
            }

    }


     printf("\nProcessed file %s and parsed data for %d movies\n", filePath, count);

    free(line);

    fclose(moviefile);


    return head;

}



void movie_info(movie *oneMovie){ //prints movie info

    printf("%s, %s, %s, %s\n", oneMovie->title, oneMovie->year, oneMovie->language, oneMovie->rating);

}



void movie_list(movie *list){ //prints list of movies

    while(list != NULL){

        movie_info(list);

        list = list->next;
    }
    
}



int check_movie(int year, int was_visited[]){ //checks if a year has already been viewed


    for(int i = 0; i < count + 1; ++i){

        if(was_visited[i] != year){         //compares visited year to current year

            return 0;

        }

        else{

            return 1;

        }
    }
}



void movies_by_year(movie *list){


    printf("\nEnter the year for which you want to see movies: ");

     int movieCounter = 0;

    char buffer[10];        

    fgets(buffer, 5, stdin);

    while(list != NULL){

        if(atoi(buffer) == atoi(list->year)){  //comparing the numbers

            printf("%s\n", list->title);

            movieCounter++;     //struct iterator

        }

        list = list->next;

    }

    if(movieCounter == 0){

        printf("\nNo data about movies released in the year %s\n", buffer);
    }


}



void high_movie_rating(movie *list){

    int nextmovie = 0;

    int was_visited[count+1];

    char *ptr1;

    char *ptr2;

    while(list != NULL){

        movie *curr = head;         //points to first movie in linked list

        if(check_movie(atoi(list->year), was_visited) == 1){

            list = list->next;

            continue;       //moves onto next

        }
            else{

                was_visited[nextmovie] = atoi(list->year);

                nextmovie++;

                ptr1 = NULL;

                double maxcurr = strtod(list->rating, &ptr1);  //double data type for printing rating

                char* ptitle = list->title;

                char* pyear = list->year;

                char* prating = list->rating;


                while(curr != NULL){

                    if(atoi(pyear) == atoi(curr->year)){  //comparing printed year and current year
                        
                        ptr2 = NULL;

                        double currRating = strtod(curr->year, &ptr2);


                        if(currRating >  maxcurr){

                            currRating = maxcurr;

                            curr->title = ptitle;   //setting all attributes to current

                            curr = curr->next;

                        }
                        else{

                            curr = curr->next;
                        }

                    }
                    else{

                        curr = curr->next;
                    }

                }

                printf("%s %.1f %s\n", pyear, maxcurr, ptitle);

                list = list->next; //moves to next movie

            }
    }

}



void movies_by_language(movie* list, char *language){

    int val;

    char *temp;

    char *string;

    bool exist = false;

    printf("\nPlease enter a language: ");
    scanf("%s", language); //scans user input

    
    while(list != NULL){

        string = strstr(list->language, language);


        if(string != NULL){

            temp = calloc(strlen(string) + 1, sizeof(char));

            strcpy(temp, string);

            char *tok = strtok(temp, ";]"); //language token

            val = strcmp(tok, language); //compares token and user input and stores result in val


            if(val == 0){

                exist = true;

                printf("%s %s\n", list->year, list->title);

            }
        }

        list = list->next; //iterates to next

    }

        if(!exist){

            printf("No data about movies released in %s", language); 

        }

}   


int main(int argc, char *argv[]){

    if(argc < 2){

        printf("Please provide the name of the file to process\n");

        return EXIT_FAILURE;

    }

    movie *list = create_structs(argv[1]);

    int user_input = 0;

    char *language = calloc(21, sizeof(char)); //language char for language function

    while(user_input != 4){

        printf("\n 1. Show Movies Released in a Specific Year");

        printf("\n 2. Show Hightest Rating Movie for each Year");

        printf("\n 3. Show the Title and Release of Movies in a Specific Language");

        printf("\n 4. Exit from the Program");


        printf("\n Please enter a choice from 1-4: ");

        scanf("%d", &user_input);

        while(('\n' != getchar()));

        switch(user_input){ //user options

            case 1:

                movies_by_year(list);

            break;


            case 2:

                high_movie_rating(list);

            break;


            case 3:

                movies_by_language(list, language);

            break;


            case 4:

                exit(0);


            default:

                printf("\n Sorry, Please enter a number from 1-4.\n"); //input validation
        }
    }

    return EXIT_SUCCESS;

}
