#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>  
#include <sys/socket.h>



void setAddressStruct(struct sockaddr_in* address, int port_num, char* hostname){

    memset((char*) address, '\0', sizeof(*address)); //clear address struct

    address->sin_family = AF_INET;

    address->sin_port = htons(port_num);


    struct hostent* hostinfo = gethostbyname(hostname);


    if(hostinfo == NULL){

        fprintf(stderr, "CLIENT: ERROR, no such host\n");

        exit(0);

    }


    memcpy((char*) &address->sin_addr.s_addr, hostinfo->h_addr_list[0], hostinfo->h_length); //copy the first ip to sin_addr.s_addr

}




void error(const char *msg){ //error message function

    perror(msg);

    exit(2);

}




int main(int argc, char *argv[]){

    struct sockaddr_in serverAddress;

    char key_buff[70000];

    char decode_buff[70000];

    char cipher_buff[70000];

    char response_buff[1000];

    int portnum;

    int socketFD;

    int writtenChar;

    int readChar;

    int i;



    if(argc < 4){  //check usage

        fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); 

        exit(0); 
    } 


  FILE *fp;

  fp = fopen(argv[1], "r"); //get cipher from file

  if(fp == NULL){

      perror("Couldn't open the file. Check entry.\n");

      exit(1);

    }

  
  fgets(cipher_buff, sizeof(cipher_buff) - 1, fp);

  fclose(fp); //close file

  cipher_buff[strcspn(cipher_buff, "\n")] = '\0'; 


  fp = fopen(argv[2], "r");

   if(fp == NULL){

      perror("Couldn't open the file. Check entry.\n");

      exit(1);

    }


  fgets(key_buff, sizeof(key_buff) - 1, fp);

  fclose(fp);

  key_buff[strcspn(key_buff, "\n")] = '\0'; 


  for(i = 0; i < strlen(cipher_buff); i++){ //check cipher valid

      if(cipher_buff[i] < 65 || cipher_buff[i] > 90){

          if(cipher_buff[i] != 32){

              fprintf(stderr, "Bad character(s) in ciphertext. Exiting\n");

              exit(1);

            }
        }
    }


  if(strlen(key_buff) < strlen(cipher_buff)){

      fprintf(stderr, "Key is shorter than plaintext. Exiting\n");

      exit(1);

    }


    for(i = 0; i < strlen(key_buff); i++){ //check key valid

      if(key_buff[i] < 65 || key_buff[i] > 90){

          if(key_buff[i] != 32){

              fprintf(stderr, "Bad character(s) in key. Exiting\n");

              exit(1);

            }
        }
    }



  socketFD = socket(AF_INET, SOCK_STREAM, 0); //create socket

  if (socketFD < 0){

    error("CLIENT: ERROR opening socket");

  }


  setAddressStruct(&serverAddress, atoi(argv[3]), "localhost"); //create server address


  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){

    error("CLIENT: ERROR connecting");

  }


  writtenChar = send(socketFD, cipher_buff, sizeof(cipher_buff), 0); //send cipher

  if (writtenChar < 0){

    error("CLIENT: ERROR writing to socket");

  }

  if(writtenChar < strlen(cipher_buff)){

    printf("CLIENT: WARNING: Not all data written to socket!\n");

  }

  memset(decode_buff, '\0', sizeof(decode_buff)); //clerar buffers


  readChar = recv(socketFD, response_buff, sizeof(response_buff) - 1, 0); 

  if(readChar < 0){

    error("CLIENT: ERROR reading from socket");

  }



  writtenChar = send(socketFD, key_buff, sizeof(key_buff), 0); //send key to socket

  if (writtenChar < 0){

    error("CLIENT: ERROR writing to socket");

  }

  if (writtenChar < strlen(key_buff)){

    printf("CLIENT: WARNING: Not all data written to socket!\n");

  }

  
  memset(key_buff, '\0', sizeof(key_buff));

 
  readChar = recv(socketFD, decode_buff, sizeof(decode_buff) - 1, 0); 

  if(readChar < 0){

    error("CLIENT: ERROR reading from socket");

  }

  
  printf("%s\n", decode_buff); //decrypted message

  memset(decode_buff, '\0', sizeof(decode_buff));


  close(socketFD); //close socket

  return 0;


}