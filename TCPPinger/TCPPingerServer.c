// Include the required Header Files
#include<stdio.h>       // for perror()
#include<stdlib.h>      // for exit()
#include<string.h>      // for bzero()
#include<sys/socket.h>  // for socket()
#include<unistd.h>      // for close()
#include<arpa/inet.h>   // for htonl()
#include<math.h>        // for rand()

// Declare the required MACROS
#define MAX_LENGTH 1024
#define RANDOM_MAX 12
#define PORT 12000

/*
Name        :   capitalize()
Description :   Converts all the lowercase letters of the buffer string to uppercase
Arguments   :   The buffer array
Return      :   None
*/
void capitalize(char buffer[]) {

    // Declare the required local variables
    int i = 0;

    // Iterate through buffer string characters, check for lowercase and convert it to uppercase, if true
    while(buffer[i]) {
        if(buffer[i]>='a' && buffer[i]<='z')
            buffer[i] = buffer[i] - 32;
        
        i++;
    }

    // Return control to the calling function
    return;
}

/*
Name        :   chat()
Description :   Keeps listening to the client socket for requests and responds to them
Arguments   :   The client socket descriptor
Return      :   0,  on successful communication with the client
                -1, otherwise
*/
int chat(int clientSocketFD) {

    // Declare the required local variables
    char buffer[MAX_LENGTH];
    int messageLength;

    // Prepare the buffer for storage and start listening to the client socket
    bzero(&buffer, sizeof(buffer));
    while(strcmp(buffer, "exit") != 0) {

        // Try reading data from the client socket and take necessary action
        if((messageLength = read(clientSocketFD, buffer, sizeof(buffer))) < 0)
            return -1;

        // Append NULL character at the end of the buffer as delimiter
        buffer[messageLength] = '\0';

        // Check for end of communication and take necessary action
        if(strcmp(buffer, "exit") == 0)
            continue;

        // Generate a random number in the range [0, 11]
        int random = rand() % RANDOM_MAX;

        // Check the generated number to decide whether to respond to this request or not and print appropriate message
        if(random < 4) {
            printf("Dropping packet...!!!\n");
            continue;
        }

        // Call the function to capitalize the buffer message
        printf("Responding to client request: %s\n", buffer);
        capitalize(buffer);

        // Send the response to the client through the socket
        write(clientSocketFD, buffer, sizeof(buffer));
    }
    return 0;
}

/*
Name        :   main()
Description :   The driver code that calls the other subroutines
Arguments   :   None
Return      :   Integer 0
*/
int main() {

    // Declare the required local variables
    int serverSocketFD, clientSocketFD, clientLength;
    struct sockaddr_in serverAddress, clientAddress;
    
    // Try creating a server socket and print appropriate message
    bzero(&serverAddress, sizeof(serverAddress));
    if((serverSocketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed...!!!\n");
        exit(0);
    }
    printf("Socket creation successful...!!!\n");

    // Prepare the server address details for binding
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_family = AF_INET;
    
    // Try binding the server address to the socket and print appropriate message
    if(bind(serverSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Socket binding failed...!!!\n");
        close(serverSocketFD);
        exit(0);
    }
    printf("Socket binding successful...!!!\n");

    // Start listening to the socket
    while(1) {

        // Read data from the socket and print appropriate message
        if(listen(serverSocketFD, 5) < 0) {
            perror("Listening from server client failed...!!!\n");
            close(serverSocketFD);
            exit(0);
        }

        // Try to accept the client socket connection and print appropriate message
        clientLength = sizeof(clientAddress);
        if((clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientLength)) < 0) {
            perror("Connection accept failed...!!!\n");
            close(serverSocketFD);
            exit(0);
        }
        printf("Socket successfully accepted...!!!\n");

        // Start listening to the client socket
        if(chat(clientSocketFD) < 0)
            perror("Communication failed...!!!\n");

        // Close the client socket
        printf("Closing the client connection...!!!\n");
        close(clientSocketFD);
    }

    // Close the socket and terminate the program
    close(serverSocketFD);
    return 0;
}