#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>




int main(int argc, char* argv[]){

    srand(time(NULL));  //seed the random number gen

    int keylen = atoi(argv[1]);  //turn string into int

    int randn;

    char* key;

    char copy[2];

    key = (char*) malloc(keylen * sizeof(char));


    for(int i = 0; i < keylen; i++){

        randn = rand() % 27 + 65; //random value from 0-26


        if(randn == 91){

            randn = 32;

        }

        copy[0] = randn;

        copy[1] = '\0';

        strcat(key, copy);

    }

    strcat(key, "\n");

    printf("%s", key); //print key

}