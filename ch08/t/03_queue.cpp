
#include <stdio.h>
#include <stdint.h>

inline uint32_t RoundupPowofTwo(uint32_t n)
{
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  return (n + 1);
}

int main()
{
  printf("===> %d\n", RoundupPowofTwo(2));
  printf("===> %d\n", RoundupPowofTwo(4));
  printf("===> %d\n", RoundupPowofTwo(7));
  printf("===> %d\n", RoundupPowofTwo(11));

  return 0;
}