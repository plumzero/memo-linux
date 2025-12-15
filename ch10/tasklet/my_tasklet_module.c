
// my_module.c
#include <linux/module.h>      // 所有模块都需要
#include <linux/kernel.h>      // KERN_INFO 等
#include <linux/init.h>        // __init, __exit 宏
#include <linux/interrupt.h>   // tasklet 相关
#include <linux/slab.h>        // 如果需要动态内存分配

static void my_tasklet_func(unsigned long data)
{
  printk(KERN_INFO "Tasklet executed with data: %lu\n", data);
  // 注意：这里不能使用标准C库函数（如 printf）
  // 只能使用内核API（如 printk）
}

// 动态初始化
static struct tasklet_struct my_tasklet;

static int __init my_module_init(void)
{
  printk(KERN_INFO "Module loaded\n");
  
  // 动态初始化 tasklet
  tasklet_init(&my_tasklet, my_tasklet_func, 0UL);

  // 调度 tasklet
  tasklet_schedule(&my_tasklet);
  
  return 0;  // 返回 0 表示成功
}

static void __exit my_module_exit(void)
{
  // 禁用 tasklet（确保不会在执行后尝试访问）
  tasklet_kill(&my_tasklet);
  printk(KERN_INFO "Module unloaded\n");
}

// 注册模块入口和出口点
module_init(my_module_init);
module_exit(my_module_exit);

// 模块信息
MODULE_LICENSE("GPL");
MODULE_AUTHOR("plumzero");
MODULE_DESCRIPTION("A simple tasklet example module");
MODULE_VERSION("1.0");
