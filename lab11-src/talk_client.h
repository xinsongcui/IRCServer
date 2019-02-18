#ifndef TALK_CLIENT_H
#define TALK_CLIENT_H

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

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 300
#define MAX_RESPONSE (20 * 1024)

class Talk_client
{
    private:
        char * host;
        char * user;
        char * password;
        char * sport;
        int port;
        int totalChars;
    public:
        Talk_client(char * ihost, char * iuser, char * ipassword, int iport);
        int open_client_socket(char * host, int port);
        int sendCommand(char * host, int port, char * command, char * user,
            char * password, char * args, char * response);
        void add_user();
        char * list_rooms();
        void create_room(char * roomname);
        void enter_room(char * roomname);
        void leave_room(char * roomname);
        char * get_all_messages(char * roomname);
        char * get_messages(char * roomname);
        void send_message(char * msg);
        char * print_users_in_room(char * roomname);
        void print_users();
        void printPrompt();
        void printHelp();
        void * getMessagesThread(void * arg);
        void startGetMessageThread();
        char * getUserName();
        void numReset();
};

#endif // TALK_CLIENT_H
