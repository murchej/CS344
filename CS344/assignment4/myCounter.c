#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

#define NUM_THREADS 2

//buffer size

#define SIZE 10

#define MAXCHAR 1000 //max chars for buffer

int flag = 0;

int myCount = 0;

int buffer_size = 0;

int producer_index = 0;

int consumer_index = 0;

char shared_buffer[SIZE][MAXCHAR];


pthread_cond_t myCond1 = PTHREAD_COND_INITIALIZER; //space available

pthread_cond_t myCond2 = PTHREAD_COND_INITIALIZER; //has data

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;  //global initializer



void program_start(){

    printf("\nPROGRAM START");      //first line

}




void exit_program(){

    printf("\nPROGRAM END\n");      //last line

    fflush(stdout);

    exit(0);            //exit program

}




void* transfer_item(char* buffer){  //send item from producer thread to buffer

    strncpy(shared_buffer + producer_index, buffer, strlen(buffer));

    producer_index = strlen(shared_buffer);

    buffer_size += 1;

}


void* producer_thread(){        //producer thread function

    if(myCount == 10){

        flag == 1;
        
    }                  
    

    while(flag != 1){

        if(myCount == 10)
            break;

        pthread_mutex_lock(&myMutex);

        printf("\nPRODUCER: myMutex is locked");


        while(buffer_size == SIZE)
            pthread_cond_wait(&myCond1, &myMutex);

        printf("\nPRODUCER: waiting on myCond1"); //waiting for room

        

        char *item = fgets(shared_buffer, SIZE, stdin); //char item pointer from stdin

        transfer_item(item); //send to buffer
        
        pthread_mutex_unlock(&myMutex);

        printf("\nPRODUCER: myMutex is unlocked");

        pthread_cond_signal(&myCond2); //has data

        printf("\nPRODUCER: signaling myCond2");

    }

            return NULL;

}



char* receive_item(){  //function for consumer to take item from buffer

    char* item = shared_buffer[consumer_index];

    consumer_index += 1;

    return item;

}




void *consumer_thread(void *args){ //consumer thread function

    char *item; //char to set equal to buffer value

    while(flag != 1){

        pthread_mutex_lock(&myMutex);

        printf("\nCONSUMER: myMutex is locked");


        while(buffer_size == 0) //wait for data
            pthread_cond_wait(&myCond2, &myMutex);

            printf("\nCONSUMER: waiting on myCond2"); //wait for data


            item = receive_item();

            myCount += 1;    //iterate myCount

            int oldcount = myCount - 1;


            printf("\nmyCount: %d -> %d", oldcount, myCount); //counter statement


            pthread_mutex_unlock(&myMutex);

            printf("\nCONSUMER: myMutex is unlocked");

            if(myCount == 10){ //myCount check

            flag == 1;  //kills the consumer loop, ends with the mutex being unlocked

            exit_program();

        }

            pthread_cond_signal(&myCond1);

            printf("\nCONSUMER: signaling myCond1"); //there is space

    }

        return NULL;    //consumer returns null

}




int main(int argc, char* argv[]){

    int tindex, result_code;        //iteration variables

    pthread_t threads[NUM_THREADS]; //create threads array for looping


    pthread_create(&threads[0], NULL, producer_thread, NULL);

    program_start();

    printf("\nCONSUMER THREAD CREATED"); //created consumer thread

    pthread_create(&threads[1], NULL, consumer_thread, NULL);

    assert(0 == result_code);
        

    for(tindex = 0; tindex < NUM_THREADS; ++tindex){ //join threads

        result_code = pthread_join(threads[tindex], NULL);

        assert(0 == result_code);

    }



    exit_program(); //end program


    
    return EXIT_SUCCESS;

}