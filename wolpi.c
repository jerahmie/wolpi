#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>

#define UDP_PORT 9
#define MAC_BCASTS 16  // number of times to transmit MAC address in magic pkt
#define LEN_MAC_ADDR_WITH_SPACERS 17
#define LEN_MAC_ADDR_STR 22

/*
 * \fn magic_packet
 * 
 */
int magic_packet(const uint8_t *mac_addr,  uint8_t **message)
{
  int status = 0;
  int i;
  uint8_t *loc;
  const uint message_chars = (1 + MAC_BCASTS) * 6;
  
  *message = (uint8_t *)malloc(message_chars * sizeof(uint8_t));
  memset(*message, 0, message_chars*sizeof(uint8_t));
  for(i=0; i<6; i++) {
    memset(*message, 0xff, 6*sizeof(uint8_t));
  }
  for(i=1; i<=MAC_BCASTS; i++) {
    memcpy(*message+i*6, mac_addr, 6*sizeof(uint8_t));
  }

  return status; 
}

void usage(void)
{
  printf("Usage: wolpi mac_address\n");
  printf("mac_address: XX:XX:XX:XX:XX:XX\n");
}

int main(int argc, char *argv[])
{
  int s1 = 0;
  int i,j, status;
  struct sockaddr_in bcast_addr;
  int sock_val = 1;
  uint8_t *mac_addr, *message; 
  char *mac_addr_str, *mac_addr_str_head;
  const uint message_chars = (1 + MAC_BCASTS) * 6;

  // Process MAC address pass in as command line argument.
  if (argc != 2)
    {
      usage();
      return -1;
    }

  if (strlen(argv[1]) == LEN_MAC_ADDR_WITH_SPACERS)
    {
      mac_addr_str_head = (char *)malloc(sizeof(char)*LEN_MAC_ADDR_STR);
      memset(mac_addr_str_head, '\0', sizeof(char)*LEN_MAC_ADDR_STR);
      mac_addr_str = mac_addr_str_head;
      for (i=0; i<strlen(argv[1]); i++)
        {
          if ( *(argv[1]+i) == ':' )
            {
              *mac_addr_str = '\\';
              mac_addr_str++;
              *mac_addr_str = 'x';
              mac_addr_str++;
            }
          else
            {
              *mac_addr_str = *(argv[1]+i);
              mac_addr_str++;
            }
        }
      printf("%s\n", mac_addr_str_head);
    }

  mac_addr = (uint8_t *)malloc(sizeof(uint8_t)*6);
  memcpy( mac_addr, mac_addr_str_head, 6);
  //  memcpy( mac_addr, "\x00\x1b\x63\x99\xf5\xcd", 6);

  memset(&bcast_addr, 0, sizeof(bcast_addr));

  status = magic_packet(mac_addr, &message);
  bcast_addr.sin_family = AF_INET;
  bcast_addr.sin_port = (in_port_t)htons(UDP_PORT);
  bcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  s1 = socket(AF_INET, SOCK_DGRAM, 0);
  status = setsockopt(s1, SOL_SOCKET, SO_BROADCAST, &sock_val, sizeof(int));
  status = sendto(s1, message, sizeof(uint8_t)*6*(1+MAC_BCASTS), 0, 
                  (struct sockaddr *) &bcast_addr, sizeof(bcast_addr));

  free(mac_addr);
  free(message);
  free(mac_addr_str_head);
  return 0;
}
