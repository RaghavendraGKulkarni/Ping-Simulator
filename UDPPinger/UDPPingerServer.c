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
Name        :   main()
Description :   The driver code that calls the other subroutines
Arguments   :   None
Return      :   Integer 0
*/
int main() {

    // Declare the required local variables
    char buffer[MAX_LENGTH];
    int socketFD, clientLength, messageLength;
    struct sockaddr_in serverAddress, clientAddress;
    
    // Try creating a Server Socket and print appropriate message
    bzero(&serverAddress, sizeof(serverAddress));
    if((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed.\n");
        exit(0);
    }
    printf("Socket creation successful...!!!\n");
    
    // Prepare the server address details for binding
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_family = AF_INET;
    
    // Try binding the server address to the Socket and print appropriate message
    if(bind(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Socket binding failed.\n");
        close(socketFD);
        exit(0);
    }
    printf("Socket binding successful...!!!\n");
    
    // Start listening to the socket
    clientLength = sizeof(clientAddress);
    while(1) {

        // Read data from the socket and print appropriate message
        if((messageLength = recvfrom(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, &clientLength)) < 0) {
            perror("Listening from client failed.\n");
            close(socketFD);
            exit(0);
        }

        // Append NULL character at the end of the buffer as delimiter
        buffer[messageLength] = '\0';

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
        sendto(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));
    }

    // Close the socket and terminate the program
    close(socketFD);
    return 0;
}