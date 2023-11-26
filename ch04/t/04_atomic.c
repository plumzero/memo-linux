
#include <stdio.h>

int main()
{
    int i = 10;
    
    int ret = __sync_add_and_fetch(&i, 2);
    
    printf("i=%d, ret=%d\n", i, ret);
    
    __sync_lock_test_and_set(&i, 5);
    
    printf("i=%d\n", i);
    
    return 0;
}