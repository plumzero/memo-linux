
#include <linux/module.h>    //所有模块都需要的头文件 
#include <linux/init.h>      // init&exit相关宏 
 
MODULE_LICENSE("Dual BSD/GPL");

int add_integer(int a, int b)
{
  return a + b;
}

int sub_integer(int a, int b)
{
  return a - b;
}

EXPORT_SYMBOL(add_integer);
EXPORT_SYMBOL(sub_integer);


/**
 * insmod module_symbol.ko 后
 * grep add_integer /proc/kallsyms 观察
*/