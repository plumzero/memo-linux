// kernel_timer_module.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>

static struct timer_list my_timer;
static int count = 0;

void timer_callback(struct timer_list *t)
{
  printk(KERN_INFO "Timer callback called! Count: %d\n", ++count);
  
  // 重新设置定时器（周期性定时器）
  if (count < 5) {
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000)); // 1秒后再次触发
  } else {
    printk(KERN_INFO "Timer stopped after 5 iterations\n");
  }
}

static int __init timer_module_init(void)
{
  printk(KERN_INFO "=== Timer Module Loaded ===\n");
  
  // 初始化定时器
  timer_setup(&my_timer, timer_callback, 0);
  
  // 设置1秒后首次触发
  mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
  
  printk(KERN_INFO "Timer scheduled for 1 second from now\n");
  return 0;
}

static void __exit timer_module_exit(void)
{
  // 删除定时器
  int ret = del_timer(&my_timer);
  printk(KERN_INFO "Timer deleted, result: %d\n", ret);
  printk(KERN_INFO "=== Timer Module Unloaded ===\n");
}

module_init(timer_module_init);
module_exit(timer_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("plumzero");
MODULE_DESCRIPTION("Kernel timer example");