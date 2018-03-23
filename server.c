#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define PATH "serverSocketFile"

struct fileDescriptorNode 
{
	int id,fd;
};

struct fileDescriptorNode clientDescriptionNodes[300];
int clientCounter = 0;

void *handleThreadsForClients(void *num)
{
	int currentFileDescriptor = clientDescriptionNodes[clientCounter-1].fd;
	int currentClientID = clientDescriptionNodes[clientCounter-1].id;
	int done = 0,n;
	char stringRecieved[100];
	
	do
	{
		n = recv(currentFileDescriptor, stringRecieved, 200, 0);
		if(n <= 0)
		{
			if(n < 0)
			{
				perror("Error in message reception: ");
			}
			done = 1;
		}
		if(!done)
		{
			int toBeSendClientID = atoi(strtok(stringRecieved," "));
			
			if(toBeSendClientID == 0)
			{
				if(send(currentFileDescriptor, "Invalid ID", 200, 0) < 0)
				{
					perror("Invalid ID");
				}
			}
			else if(toBeSendClientID != -1)
			{
				toBeSendClientID -= 1;
				printf("Parameter 1: %d\n", toBeSendClientID);
				char* msg = strtok(NULL, "\n");
				printf("Parameter 2: %s\n", msg);
				
				// if(msg == NULL || *msg == '\0')
				// {
				// 	printf("qewrtyew");
				// }

				if(msg == NULL || *msg == '\0')
				{
					if(send(currentFileDescriptor, "null Message", 200, 0) < 0)
					{
						perror("null Message");
					}
				}
				else
				{
					char sendMsg[200];
					sprintf(sendMsg, "Message Recieved - \n\tuser:%d \n\tmessage:%s\n", currentClientID, msg);
					if(toBeSendClientID == -1)
					{
						for(int i=0; i<clientCounter; i++)
						{
							if(currentClientID != clientDescriptionNodes[i].id)
							{
								if(send(clientDescriptionNodes[i].fd, sendMsg, 200, 0) < 0)
								{
									perror("Error in sending message - group/ user left: ");
								}
							}
						}
					}
					else
					{
						if(send(clientDescriptionNodes[toBeSendClientID].fd, sendMsg, 200, 0) < 0)
						{
							perror("Error in sending message/user left: ");
							if(send(currentFileDescriptor, "user not present", 200, 0) < 0)
							{
								perror("current user left!");
							}	
						}
					}
				}
			}
		}
	} while(!done);

	close(currentFileDescriptor);
	return NULL;
}

int main(void)
{
	int s,s2,len;
	unsigned int t;
	struct sockaddr_un local,remote;
	char str[100];

	s = socket(AF_UNIX,SOCK_STREAM,0);

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, PATH);
	unlink(PATH);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	bind(s,(struct sockaddr *)&local, len+1);
	if(listen(s,5) == -1)
	{
		printf("404: Unknown error.\n");
		return -1;
	}


	printf("Waiting for users to connect.\n");

	for(;;)
	{
		int done,n;
		t = sizeof(remote);
		s2 = accept(s,(struct sockaddr *)&remote, &t);
		clientDescriptionNodes[clientCounter].id = clientCounter;
		clientDescriptionNodes[clientCounter].fd = s2;
		clientCounter++;

		printf("Connected with id: %d\n", clientCounter);

		pthread_t thread1;
		pthread_create(&thread1, NULL, handleThreadsForClients, NULL);
		// done = 0;
		// do
		// {
		// 	n = recv(s2,str,100,0);
		// 	if(n<=0)
		// 	{
		// 		if(n<0)	perror("recv");
		// 		done = 1;
		// 	}
		// 	if(!done)
		// 	{
		// 		send(s2,str,n,0);
		// 	}
		// }while(!done);
		// close(s2);
	}
	return 0;
}
