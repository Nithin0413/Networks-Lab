#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
# define port_no 11111

struct packet 
{
    int sequence_no;
    char payload[1024];
    int checksum;
};

void generate_random_string(char *str) 
{
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charset_size = sizeof(charset) / sizeof(char);

    for (int i = 0; i < 10; i++) 
    {
        str[i] = charset[rand() % charset_size];
    }
}
int checksum_calculator(struct packet data)
{
    int checksum=0;
    for (int i = 0; i < sizeof(data) - sizeof(int); i++) 
    {
        checksum =checksum^(((uint8_t*)(&data))[i]);
    }
    return checksum;
}

int main() 
{
  int sockfd;
  struct sockaddr_in server_addr; 
  struct packet data;
  int ack=1; // +1 - Acknowledgement and -1 - Negative Acknowledgement

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  // Socket creation
  if (sockfd < 0) 
  {
    printf("Error 1: Socket Creating Failed\n");
    printf("errno: %d\n", errno);
    return -1;
  }
  printf("Socket created succesfully\n");

  // Setting up sever
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;  
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port_no); 

  // Establishing connection
  int n = connect(sockfd,(struct sockaddr*)&server_addr, sizeof(server_addr));
  if(n<0)
  {
    printf("Error 2: Connection establishing failed\n");
    printf("%d\n",errno);
    return -1;
  }
  printf("Connection established successfully\n");

  int count = 1;
  
  while(1)
  {
      if(ack==1)
      {
        usleep(1000000);
        data.sequence_no=count;
        count++;
        generate_random_string(data.payload);
        data.checksum = checksum_calculator(data);
      }
      printf("-----------------Sending Pakcet %d-----------------\n",data.sequence_no);
      struct timespec start, end;
      clock_gettime(CLOCK_MONOTONIC, &start);
      n=write(sockfd,&data, sizeof(data));
      if(n<0)
      {
        printf("Error 3: Sending failed\n");
        printf("%d\n",errno);
        return -1;
      }
      n=read(sockfd,&ack,sizeof(int));
      clock_gettime(CLOCK_MONOTONIC, &end);
      if(n<0)
      {
        printf("Error 4: Recieving failed\n");
        printf("%d\n",errno);
        return -1;
      }
      if(ack==1)
      {
        printf("Postive Acknowledgement recieved\n");
        double rtt = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
        printf("Total Round Trip time for packet is -%f\n\n",rtt);
      }
      else if(ack==-1)
          printf("Negative Acknowledgement recieved - resend the packet\n");
  }
  close(sockfd);

  return 0;
}
