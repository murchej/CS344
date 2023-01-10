#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netdb.h> 




void setAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname){

    memset((char*) address, '\0', sizeof(*address)); //clear address struct

    address->sin_family = AF_INET;

    address->sin_port = htons(portNumber);


    struct hostent* hostInfo = gethostbyname(hostname);


    if(hostInfo == NULL){

        fprintf(stderr, "CLIENT: ERROR, no such host\n");

        exit(0);

    }


    memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length); //copy the first ip to sin_addr.s_addr

}




void errormsg(const char *msg){ //error message function

    perror(msg);

    exit(2);

}




int main(int argc, char *argv[]){

    struct sockaddr_in serverAddress;

    char key_buff[70000];

    char plaintxt_buff[70000];

    char cipher_buff[70000];

    int portnum;

    int socketfd;

    int writtenChars;

    int readChar, i;

 


    if(argc < 4){ //usage and argument check

        fprintf(stderr, "USAGE: %s plaintext key port\n", argv[0]);

        exit(0);

    }


    FILE *fp; //get text from file

    fp = fopen(argv[1], "r");


    if(fp == NULL){

        perror("Could not open the file. Please check entry.\n");

        exit(1);

    }


    fgets(plaintxt_buff, sizeof(plaintxt_buff) - 1, fp);

    fclose(fp); //close file


    plaintxt_buff[strcspn(plaintxt_buff, "\n")] = '\0';


    fp = fopen(argv[2], "r"); //get key from the file


    if(fp == NULL){

        perror("Could not open the file. Please check entry");

        exit(1);

    }

    fgets(key_buff, sizeof(key_buff) - 1, fp);

    fclose(fp);


    key_buff[strcspn(key_buff, "\n")] = '\0'; //remove \n


    for(i = 0; i < strlen(plaintxt_buff); i++){ //check if plaintext is valid

        if(plaintxt_buff[i] < 65 || plaintxt_buff[i] > 90){

            if(plaintxt_buff[i] != 32){

                fprintf(stderr, "Bad character(s) in plaintext. Exiting now\n");

                exit(1);

            }

        }

    }


    if(strlen(key_buff) < strlen(plaintxt_buff)){ //check if key is valid

      fprintf(stderr, "Key is shorter than plaintext. Exiting\n");

      exit(1);

    }


    for(i = 0; i < strlen(key_buff); i++){

      if(key_buff[i] < 65 || key_buff[i] > 90){

          if(key_buff[i] != 32){

              fprintf(stderr, "Bad character(s) in key. Exiting now\n");

              exit(1);

            }

        }

    }


    socketfd = socket(AF_INET, SOCK_STREAM, 0); //create socket

    
    if(socketfd < 0){

        errormsg("CLIENT: ERROR opening the socket");

    }


    setAddressStruct(&serverAddress, atoi(argv[3]), "localhost"); //set up the server address


    if(connect(socketfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){

        errormsg("CLIENT: ERROR with connection");

    }


    writtenChars = send(socketfd, plaintxt_buff, sizeof(plaintxt_buff), 0); //send plain text


    if(writtenChars < 0){

        errormsg("CLIENT: ERROR writing to the socket");

    }


    if(writtenChars < strlen(plaintxt_buff)){

        printf("CLIENT: WARNING! Not all the data has been written to the socket");

    }


    memset(plaintxt_buff, '\0', sizeof(plaintxt_buff));


    readChar = recv(socketfd, plaintxt_buff, sizeof(plaintxt_buff) - 1, 0); //read data from the socket


    if(readChar < 0){

        errormsg("CLIENT: ERROR reading from the socket");

    }


    writtenChars = send(socketfd, key_buff, strlen(key_buff), 0); //send the key to the server


    if(writtenChars < 0){

        errormsg("CLIENT: ERROR writing to the socket");

    }


    if(writtenChars < strlen(key_buff)){

        printf("CLIENT: WARNING! Not all the data has been written to the socket");

    }


    memset(key_buff, '\0', sizeof(key_buff)); //clear buffers


    readChar = recv(socketfd, cipher_buff, sizeof(plaintxt_buff) - 1, 0); //get return message


    if(readChar < 0){

        errormsg("CLIENT: ERROR reading from the socket");

    }


    printf("%s\n", cipher_buff); //print out the encoded message


    memset(cipher_buff, '\0', sizeof(cipher_buff));


    close(socketfd); //close socket


    return 0;

}