#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

void rev_string(char string[])
{
    int length = strlen(string);
    int i;
    for (i = 0; i < length / 2; i++)
    {
        char temp = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = temp;
    }
}

int main()
{
    int sockfd, newsockfd, portno, client_len;
    char string[256];
    struct sockaddr_in serv_addr, cli_addr;

    int x;
    printf("Enter the Port Number: ");
    scanf("%d",&x);
    portno = x;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error: Socket Creating Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    printf("Socket created\n");

    memset((char*) &serv_addr,0,sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Error: Binding Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    printf("Binding Completed\n");

    listen(sockfd, 5);

    client_len = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &client_len);
    if (newsockfd < 0)
    {
        printf("Error: Connection Not Accepted\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    printf("Connection Accepted\n");

    memset(string, 0,256);
    int ret;
    ret = read(newsockfd, string, 255);
    if (ret < 0)
    {
        printf("Error: Reading Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }

    rev_string(string);

    ret = write(newsockfd, string, strlen(string));
    if (ret< 0)
    {
        printf("Error: Writing Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    shutdown(sockfd, SHUT_RDWR);
    shutdown(newsockfd, SHUT_RDWR);
    close(sockfd);
    close(newsockfd);
    return 0;
}
