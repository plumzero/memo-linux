#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

int get_mac(const char* name, unsigned char* buf, int bufsize)
{
  struct ifreq ifreq;
  int sock;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror ("socket");
    return -1;
  }

  strcpy(ifreq.ifr_name, name);    // Currently, only get eth0

  if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0) {
    perror ("ioctl");
    return -1;
  }
  
  memcpy(buf, ifreq.ifr_hwaddr.sa_data, 6);

  return 0;
}

int main()
{
  unsigned char buf[6] = {0};
  int ret = get_mac("enp0s31f6", buf, 6);
  if (ret != 0) {
    fprintf(stderr, "MAC ADDR\n");
    return -1;
  }

  for (int i = 0; i < 6; i++) {
    printf("%02X ", (int)buf[i]);
  }
  printf("\n");

  return 0;
}