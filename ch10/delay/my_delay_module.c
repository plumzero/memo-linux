// kernel_delay_module.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>  // 内核延迟函数头文件

static int __init delay_module_init(void)
{
  printk(KERN_INFO "=== Kernel Delay Module Loaded ===\n");
  
  // 内核中的延迟函数
  printk(KERN_INFO "1. mdelay(1000) - 延迟1秒开始\n");
  mdelay(1000);        // 忙等待1秒
  printk(KERN_INFO "   延迟1秒完成\n");
  
  printk(KERN_INFO "2. udelay(500) - 延迟500微秒开始\n");
  udelay(500);         // 忙等待500微秒
  printk(KERN_INFO "   延迟500微秒完成\n");
  
  // 注意：ndelay() 在某些架构上可能不支持
  printk(KERN_INFO "3. ndelay(100) - 延迟100纳秒尝试\n");
  ndelay(100);         // 忙等待100纳秒
  printk(KERN_INFO "   延迟100纳秒完成\n");
  
  return 0;
}

static void __exit delay_module_exit(void)
{
  printk(KERN_INFO "=== Kernel Delay Module Unloaded ===\n");
}

module_init(delay_module_init);
module_exit(delay_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("plumzero");
MODULE_DESCRIPTION("Kernel delay functions example");