// client
/*----------------------------------------------------------------------------
 * Name : Choi ho yong
 * Student ID : 20130672
 * File name : client.c
 *
 * Project 1. Introduction of socket programming
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PACKETSIZE 1024
#define MAXDATASIZE 1000000

//check_port_ip_number : check command line correctness
// ./client -p PORTNUM -h IP or ./client -h IP -p PORTNUM
// 1024 <= PORTNUM <= 65536
bool check_port_ip_number (int argc, char** argv) // true -> port first, false -> ip first
{
  int port_location, ip_location;
  struct sockaddr_in soc_addr;

  if (!((strcmp (argv[1], "-p") == 0) || (strcmp (argv[1], "-h") == 0)) ||\
      !((strcmp (argv[3], "-p") == 0) || (strcmp (argv[3], "-h") == 0)) ||\
      (strcmp (argv[1], argv[3]) == 0) || (argc != 5))
  {
    perror ("client : argument\n");
    exit (1);
  }

  if (strcmp (argv[1], "-p") == 0) 
  {
    port_location = 2;
    ip_location = 4;
  }
  else
  { 
    port_location = 4;
    ip_location = 2;
  }

  for (int i = 0; i < strlen (argv[port_location]); i++)
  {
    if (isdigit (argv[port_location][i]) == 0)
    {
      perror ("server : PORT number\n");
      exit (1);
    }
  }

  if ((atoi (argv[port_location]) > 65536) ||\
      (atoi (argv[port_location]) < 1024))
  {
    perror ("server : PORT number\n");
    exit (1);
  }

  if (inet_pton (AF_INET, argv[ip_location], &(soc_addr.sin_addr)) != 1)
  {
    perror ("server : IP address\n");
    exit (1);
  }

  if (port_location == 2) return true;
  else false;
}

// main : make client which send message to server
int main (int argc, char** argv)
{
  int sockfd, location, i;
  size_t bytes;
  struct addrinfo hints, *servinfo, *p;
  int success, enter_number;
  char s[INET6_ADDRSTRLEN];
  bool is_port_first = check_port_ip_number (argc, argv);
  char *PORT, *IP;
  char buff[MAXDATASIZE];
  char *double_enter;

  memset (buff, 0, MAXDATASIZE);

  if (is_port_first)
  {
    PORT = argv[2];
    IP = argv[4];
  }
  else
  {
    PORT = argv[4];
    IP = argv[2];
  }

  memset (&hints, 0, sizeof (hints));
  hints.ai_family  = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if ((success = getaddrinfo (IP, PORT, &hints, &servinfo)) != 0) 
  {
    fprintf (stderr, "getaddrinfo : %s\n", gai_strerror (success));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket (p->ai_family, p->ai_socktype,\
                          p->ai_protocol)) == -1)
    {
      perror ("client : socket\n");
      continue;
    }

    if (connect (sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close (sockfd);
      perror ("client : connect\n");
      continue;
    }

    break;
  }

  if (p == NULL)
  {
    fprintf (stderr, "client : failed to connect\n");
    return 2;
  }

  freeaddrinfo (servinfo);

  // client is success to connect with server
  enter_number = 0;
  location = 0;

  while (1)
  {
    fgets (buff, MAXDATASIZE, stdin);

    if (buff[0] == '\n' && strlen (buff) == 1) //type just enter
    {
      enter_number++;
      if (enter_number == 2) break;
      continue;
    }

    bytes = strlen (buff);
    
    if (bytes == 0) //EOF case
    {
      close (sockfd);
      return 0;
    }

    if (bytes < PACKETSIZE) // short message case
    {
      if (send (sockfd, buff, bytes, 0) == -1)
      {
        perror ("client : send\n");
        exit (1);
      }
    }

    // If a message is long, divided the message to packet and send separately.
    else 
    {
      do
      {
        if (send (sockfd, &buff[location], PACKETSIZE, 0) == -1)
        {
          perror ("client : send\n");
          exit (1);
        }
        location += PACKETSIZE;
      } while ((int)bytes - location > PACKETSIZE);

      //last packet
      if (send (sockfd, &buff[location], strlen (&buff[location]), 0) == -1)
      {
        perror ("client : send\n");
        exit (1);
      }
    }

    // before reaching EOF or double enter, client does not terminate. 
    if (buff[bytes - 1] == '\n')
    {
      if (location == bytes)
      {
        if (send (sockfd, "\n", PACKETSIZE, 0) == -1)
        {
          perror ("client : send\n");
          exit (1);
        }
      }
      location = 0;
      enter_number = 1;
      memset (buff, 0, MAXDATASIZE);
    }
    else break;
  }

  close (sockfd);
  return 0;
}
