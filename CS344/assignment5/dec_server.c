#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>




void setAddressStruct(struct sockaddr_in* address, int port_num){ //server socket address struct

    memset((char*) address, '\0', sizeof(*address));

    address->sin_family = AF_INET;

    address->sin_port = htons(port_num);

    address->sin_addr.s_addr = INADDR_ANY;

}




void error(const char *msg){ //error message function

    perror(msg);

    exit(1);

}




int modulo(int x, int N){ // https://stackoverflow.com/questions/11720656/modulo-operation-with-negative-numbers  //modulo function I found on stackoverflow

    return (x % N + N) %N;

}




/*char* decryptMsg(char* buff, char* key){

    int len = strlen(buff);

    char* encbuff;

    encbuff = calloc(len, sizeof(char));   

    int i, textchar, keychar;

      
    for(i = 0; i < strlen(buff); ++i){

        char currentbuff = buff[i];

        char currentkey = key[i];
        

        if(currentbuff == ' '){

            textchar = 26;

        }


        else{

            textchar = currentbuff - 'A';

        }


        if(currentkey == ' '){

            keychar = 26;

        }


        else{

            keychar = currentkey - 'A';

        }
       
        int cypherval;

        cypherval = (textchar - keychar);


        if(cypherval < 0){

            cypherval += 27;

        }


        if(cypherval == 26){

            encbuff[i] = ' ';

        }


        else{

            encbuff[i] = (cypherval + 'A'); 

        }

    }

    encbuff[i] = '\0';

    return encbuff;
}*/




int main(int argc, char *argv[]){;

    int connectsocket;

    int writtenChar;

    int readChar;

    int status, i;

    pid_t pid;

    char key_buff[70000];

    char decode_buff[70000];

    char cipher_buff[70000];


    struct sockaddr_in serverAddress, clientAddress;

    socklen_t sizeofClient = sizeof(clientAddress);


    if(argc < 2){ //check usage

        fprintf(stderr,"USAGE: %s port\n", argv[0]);

        exit(1);

    } 



    int listensocket = socket(AF_INET, SOCK_STREAM, 0); //create listening socket


    if(listensocket < 0){

        error("ERROR opening socket");

    }


    setAddressStruct(&serverAddress, atoi(argv[1])); //set server address


  if(bind(listensocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){

        error("ERROR on binding");

    }


  listen(listensocket, 5); //begin listening

  while(1){

    connectsocket = accept(listensocket, (struct sockaddr *)&clientAddress, &sizeofClient); //accept connection


    if(connectsocket < 0){

      error("ERROR on accept");

    }



    pid = fork();

    switch(pid){ //pid switch cases

        case -1:{
            error("Busted. Couldn't fork\n");
        }

        case 0:{

            memset(cipher_buff, '\0', 70000);

    
            readChar = recv(connectsocket, cipher_buff, sizeof(cipher_buff), 0); 


            if(readChar < 0){

                error("ERROR reading from socket");

            }

            readChar = send(connectsocket, "I am the server, and I got your message", 39, 0); //success message

            if(readChar < 0){

                error("ERROR writing to socket");

            }

          
            memset(key_buff, '\0', 70000); //clear buffers
      
            readChar = recv(connectsocket, key_buff, sizeof(key_buff), 0);  //receive key

            if(readChar < 0){

                error("ERROR reading from socket");

            }
       

            for(i = 0; i < strlen(cipher_buff); i++){

                int offset = key_buff[i];

                int ciphertxtChar = cipher_buff[i];

                ciphertxtChar -= 65;

                offset -= 65;

                if(offset == -33){

                    offset = 26;

                }

                if(ciphertxtChar == -33){

                    ciphertxtChar= 26;

                }

                int ciphertxtascii = modulo((ciphertxtChar - offset), 27);

                ciphertxtascii += 65;

                if(ciphertxtascii == 91){

                    ciphertxtascii = 32;

                }

                decode_buff[i] = ciphertxtascii;

                }


            decode_buff[i] = '\0';

            readChar = send(connectsocket, decode_buff, sizeof(decode_buff), 0); // Send decoded cipher

            if(readChar < 0){

                error("ERROR writing to socket");

            }

            exit(0);

        }

        default:{

            pid_t actualpid = waitpid(pid, &status, WNOHANG);

        }

    }

     
    close(connectsocket); //Close the connection

  }

  
  close(listensocket); //end listening

  return 0;

}




