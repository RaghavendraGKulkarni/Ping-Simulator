// Include the required Header Files
#include<stdio.h>       // for sprintf() and perror()
#include<stdlib.h>      // for exit()
#include<string.h>      // for string handling functions
#include<netinet/in.h>  // for htons()
#include<sys/socket.h>  // for socket()
#include<unistd.h>      // for close()
#include<arpa/inet.h>   // for inet_addr()
#include<sys/time.h>    // for struct timeval
#include<time.h>        // for time_t

// Declare the required MACROS
#define MAX_LENGTH 1024
#define PORT 12000

/*
Name        :   formatPingMessage()
Description :   Prepares the ping message in the required format and stores it in the buffer
Arguments   :   The buffer array, the Ping Number and the Time Stamp of the Ping in that order
Return      :   None
*/
void formatPingMessage(char buffer[], int pingNumber) {
    
    // Declare the required local variables
    char temp[30];
    struct tm* timeStamp;
    time_t t;

    // Copy the ping number in the buffer
    sprintf(buffer, "ping %d ", pingNumber);

    //Get the Time Stamp and copy it into a temporary buffer according to the required format
    time(&t);
    timeStamp = localtime(&t);
    strftime(temp, sizeof(temp), "%Y:%m:%d %H:%M:%S",timeStamp);
    
    // Concatenate the time buffer to the ping message and return
    strcat(buffer, temp);
    return;
}

/*
Name        :   extractPingNumber()
Description :   Extracts the ping number from the ping message
Arguments   :   The buffer array where the ping message is stored
Return      :   The ping number
*/
int extractPingNumber(char buffer[]) {
    
    // Declare the required local variables
    char *token, bufferCopy[MAX_LENGTH];

    // Copy the buffer message into a local buffer
    strcpy(bufferCopy, buffer);

    // Extract the first words
    token = strtok(bufferCopy, " ");
    token = strtok(NULL, " ");

    // Convert the second word into number and return to the calling function
    return atoi(token);
}

/*
Name        :   main()
Description :   The driver code that calls the other subroutines
Arguments   :   The executable object, an option "-c" and the number of times the server needs to be pinged
Return      :   Integer 0
*/
int main(int argc, char **argv) {
    
    // Declare the required local variables
    char buffer[MAX_LENGTH];
    int socketFD, serverLength, messageLength, numberOfPings, packetLossCount;
    float RTT, maximumRTT, minimumRTT, totalRTT;
    struct sockaddr_in serverAddress;
    struct timeval timeout;
    clock_t start, end;

    // Validate and extract the command line arguments
    if(argc < 3 || strcmp(argv[1], "-c")!=0 || atoi(argv[2])<=0) {
        printf("Unknown command...!!!\n");
        printf("Expected format: <executable_object> -c <positive integer>\n");
        exit(0);
    }
    numberOfPings = atoi(argv[2]);

    // Try creating a Client Socket and print appropriate message
    bzero(&serverAddress, sizeof(serverAddress));
    if((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed.\n");
        exit(0);
    }
    printf("Socket creation successful...!!!\n");
    
    // Prepare the server address details for connection
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_family = AF_INET;

    // Initialize the required variables
    packetLossCount = 0;
    totalRTT = 0.0;

    // Configure the socket to set a timer of 1 second
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // Start pinging the server
    for(int i = 1; i <= numberOfPings; i++) {   
        
        // Prepare the ping message and display it
        bzero(&buffer, sizeof(buffer));
        formatPingMessage(buffer, i);
        printf("%s\t", buffer);

        // Send the ping message through the socket
        sendto(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        start = clock();
        
        // Start listening to the socket and print the appropriate response
        serverLength = sizeof(serverAddress);
        listen:
            messageLength = recvfrom(socketFD, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddress, &serverLength);
            end = clock();

        // If no response for upto 1 second, print Request Timeout and continue
        if(messageLength < 0) {
            printf("Request Timeout...!!!\n");
            packetLossCount++;
            continue;
        }

        // Extract the ping number of the received message and check for match. If not matching, continue listening
        if(extractPingNumber(buffer) < i)
            goto listen;

        // Compute the RTT
        RTT = ((float)(end - start))/CLOCKS_PER_SEC;
        
        // If the response received above 1 second, print Request Timeout and continue
        if(RTT > 1) {
            printf("Request Timeout...!!!\n");
            packetLossCount++;
            continue;
        }
        
        // Print the RTT of the ping message
        printf("Response:%s\tRTT:%.6f seconds\n", buffer, RTT);
        
        // Update Minimum RTT, Maximum RTT and the total RTT accordingly
        if((i == 1) || maximumRTT < RTT)
            maximumRTT = RTT;
        if((i == 1) || minimumRTT > RTT)
            minimumRTT = RTT;
        totalRTT += RTT;
    }

    // Calculate the average RTT and print the minimum, maximum and average RTTs only if atleast one response is received
    if(packetLossCount < numberOfPings) {
        float averageRTT = totalRTT/(numberOfPings - packetLossCount);
        printf("Minimum RTT:%f\tMaximum RTT:%f\tAverage RTT:%f\t", minimumRTT, maximumRTT, averageRTT);
    }

    // Calculate and report the Packet Loss Percentage
    printf("Packet Loss:%.2f%%\n", (packetLossCount * 100.0)/numberOfPings);

    // Close the socket and terminate the program
    close(socketFD);
    return 0;
}