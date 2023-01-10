#define _GNU_SOURCE
#define PREFIX "movies_"
#define FILE_TYPE ".csv"


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>



typedef struct film{ //movie struct

    char *title;
    char *year;
    char *language;
    char *rating;

    struct film *next; //pointer to next movie

}movie;



int count = 0; //count iterator for processfile function



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



movie* processfile(char *filePath){

    char *line = NULL;

    char *tok;

    size_t length = 0;

    FILE *moviefile = fopen(filePath, "r");

    size_t read;

    movie *head = NULL;
    
    movie *tail = NULL;

    
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


    free(line);

    fclose(moviefile);


    return head;

}



int initprompt(){

    char* prompt = calloc(sizeof(char), 16);

    printf("\n1.Select a file to process\n");

    printf("2.Exit the program\n");

    printf("\nEnter a choice 1 or 2: ");


    scanf("%s", prompt);

    int intprompt = atoi(prompt);

    free(prompt);

    return intprompt;

}



void largeFile(char* pathName, char* largestFile){

    DIR *currrentDir = opendir(pathName); //current directory struct pointer

    struct dirent* pdirent;

    struct stat direntstat;

    off_t largest_file = -1;


    while((pdirent = readdir(currrentDir)) != NULL){

        if((strncmp(PREFIX, pdirent->d_name, strlen(PREFIX)) == 0) && strrchr(pdirent->d_name, '.') != NULL 
        && (strncmp(strrchr(pdirent->d_name, '.'), FILE_TYPE, strlen(FILE_TYPE)) == 0)){ //string comparison for 


        stat(pdirent->d_name, &direntstat);

        if(largest_file < direntstat.st_size){

            largest_file = direntstat.st_size;

            strcpy(largestFile, pdirent->d_name);
        }


    }

}

    closedir(currrentDir);

    return;

}



void smallFile(char* pathName, char* smallestFile){ //finds smallest file

    DIR* currentDir = opendir(pathName); //current directory struct pointer

    struct dirent* pdirent;

    struct stat direntstat;

    off_t smallest_file = -1;


    while((pdirent = readdir(currentDir)) != NULL){

        if((strncmp(PREFIX, pdirent->d_name, strlen(PREFIX)) == 0) && strrchr(pdirent->d_name, '.') != NULL 
        && (strncmp(strrchr(pdirent->d_name, '.'), FILE_TYPE, strlen(FILE_TYPE)) == 0)){

            stat(pdirent->d_name, &direntstat);

            if(smallest_file < 0 || smallest_file > direntstat.st_size){

                smallest_file = direntstat.st_size;

                strcpy(smallestFile, pdirent->d_name);
            }
        }
    }

    closedir(currentDir);

    return;

}



void userFile(char* pathName){

    struct stat direntstat;

    printf("Please enter the complete file name: ");  //for if user decides to input their own file name

    scanf("%s", pathName);


    if(stat(pathName, &direntstat) != 0){

        printf("The file %s was not found. Please try again.\n", pathName); //error handling

        strcpy(pathName, "");
    }

    return;

}



int processPrompt(){

    char* input = calloc(sizeof(char), 16); //calloc space for user input

    printf("While file do you want to process?\n");

    printf("Enter 1 to pick the largest file\n");

    printf("Enter 2 to pick the smallest file\n");

    printf("Enter 3 to specify the name of a file\n");

    printf("\nEnter a choice from 1 to 3: ");

    scanf("%s", input);

    int prompt = atoi(input);

    free(input); //free memory


    return prompt;

}



void processpath(struct movie* list){

    FILE* pfile = NULL;

    DIR* filedir = NULL;

    int randomsufx, d;

    char dirpath[32], filepath[64];

    movie *head = NULL;

    movie *prev = NULL;

    movie *next = NULL;

    movie *currmov = NULL;

    movie *currlist = NULL;

    char curryear[5];

    currmov = list;


    srandom(time(NULL));

    randomsufx = random() % 99999;

    sprintf(dirpath, "murchej.movies.%d", randomsufx);

    d = mkdir(dirpath, 0750);


    while(currmov != NULL){

        currlist = list;

        strcpy(curryear, currmov->year);

        sprintf(filepath, "%s/%s.txt", dirpath, curryear);

        pfile = fopen(filepath, "wa");

        while(currlist != NULL){

            if(strcmp(currlist->year, currmov->year) == 0){

                fprintf(pfile, currlist->title);

                fprintf(pfile, "\n");

            }

            currlist = currlist->next;

        }

        fclose(pfile);

        chmod(filepath, 0640);

        currmov = currmov->next;

    }


    printf("Created directory with name %s\n", dirpath);


}



free_list(movie *list){

    movie *curr;

    while(list != NULL){

        curr = list;

        list = list->next;

        free(curr->title);

        free(curr->year);

        free(curr);

    }
}



void processUser(){

    int valid = 0;

    char pathName[256];

    int prompt;

    while(!valid){

        prompt = processPrompt();

        switch(prompt){

            case 1: //largest file

            largeFile(".", pathName);

            valid = 1;

            break;


            case 2: //smallest file
            
            smallFile(".", pathName);

            valid = 1;

            break;


            case 3: //input file name

            userFile(pathName);

            if(strcmp(pathName, "") != 0){

                valid = 1;

            }

            break;


            default:

            printf("Please enter a number 1-3\n");

            break;

        }


    }

    printf("Now processing the chosen filed named %s\n", pathName);

    struct movie* listofmovies = processfile(pathName);

    processpath(listofmovies);

    free_list(listofmovies); //free memory

}



int main(int argc, char *argv[]){

    int input = 0;

    do{ //do while conditional to keep the program running until user is done.
    
        input = initprompt();

        switch(input){

            case 1: 

                processUser();

                break;


            case 2:

                exit(0); //for if user chooses to exit program


            default:

                printf("\nPlease enter a number 1-2");

                break;

        }

    }while(input != 2);

    return EXIT_SUCCESS;

}