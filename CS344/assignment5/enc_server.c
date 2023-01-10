#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>




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



/*char* encryptMsg(char* buff, char* key){

    int len = strlen(buff);

    char* encbuff;

    encbuff = calloc(len, sizeof(char));  

    int textchar, keychar, i;   
    
      
    
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

        cypherval = (textchar + keychar) % 27;   


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




int main(int argc, char *argv[]){

    struct sockaddr_in serverAddress, clientAddress;

    socklen_t sizeofClient = sizeof(clientAddress);

    int connectSocket;

    int readChar;

    int status;

    int writtenChar, i;

    char plaintxt_buff[70000];

    char key_buff[70000];

    char cipher_buff[70000];

    pid_t pid;

  
  if(argc < 2){ //usage check

    fprintf(stderr,"USAGE: %s port\n", argv[0]); 

    exit(1);

  } 
  

  int listensocket = socket(AF_INET, SOCK_STREAM, 0); //create listening socket


  if(listensocket < 0){

    error("ERROR opening socket");

  }


  setAddressStruct(&serverAddress, atoi(argv[1]));


  if(bind(listensocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){

        error("ERROR on binding");

    }


  listen(listensocket, 5);  //begin listening
  

  while(1){

    connectSocket = accept(listensocket, (struct sockaddr *)&clientAddress, &sizeofClient); //accept connection


    if(connectSocket < 0){

      error("ERROR on accept");

    }

    pid = fork();

    switch(pid){ //pid switch case

        case -1:{

            error("Couldn't fork\n");
        }

        case 0:{

            memset(plaintxt_buff, '\0', 70000);
            
            readChar = recv(connectSocket, plaintxt_buff, sizeof(plaintxt_buff), 0); 

            if(readChar < 0){

                error("ERROR reading from socket");

            }


            readChar = send(connectSocket, "I am the server, and I got your message", 39, 0); //success message

            if(readChar < 0){

                error("ERROR writing to socket");

            }

            
            memset(key_buff, '\0', 70000);// Get the message 

           
            readChar = recv(connectSocket, key_buff, sizeof(key_buff), 0); //receive key

            if (readChar < 0){

                error("ERROR reading from socket");

            }

            for(i = 0; i < strlen(plaintxt_buff); i++){

                int offset = key_buff[i];

                int plaintxt_char = plaintxt_buff[i];

                plaintxt_char -= 65;

                offset -= 65;

                if(offset == -33){

                    offset = 26;

                }

                if(plaintxt_char == -33){

                    plaintxt_char = 26;

                }

                int ciphertext_ascii = (plaintxt_char + offset) % 27;

                ciphertext_ascii += 65;

                if(ciphertext_ascii == 91){

                    ciphertext_ascii = 32;

                }

                cipher_buff[i] = ciphertext_ascii;

                }


            cipher_buff[i] = '\0';

            readChar = send(connectSocket, cipher_buff, sizeof(cipher_buff), 0); //send cipher to socket
           

            if (readChar < 0){

                error("ERROR writing to socket");

            }

            exit(0);
        }

        default:{

            pid_t actualpid = waitpid(pid, &status, WNOHANG);

        }

    }
    
    close(connectSocket); //Close the connection 

  }

  
  close(listensocket); //Close the listening socket

  return 0;



}