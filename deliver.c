#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc,char *argv[])
{
    //Check to see if there are the correct number of arguments
    if (argc != 3){
        printf ("Error\n");
        return -1;
    }
    char *str = NULL;
    str = (char *) malloc(1024);
    printf ("Please input a message as follows: ftp <file name>\n");
    fgets(str, 1024, stdin);

    if (strncmp (str, "ftp ", 4) != 0){
        printf ("Command must start with \"ftp\" \n");
        return -1;
    }
    char *file_name = str + 4;
    size_t len = strlen(file_name);

    // Remove trailing spaces and newline characters
    while (len > 0 && (file_name[len - 1] == ' ' || file_name[len - 1] == '\t' || file_name[len - 1] == '\n')) {
        len--;
    }

    // Set the null terminator at the last non-whitespace character
    file_name[len] = '\0';
    printf ("%s\n", file_name);

    if (access(file_name, F_OK) != 0) {
        // file doesn't exists
        printf("File with name: \"%s\" does not exist\n", file_name);
        return -1;
    }

    //Fill in server information
    int port = atoi(argv[2]);
    if (port == 0 && strcmp(argv[2], "0") !=0){
        //Failed string to int conversion
        printf("Invalid port number: %s\n", argv[2]);
        return -1;
    }

    char* address = argv[1];
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof (server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(address);


    //Make the socket
    int sockfd;
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    //Send "ftp"
    if (sendto(sockfd, "ftp", strlen("ftp"), 0, (struct sockaddr *) &server_address, sizeof (server_address))<0){
        printf ("Error\n");
        return -1;
    }

    //Receive message from the server
    ssize_t msg_len;
    char buffer [1024];
    printf ("ftp sent to the server\n");
    msg_len = recvfrom(sockfd, (char*) buffer, 1024, MSG_WAITALL, (struct sockaddr*) &server_address, NULL);
    buffer[msg_len] = '\0';

    if (strcmp(buffer, "yes") == 0){
        printf ("A file transfer can start.\n");
    }else {
        printf ("Exiting");
        return -1;
    }
    return 0;
}