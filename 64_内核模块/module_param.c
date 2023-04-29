#include <linux/init.h>
#include <linux/module.h>

/**
 * 遇到的问题：
 *  1.Makefile 中 obj-m 中的 *.o 与文件要与 *.c 文件相同，即 module_param.o module_param.c;
 *  2.Makefile 必须使用 Tab 符进行缩进;
 *  3.module_param.c 中，module_param_init 中建议传进去一个 void 参数，否则会报警告：
 *  	warning: function declaration isn’t a prototype [-Wstrict-prototypes]
 *  4.module_param.c 中，打印函数为 printk, 而不是 printf;
 * 
 * 加载进入内核操作步骤:
 *  1.为了便于管理，将生成的 module_param.ko 拷贝到 /opt/drivers/ 下
 *  	cp ./module_param.ko /opt/drivers
 *  	cd /opt/drivers
 *  2.加载内核程序，同时传参。参数结构 name=value = 两边不允许有空格，参数与参数之间使用空格分隔
 *  	insmod module_param.ko irq=2000 pstr=china fish=1,2,3,4,5,6 
 *  3.查看内核是否成功加载模块
 *  	lsmod | grep module_param
 *	 	cat /proc/modules | grep module_param
 *  4.一旦加载完毕，内核就会在 /sys/module/ 目录下生成一个与模块名同名的目录
 *    	ls -al /sys/module/module_param/
 *    	cat /sys/module/module_param/irq
 *    基中 /sys/module/module_param/parameters/ 目录下为模块内变量参数文件，可以通过修改参数文件来
 *    间接修改变量的值。如果权限为0，则没有同名的文件，只能在 insmod 时传递参数；也可以使用下列方
 *    式向参数文件中写入参数；
 *  5.为模块参数赋值
 *  	echo 25000 >/sys/module/module_param/parameters/irq
 *  	echo 100,200,300,400,500,600,700,800 >/sys/module/module_param/parameters/fish
 *  6.从内核中卸载模块
 *   	rmmod module_param.ko
 *  7.其他
 *	  每次重新加载模块之前需要从内核中卸载旧模块
 *	  传参个数不能超过定义数组时指定的长度
 *	  可以通过 modinfo module_param 查看模块信息
 *	  可以通过 modinfo -p module_param 查看模块参数
 *	  初学时最好创建一个空目录进行操作
 *
 *  函数:
 *  	module_param(name, type, perm);
 *  	功能：对需要进行传参的内核变量进行声明
 *  	参数：
 *  	      name	变量名
 *  	      type	类型，可以是 bool inbool char uchar short ushort int uint long ulong charp(字符指针)
 *   	      perm	设置对变量的访问权限，可参考 linux 文件系统权限
 *
 *  	module_param_array(name, type, nump, perm);
 *  	功能：对需要进行传参的内核数组进行声明
 *  	参数：
 *  	      name	数组名
 *  	      type	类型
 *  	      nump	数组指针
 *  	      perm	权限
 * */

/**
 *  Makefile 文件内容
 *
 *	ifneq ($(KERNELRELEASE),)
 *		obj-m := module_param.o
 *	else
 *		KDIR := /lib/modules/$(shell uname -r)/build
 *		PWD := $(shell pwd)
 *	all:
 *		make -C $(KDIR) M=$(PWD) modules
 *	clean:
 *		rm -f *.ko *.o *.symvers *.order *.mod.o *.mod.c
 *	endif
 *
 *  模块的编译分为：静态编译 和 动态编译 两种，动态编译又分为两种：
 *		静态编译：将驱动直接编译进内核
 *		动态编译：将驱动编译成模块
 *			内部编译：在内核源码目录内编译
 *			外部编译：在内核源码目录外编译
 *	本次编译为 动态外部编译， linux 内核版本 3.10.0-862.9.1.el7.x86_64
 *  
 *  1.Makefile 中的变量
 *	  1）KERNELRELEASE		在linux内核源代码中的顶层 makefile 中有定义
 *	  2）KDIR				当前内核的源代码目录
 *	  3）make -C $(KDIR) M=$(PWD) modules
 *       首先改变目录到 -C 指定的内核源代码目录，其中保存有内核顶层的 makefile
 *		 M 选项让该 makefile 在构造 modules 目标之前返回到模块源代码目录(即 $(pwd) 目录)；然后，modules 目标指向 obj-m 变量设定的模块
 *  2.Makefile 执行步骤
 *	  1）宏 KERNELRELEASE 未定义，进入 else;
 *    2）执行 all 规则：-C 进入到内核源代码目录执行顶层 Makefile，之后 KERNELRELEASE 被赋值。再返回到当前目录，再次执行 Makefile，生成 obj-m 设定的模块
 *  3.make 一共调用 3 次
 */

//定义全局变量
static int irq;
static char *pstr;

static int fish[10];
static int nr_fish;	//记录数组有效的个数

//命令行传参的声明
module_param(irq, int, 0664);		//权限非0
module_param(pstr, charp, 0);		//权限为0
module_param_array(fish, int, &nr_fish, 0664);

static int module_param_init(void)
{
	int i;
	printk("%s:irq = %d, pstr = %s\n", __FUNCTION__, irq, pstr);
	for (i = 0; i < nr_fish; i++)
		printk("%s:fish[%d] = %d\n", __FUNCTION__, i, fish[i]);
	return 0;
}

static void module_param_exit(void)
{
	int i;
	printk("%s:irq = %d, pstr = %s\n", __FUNCTION__, irq, pstr);
	for (i = 0; i < nr_fish; i++)
		printk("%s:fish[%d] = %d\n", __FUNCTION__, i, fish[i]);
}

module_init(module_param_init);
module_exit(module_param_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Create by C");
MODULE_DESCRIPTION("How to build a module");
MODULE_VERSION("Non-Version");
