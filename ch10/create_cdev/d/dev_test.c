
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/device.h>

#define DevName "test"
#define ClassName "class_test"

struct class    *mem_class;
struct Pci_dev  *test_devices;
struct cdev 	_cdev;
dev_t    dev;

static int Test_open(struct inode *inode,struct file *filp)
{
	printk("debug: open success\n");
	return 0;
}

static int Test_release(struct inode *inode,struct file *filp)
{
	printk("debug: close success\n");
	return 0;
}

//这里只添加了open、release函数，如果需要用到ioctl函数在file_operations描述项里添加即可
static struct file_operations test_fops = {
.owner = THIS_MODULE,
//.ioctl = Test_ioctl,
.open = Test_open,
.release = Test_release,
};

static int Test_init_module(void)//驱动入口函数
{	
	//动态分配设备号
	int result = alloc_chrdev_region(&dev, 0, 2, DevName);
	if (result < 0)
	{
		printk("Err:failed in alloc_chrdev_region!\n");
		return result;
	}
	//创建class实例
	mem_class = class_create(THIS_MODULE,ClassName);// /dev/ create devfile 
  if (IS_ERR(mem_class))
  {
		printk("Err:failed in creating class!\n");
  }
  //动态创建设备描述文件 /dev/test
	device_create(mem_class, NULL, dev, NULL, DevName);

	cdev_init(&_cdev, &test_fops);
	_cdev.owner = THIS_MODULE;
	_cdev.ops = &test_fops;//Create Dev and file_operations Connected
	result = cdev_add(&_cdev,dev,1);
	return result;
}

static void Test_exit_module(void)//驱动退出函数
{
	if (0 != mem_class)
  {
		device_destroy(mem_class,dev);
		class_destroy(mem_class);
		mem_class = 0;
  }
	cdev_del(&_cdev);
}
module_init(Test_init_module);
module_exit(Test_exit_module);
MODULE_AUTHOR(DevName);
MODULE_LICENSE("GPL");
