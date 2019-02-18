
#include <time.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)

int lastMessage = 0;

#include "talk_client.h"

Talk_client::Talk_client(char * ihost, char * iuser, char * ipassword, int iport)
{
    char * thost = (char *)malloc(sizeof(char) * strlen(ihost));
    char * tuser = (char *)malloc(sizeof(char) * strlen(iuser));
    char * tpassword = (char *)malloc(sizeof(char) * strlen(ipassword));
    memcpy(thost, ihost, strlen(ihost) + 1);
    memcpy(tuser, iuser, strlen(iuser) + 1);
    memcpy(tpassword, ipassword, strlen(ipassword) + 1);
    this->host = thost;
    this->user = tuser;
    this->password = tpassword;
    this->port = iport;
    Talk_client::add_user();
    totalChars = 0;
}

char * Talk_client::getUserName(){
    return user;
}

int Talk_client::open_client_socket(char * host, int port) {
    // Initialize socket address structure
    struct  sockaddr_in socketAddress;

    // Clear sockaddr structure
    memset((char *)&socketAddress,0,sizeof(socketAddress));

    // Set family to Internet
    socketAddress.sin_family = AF_INET;

    // Set port
    socketAddress.sin_port = htons((u_short)port);

    // Get host table entry for this host
    struct  hostent  *ptrh = gethostbyname(host);
    if ( ptrh == NULL ) {
        perror("gethostbyname");
        exit(1);
    }

    // Copy the host ip address to socket address structure
    memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

    // Get TCP transport protocol entry
    struct  protoent *ptrp = getprotobyname("tcp");
    if ( ptrp == NULL ) {
        perror("getprotobyname");
        exit(1);
    }

    // Create a tcp socket
    int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    // Connect the socket to the specified server
    if (connect(sock, (struct sockaddr *)&socketAddress,
            sizeof(socketAddress)) < 0) {
        perror("connect");
        exit(1);
    }

    return sock;
}

int Talk_client::sendCommand(char * host, int port, char * command, char * user,
    char * password, char * args, char * response) {
    int sock = open_client_socket( host, port);

    // Send command
    write(sock, command, strlen(command));
    write(sock, " ", 1);
    write(sock, user, strlen(user));
    write(sock, " ", 1);
    write(sock, password, strlen(password));
    write(sock, " ", 1);
    write(sock, args, strlen(args));
    write(sock, "\r\n",2);

    // Keep reading until connection is closed or MAX_REPONSE
    int n = 0;
    int len = 0;
    while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
        len += n;
    }
    *(response+len) = '\0';
    printf("response:%s\n", response);

    close(sock);
}

void Talk_client::add_user() {
    // Try first to add user in case it does not exist.
    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "ADD-USER", user, password, "", response);
}

void Talk_client::create_room(char * roomname){
    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "CREATE-ROOM", user, password, roomname, response);
}

char * Talk_client::list_rooms(){
    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "LIST-ROOMS", user, password, "" , response);
    char * re = response;
    return re;
}

void Talk_client::enter_room(char * roomname) {

    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "ENTER-ROOM", user, password, roomname, response);
    char str[100];
    strcpy(str, roomname);
    strcat(str, "has entered the room");
    char * msg = (char*)malloc(sizeof(char)*strlen(str)+1);
    strcpy(msg,str);
    Talk_client::send_message(msg);
}

void Talk_client::numReset(){
    totalChars = 0;
}

void Talk_client::leave_room(char * roomname) {

    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "LEAVE-ROOM", user, password, roomname, response);

    if (!strcmp(response,"OK\r\n")) {
        printf("User %s left room %s\n", user, roomname);
    }
}


char* Talk_client::get_messages(char* roomname) {

    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "GET-MESSAGES", user, password, roomname, response);
    int charNum = 0;
    int newLine = 0;
    char msg[ MAX_RESPONSE ];
    int pos = 0;
    int spaceCheck = 0;
    char* resp = response;
    while(*resp!='\0'){
        if(*resp==' '){
            newLine = 1;
            if(spaceCheck==0){
                resp++;

                spaceCheck = 1;

            }
        }
        if(*resp=='\n'){
            newLine = 0;
            spaceCheck = 0;

        }
        if(charNum>=totalChars - 1 && newLine == 1  && *resp!='\n'){
            msg[pos] = *resp;
            pos++;
        }
        resp++;
        charNum++;
    }

    msg[pos] = '\0';
    totalChars = charNum;

    char * ret = msg;
    strcpy(ret,msg);
    return ret;
}

void Talk_client::send_message(char * msg) {

    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "SEND-MESSAGE", user, password, msg, response);

}

char * Talk_client::print_users_in_room(char * roomname) {
    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "GET-USERS-IN-ROOM", user, password, roomname , response);
    char * re = response;
    return re;
}

void Talk_client::print_users() {
    char response[ MAX_RESPONSE ];
    sendCommand(host, port, "ADD-USER", user, password, "", response);

    if (!strcmp(response,"OK\r\n")) {
        printf("User %s added\n", user);
    }
}

void Talk_client::printPrompt() {
    printf("talk> ");
    fflush(stdout);
}

void Talk_client::printHelp() {
    printf("Commands:\n");
    printf(" -who   - Gets users in room\n");
    printf(" -users - Prints all registered users\n");
    printf(" -help  - Prints this help\n");
    printf(" -quit  - Leaves the room\n");
    printf("Anything that does not start with \"-\" will be a message to the chat room\n");
}

void * Talk_client::getMessagesThread(void * arg) {
    // This code will be executed simultaneously with main()
    // Get messages to get last message number. Discard the initial Messages

    while (1) {
        // Get messages after last message number received.

        // Print messages

        // Sleep for ten seconds
        usleep(2*1000*1000);
    }
}

void Talk_client::startGetMessageThread()
{
    //pthread_create(NULL, NULL, getMessagesThread(), NULL);
}
