#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main() 
{
  int sockfd,portno; 
  struct sockaddr_in server_addr; 
  char string[256]; 

  int x;
  printf("Enter the Port Number:");
  scanf("%d",&x);
  portno = x;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
  {
    printf("Error: Socket Creating Failed\n");
    printf("errno: %d\n", errno);
    return 1;
  }
  printf("Socket created\n");

  
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;  
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(portno); 

  printf("Enter Your String: \n");
  memset(string,0, 256);
  char temp[10];
  fgets(temp,sizeof(temp),stdin);
  fgets(string,256,stdin);

  if (sendto(sockfd, string, 256, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
  {
    printf("Error sending data to server");
    printf("errno: %d\n", errno);
    return 1;
  }

  if (recvfrom(sockfd, string, 256, 0, NULL,NULL) < 0) 
  {
    printf("Error receiving data from server");
    printf("errno: %d\n", errno);
    return 1;
  }

  printf("Reversed String:");
  printf("%s\n",string);

  close(sockfd);

  return 0;
}
