#include <stdio.h>
#include <string.h> // for strlen
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write
#include <time.h>
#define PORT_NUMBER 60000
#define BUFFER_SIZE 1024
int main(int argc, char *argv[])
{
    int socket_desc, new_socket, c, valread;
    struct sockaddr_in server, client;
    char buffer[BUFFER_SIZE] = {0};
    char *response;
    char *message;
    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    // Address Family - AF_INET (this is IP version 4)
    // Type - SOCK_STREAM (this means connection oriented TCP protocol)
    // Protocol - 0 (Specifies a particular protocol to be used with the socket...
    // Specifying a protocol of 0 causes socket() to use an unspecified default protocol)
    //[ or IPPROTO_IP This is IP protocol]
    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }
    server.sin_family = AF_INET;          // IPv4 Internet protocols
    server.sin_addr.s_addr = INADDR_ANY;  // IPv4 local host address
    server.sin_port = htons(PORT_NUMBER); // server will listen to 60000 port

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Binding failed");
        return 1;
    }
    puts("Socket is binded");

    // Listen
    listen(socket_desc, 1);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);

    while (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))
    {
        while (1)
        {
            if (recv(new_socket, buffer, BUFFER_SIZE, 0) < 0)
            {
                puts("Recv failed");
                close(socket_desc);
                return 1;
            }
            if (strcmp(buffer, "") != 0) //CLIENT HAS A REQUEST
            {
                write(new_socket, "\n", 1);
                time_t current_time = time(NULL);
                struct tm *tm_info = localtime(&current_time);
                if (strncmp(buffer, "GET_DATE", strlen("GET_DATE")) == 0)
                {
                    // Print the date
                    char date[128];
                    strftime(date, sizeof(date), "%D", tm_info);
                    write(new_socket, date, strlen(date));
                }
                else if (strncmp(buffer, "GET_TIME_ZONE", strlen("GET_TIME_ZONE")) == 0)
                {
                    struct tm tm = *localtime(&current_time);
                    long timezone_offset = tm.tm_gmtoff;
                    // Convert the time zone offset to hours and minutes
                    int timezone_hours = timezone_offset / 3600;
                    int timezone_minutes = abs(timezone_offset % 3600) / 60;
                    // Format the time zone string
                    char timezone_str[8];
                    sprintf(timezone_str, "%+03d:%02d", timezone_hours, timezone_minutes);
                    write(new_socket, timezone_str, strlen(timezone_str));
                }
                else if (strncmp(buffer, "GET_TIME_DATE", strlen("GET_TIME_DATE")) == 0)
                {
                    char time_date[128];
                    char date[128];
                    char time_string[128];
                    strftime(date, sizeof(date), "%D", tm_info);
                    write(new_socket, date, strlen(date));
                    write(new_socket, ",", 1);
                    strftime(time_string, sizeof(time_string), "%X", tm_info);
                    write(new_socket, time_string, strlen(time_string));
                }
                else if (strncmp(buffer, "GET_TIME", strlen("GET_TIME")) == 0)
                { // Print the time
                    char time_string[128];
                    strftime(time_string, sizeof(time_string), "%X", tm_info);
                    write(new_socket, time_string, strlen(time_string));
                }
                else if (strncmp(buffer, "GET_DAY_OF_WEEK", strlen("GET_DAY_OF_WEEK")) == 0)
                { // Print the day name
                    char date_string[128];
                    strftime(date_string, sizeof(date_string), "%A", tm_info);
                    write(new_socket, date_string, strlen(date_string));
                }

                else if (strncmp(buffer, "CLOSE_SERVER", strlen("CLOSE_SERVER")) == 0)
                { // close the server
                    message = "GOOD BYE.";
                    write(new_socket, message, strlen(message));
                    close(socket_desc);
                    close(new_socket);
                }
                else
                {
                    write(new_socket, "INCORRECT REQUEST\n", strlen("INCORRECT REQUEST\n"));
                }
                write(new_socket, "\n", 1);
            }
        }
    }
    close(socket_desc);
    close(new_socket);
    
    return 0;
}
