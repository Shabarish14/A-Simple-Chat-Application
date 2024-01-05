#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>

#define MAXLINE 1000
#define MAX_TOKENS 1000
#define LISTENQ 1024


typedef struct
{
    char name[50];
    int connFdConnection;
    int authenticated;
} Client;

typedef enum {
    LOGIN,
    LOGOUT,
    CHAT,
    EXIT,
    INVALID_COMMAND
} Command_type;




void handlingSigintClosingConnections(int sig)
{
    printf("trying to close connections\n");
     for (int i = 0; i <= max; i++)
    {
        if (clients[i].connFdConnection > 0)
        {
            close(clients[i].connFdConnection);
        }
    }
    close(listeningFdConnection);
    exit(0);
}




char *RemoveFirstCharacterFromString(char *str)
{
    if (str == NULL || str[0] == '\0') {
        return str;
    }
    int len = strlen(str);
    memmove(str, str+1, len); 
    str[len-1] = '\0';
    return str;
}

int checkClientMessageValidations(int broadcast,int pointer,char *clientName, int destFdConnection)
{
    
    if(broadcast==1 && clients[pointer].authenticated)
    {
        return 1;
    }
    if(strcmp(clients[pointer].name, clientName) == 0)
    {
        return 1;
    }
    if(clients[pointer].connFdConnection == destFdConnection)
    {
        return 1;
    }
    return 0;

}

void sendMessageToSpecificClient(char *clientName, char *message, int dest_fd,int ind)
{
     int broadCast = 0;
    
    fflush(stdout);
   
    if (strcmp(clientName, "broadcast") == 0)
        broadCast = 1;
    
    fflush(stdout);

    int pointer =0;

    Y:
    {
        
        if (clients[pointer].connFdConnection >= 0 && checkClientMessageValidations(broadCast,pointer,clientName,dest_fd) ==1)
        {
            int bytesSent;
            
            if(ind>-1)
            {
                char* sender = clients[ind].name;
                char* sign=">>";
                char* newMessage = (char *)malloc(strlen(message) + strlen(sender) + strlen(sign)+1);
                strcpy(newMessage,sender);
                strcat(newMessage,">>");
                strcat(newMessage,message);
                
                bytesSent = send(clients[pointer].connFdConnection,newMessage,strlen(newMessage),0);
            }
            else{
                bytesSent = send(clients[pointer].connFdConnection, message, strlen(message), 0);
            }
            
            if (bytesSent == -1)
            {
                perror("Error sending message");
                
            }
            else
            {
                fflush(stdout);
            }
        }
        if(pointer<=max)
        {
            pointer=pointer+1;
            goto Y;
        }
        
    }
}

void sendMessageAllClients( char *message, int ind)
{

    int pointer =0;

    Y:
    {
        
        if (clients[pointer].connFdConnection >= 0 && clients[pointer].authenticated==1 && pointer!=ind)
        {
            int bytesSent;
            if(ind>-1)
            {
                char* sender = clients[ind].name;
                char* sign=">>";
                char* newMessage = (char *)malloc(strlen(message) + strlen(sender) + strlen(sign)+1);
                strcpy(newMessage,sender);
                strcat(newMessage,">>");
                strcat(newMessage,message);
                
                bytesSent = send(clients[pointer].connFdConnection,newMessage,strlen(newMessage),0);
            }
            else{
                bytesSent = send(clients[pointer].connFdConnection, message, strlen(message), 0);

            }

            if (bytesSent == -1)
            {
                perror("Error sending message");
                
            }
            else
            {
                fflush(stdout);
            }
        }
        if(pointer<=max)
        {
            pointer=pointer+1;
            goto Y;
        }
        
    }
}

int getCommandType(char* command) {
    if (strcmp(command, "login") == 0) {
        return LOGIN;
    }
    else if (strcmp(command, "logout") == 0) {
        return LOGOUT;
    }
    else if (strcmp(command, "exit") == 0) {
        return EXIT;
    }
    else if (strcmp(command, "chat") == 0) {
        return CHAT;
    }
    else {
        return INVALID_COMMAND;
    }
}


    
    char* result = (char*) malloc(sizeof(char) * (totalLength + 1));

    
    int index = 0;
    i = startIndex;
    while (i < numWords) {
        strcpy(result + index, words[i]);
        index += strlen(words[i]);
        result[index++] = ' '; 
        i++;
    }

    result[totalLength] = '\0';

    return result;
}





void decodeRequestRecieved(int ind, char *message)
{
    char *newMessage = (char *)malloc(strlen(message) + 1);
    int numTokens;
    
    strcpy(newMessage, message);
    
    char** tokens = NULL;
    char* token = strtok(message, " "); 
    int count = 0;
    while (token != NULL) {
        tokens = realloc(tokens, sizeof(char*) * (count + 1)); 
        tokens[count] = token; 
        count++;
        token = strtok(NULL, " "); 
    }
    numTokens = count;


    switch (getCommandType(tokens[0])) {
        case LOGIN:
            if (clients[ind].authenticated == 1)
            {
               
                sendMessageToSpecificClient(clients[ind].name,"Already Logged in",-1,-1);
            }
            else
            {
                strcpy(clients[ind].name, tokens[1]);
                clients[ind].authenticated = 1;
                printf("Loggedin\n");
            }
            
            break;
        case LOGOUT:
            if (clients[ind].authenticated == 0)
            {
                
                sendMessageToSpecificClient(clients[ind].name,"Should login to logout",-1,-1);
            }
            else
            {
                clients[ind].authenticated = 0;
                printf("Loggedout\n");
            }
            
            break;

        case EXIT:
       
            if (clients[ind].authenticated == 1)
            {
                 printf("if %d\n",clients[ind].authenticated);
        fflush(stdout);
                sendMessageToSpecificClient(clients[ind].name,"Please logout to exit ",-1,-1);
            }
            else
            {
                 printf(" else %d\n",clients[ind].authenticated);
                fflush(stdout);
                
                sendMessageToSpecificClient(clients[ind].name,"999",-1,-1);
                clients[ind].authenticated = 0;
            }
            
            break;
        case CHAT:
            if (clients[ind].authenticated == 0)
            {
                printf("Not logged in\n");
                sendMessageToSpecificClient(clients[ind].name,"Please login to continue to use chat or use exit command",-1,-1);
            }
            else
            {
                if (tokens[1][0] == '@')
                    sendMessageToSpecificClient(RemoveFirstCharacterFromString(tokens[1]),concatWords(tokens,numTokens,2), -1,ind);
                else
                    sendMessageAllClients(concatWords(tokens,numTokens,1), ind);
            }
            break;
        default:
            printf("wrong command\n");
            sendMessageToSpecificClient(clients[ind].name,"Wrong command ,If u want to use chat command u have to login first",-1,-1);
    }
}




int bindAndListenServer(int port) {
    int listeningFdConnection = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningFdConnection < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(listeningFdConnection, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(listeningFdConnection);
        return -1;
    }

    if (listen(listeningFdConnection, LISTENQ) < 0) {
        perror("listen");
        close(listeningFdConnection);
        return -1;
    }

    char hostname[1024];
    if (gethostname(hostname, sizeof(hostname)) < 0) {
        perror("gethostname");
        close(listeningFdConnection);
        return -1;
    }

    struct hostent *he = gethostbyname("localhost");
    if (!he) {
        perror("gethostbyname");
        close(listeningFdConnection);
        return -1;
    }

    struct sockaddr_in local_addr = {0};
    socklen_t local_addr_len = sizeof(local_addr);
    if (getsockname(listeningFdConnection, (struct sockaddr *)&local_addr, &local_addr_len) < 0) {
        perror("getsockname");
        close(listeningFdConnection);
        return -1;
    }

    printf("Hostname: %s\n", he->h_name);
    printf("IP address: %s\n", inet_ntoa(*((struct in_addr *)he->h_addr)));
    printf("Assigned port number: %d\n", ntohs(local_addr.sin_port));

    return listeningFdConnection;
}




int main()
{

    int i;
    struct sockaddr_in cliaddr;
    socklen_t clilen;

    fd_set allset, rset;
    FD_ZERO(&allset);
    signal(SIGINT, handlingSigintClosingConnections);

    char line[MAXLINE];
    char *key, *value;
    char *delim = ":";
    FILE *file = fopen("server_file", "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        exit(1);
    }
   // char *keys[1];
    char *values[1];
    int count = 0;
    int servPort;
    while (fgets(line, MAXLINE, file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        key = strtok(line, delim);
        value = strtok(NULL, delim);
        
        
        if (key != NULL && value != NULL) {
         //   keys[count] = strdup(key);
            values[count] = strdup(value);
            count++;
        }
    }
     
     
     
    servPort = (int)strtol(values[0], (char **)NULL, 10);
    

    

    for (i = 0; i <= FD_SETSIZE -1; i++)
    {
        clients[i].connFdConnection = -1;
        clients[i].authenticated = 0;
    }

    Z:
    {
        int one=1;
        
        rset = allset;
        // should return something
        select(maxFdConnections + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listeningFdConnection, &rset)*one)
        {
            clilen = sizeof(cliaddr);
            connFdConnection = accept(listeningFdConnection, (struct sockaddr *)&cliaddr, &clilen);

            for (i = 0; i < (FD_SETSIZE * one); i++)
            {
                if (clients[i].connFdConnection < 0)
                {
                    clients[i].connFdConnection = connFdConnection;
                    break;
                }
            }

            if (i == (FD_SETSIZE*one))
            {
                fprintf(stderr, "too many clients\n");
                exit(1);
            }

            FD_SET(connFdConnection, &allset);
            if (connFdConnection > maxFdConnections)
            {
                maxFdConnections = connFdConnection;
            }

            max = (i > max) ? i : max;

            printf("new client has been connected, port %d, clientNumber is %d\n", ntohs(cliaddr.sin_port), max);
        }

        for (i = 0; i < (max*one) + one; i++)
        {
          //  int is_received = 0;

            if ((connFdConnection = clients[i].connFdConnection) < 0 && one)
            {
                continue;
            }
            if (FD_ISSET(connFdConnection, &rset) && one)
            {
                
                char buff[2000];
                memset(buff, 0, 2000);
                ssize_t n;
                if ((n = read(connFdConnection, buff, MAXLINE)) == 0)
                {
                    printf("client connection closed:\n");

                    FD_CLR(connFdConnection, &allset);
                    clients[i].connFdConnection = -1;
                    close(connFdConnection);
                    strcpy(clients[i].name, "loggedout");
                    
                }
                else
                {
                   // is_received = 1;
                    decodeRequestRecieved(i, buff);
                    
                }
            }
        }
        goto Z;
    }

    return 0;
}
