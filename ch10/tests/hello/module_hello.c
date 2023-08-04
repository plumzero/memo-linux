
#include <linux/module.h>    //所有模块都需要的头文件 
#include <linux/init.h>      // init&exit相关宏 
 
static int __init hello_init (void) 
{
    printk("Hello module init\n");
    return 0; 
} 
 
static void __exit hello_exit (void) 
{
    printk("Hello module exit\n");
}
 
module_init(hello_init); 
module_exit(hello_exit);
 
MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("LYB");
MODULE_DESCRIPTION("test for linux driver");