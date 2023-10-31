// Include the required Header Files
#include<stdio.h>       // for perror()
#include<stdlib.h>      // for exit()
#include<string.h>      // for bzero()
#include<sys/socket.h>  // for socket()
#include<unistd.h>      // for close()
#include<arpa/inet.h>   // for htonl()
#include<math.h>        // for rand()
#include<pthread.h>     // for pthread_t()

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
Return      :   None
*/
void* chat(void *client) {

    // Declare the required local variables
    char buffer[MAX_LENGTH];
    int messageLength, clientSocketFD;

    // Print the connection success message and extract the client socket file descriptor
    printf("New client connection accepted with ID:%d\n", (int)pthread_self());
    clientSocketFD = *(int*)client;

    // Detach the thread from the master thread
    pthread_detach(pthread_self());

    // Prepare the buffer for storage and start listening to the client socket
    bzero(&buffer, sizeof(buffer));
    while(strcmp(buffer, "exit") != 0) {

        // Try reading data from the client socket and take necessary action
        if((messageLength = read(clientSocketFD, buffer, sizeof(buffer))) < 0) {
            printf("Reading from client %d failed...!!!\n", (int)pthread_self());
            break;
        }

        // Append NULL character at the end of the buffer as delimiter
        buffer[messageLength] = '\0';

        // Check for end of communication and take necessary action
        if(strcmp(buffer, "exit") == 0)
            continue;

        // Generate a random number in the range [0, 11]
        int random = rand() % RANDOM_MAX;

        // Check the generated number to decide whether to respond to this request or not and print appropriate message
        if(random < 4) {
            printf("Dropping packet from client %d...!!!\n", (int)pthread_self());
            continue;
        }

        // Call the function to capitalize the buffer message
        printf("Responding to client %d request: %s\n", (int)pthread_self(), buffer);
        capitalize(buffer);

        // Send the response to the client through the socket
        write(clientSocketFD, buffer, sizeof(buffer));
    }

    // Close the client socket and return
    printf("Closing the client %d connection...!!!\n", (int)pthread_self());
    close(clientSocketFD);
    pthread_exit(NULL);

    return NULL;
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
            perror("Listening from server socket failed...!!!\n");
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

        // Create a thread for the accepted client connection and invoke the function
        pthread_t ptid;
        pthread_create(&ptid, NULL, &chat, (void*) &clientSocketFD);

    }

    // Close the socket and terminate the program
    close(serverSocketFD);
    return 0;
}