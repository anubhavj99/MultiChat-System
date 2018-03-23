#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#define SOCK_PATH "serverSocketFile\0"

int s;

void *messageSendingFunction(void *num)
{
	char messageString[100];
	while(printf("Send message as userID<space>Message:\n-> "), fgets(messageString, 100, stdin), !feof(stdin)) 
	{
        if(messageString[0] == '\0')
        {

        }
        else if(messageString[0] == '\n')
        {

        }
        else if(messageString[0] == 'q')
        {
            printf("Quiting the program.\n");
            exit(1);
        }
        else if(send(s, messageString, strlen(messageString), 0) == -1) 
        {
            perror("Error in sending: ");
            exit(1);
        }
    } 
    exit(1);
}

void *messageRecievingFunction(void *num)
{
	while(1) 
	{
        int recvMessageLen;
		char messageString[200]; 
        if ((recvMessageLen = recv(s, messageString, 200, 0)) > 0) 
        {
            messageString[recvMessageLen] = '\0';
            printf("\n%s\nSend message as userID<space>Message:\n-> ", messageString);
        }
        else 
        {
        	if(recvMessageLen < 0)
            {
                perror("Error in recieving data from server: ");
            }
            else
            {
                printf("Server closed connection\nClosing the application\n");
            }
            exit(1);
        }
    }
}
int main(void)
{
    int len;
    struct sockaddr_un remote;
    char str[100];
    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
    {
        perror("Error in connection - client side: ");
        exit(1);
    }

    printf("Connecting to server\n");
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if(connect(s, (struct sockaddr *)&remote, len) == -1) 
    {
        perror("Error in connection - server side: ");
        exit(1);
    }
    
    printf("Connected.\n");
   
   	pthread_t thread1,thread2;

   	pthread_create(&thread1,NULL,messageSendingFunction,NULL);
   	pthread_create(&thread2,NULL,messageRecievingFunction,NULL);
   	pthread_join(thread1,NULL);
   	pthread_join(thread2,NULL);

    close(s);
    return 0;
}