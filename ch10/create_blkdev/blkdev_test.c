#include <linux/fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/errno.h>
#include <linux/hdreg.h>
#include <linux/version.h>
 
#define MY_DEVICE_NAME "device-mapper"
 
static int mybdrv_ma_no, diskmb = 256, disk_size;     //定义ramdisk的大小是256MB
static char *ramdisk;
static struct gendisk *my_gd;
static spinlock_t lock;
static unsigned short sector_size = 512;
static struct request_queue *my_request_queue;
 
module_param_named(size, diskmb, int, 0);
static void my_request(struct request_queue *q)
{
	struct request *rq;
	int size, res = 0;
	char *ptr;
	unsigned nr_sectors, sector;
	pr_info("start handle request\n");
 
	rq = blk_fetch_request(q);    //从请求队列中拉一个request出来
	while (rq) {
		nr_sectors = blk_rq_cur_sectors(rq);   //这个请求需要多少个sector
		sector = blk_rq_pos(rq);
 		//因为我们的块设备是一个简单的以连续内存作为存储的一个虚拟的块设备，所以可以通过一个简单的计算算出当前的request它的存储地址和大小
		ptr = ramdisk + sector * sector_size;   
		size = nr_sectors * sector_size;
 
		if ((ptr + size) > (ramdisk + disk_size)) {
			pr_err("end of device\n");
			goto done;
		}
 
		if (rq_data_dir(rq)) {
			pr_info("writing at sector %d, %u sectors\n",
				sector, nr_sectors);
			memcpy(ptr, bio_data(rq->bio), size);   //数据做一个拷贝，将bio_data请求的数据拷贝到ptr指针里面
		} else {
			pr_info("reading at sector %d, %u sectors\n",
				sector, nr_sectors);
			memcpy(bio_data(rq->bio), ptr, size);
		}
done:
		if (!__blk_end_request_cur(rq, res))
			rq = blk_fetch_request(q);
	}
	pr_info("handle request done\n");
}
 
static int my_ioctl(struct block_device *bdev, fmode_t mode,
		    unsigned int cmd, unsigned long arg)
{
	long size;
	struct hd_geometry geo;
 
	pr_info("cmd=%d\n", cmd);
 
	switch (cmd) {
	case HDIO_GETGEO:
		pr_info("HIT HDIO_GETGEO\n");
		/*
		 * get geometry: we have to fake one...
		 */
		size = disk_size;
		size &= ~0x3f;
		geo.cylinders = size>>6;
		geo.heads = 2;
		geo.sectors = 16;
		geo.start = 4;
 
		if (copy_to_user((void __user *)arg, &geo, sizeof(geo)))
			return -EFAULT;
 
		return 0;
	}
	pr_warn("return -ENOTTY\n");
 
	return -ENOTTY;
}
 
static const struct block_device_operations mybdrv_fops = {
	.owner = THIS_MODULE,
	.ioctl = my_ioctl,
};
 
static int __init my_init(void)
{
	disk_size = diskmb * 1024 * 1024;    //简单给ramdisk设定一个大小256MB。
	spin_lock_init(&lock);
 
	ramdisk = vmalloc(disk_size);
	if (!ramdisk)
		return -ENOMEM;
 
	my_request_queue = blk_init_queue(my_request, &lock);  //初始化请求队列，第一个参数是函数指针
	if (!my_request_queue) {
		vfree(ramdisk);
		return -ENOMEM;
	}
	blk_queue_logical_block_size(my_request_queue, sector_size);
 
	mybdrv_ma_no = register_blkdev(0, MY_DEVICE_NAME);  //注册一个块设备，第一个参数为0的话就是让系统自动分配一个主设备号，第二个参数是设备名称，注册成功会返回一个主设备号
	if (mybdrv_ma_no < 0) {
		pr_err("Failed registering mybdrv, returned %d\n",
		       mybdrv_ma_no);
		vfree(ramdisk);
		return mybdrv_ma_no;
	}
 
	my_gd = alloc_disk(16);   //分配一个gendisk
	if (!my_gd) {
		unregister_blkdev(mybdrv_ma_no, MY_DEVICE_NAME);
		vfree(ramdisk);
		return -ENOMEM;
	}
 	//初始化
	my_gd->major = mybdrv_ma_no;  //主设备号
	my_gd->first_minor = 0;		  //次设备号
	my_gd->fops = &mybdrv_fops;    //方法集
	strcpy(my_gd->disk_name, MY_DEVICE_NAME);   //名字
	my_gd->queue = my_request_queue;    //请求队列
	set_capacity(my_gd, disk_size / sector_size);    //设置容量
    
	add_disk(my_gd);  //把gendisk添加到Linux系统的块设备子系统里   
 
	pr_info("device successfully   registered, Major No. = %d\n",
		mybdrv_ma_no);
	pr_info("Capacity of ram disk is: %d MB\n", diskmb);
 
	return 0;
}
 
static void __exit my_exit(void)
{
	del_gendisk(my_gd);
	put_disk(my_gd);
	unregister_blkdev(mybdrv_ma_no, MY_DEVICE_NAME);
	pr_info("module successfully unloaded, Major No. = %d\n", mybdrv_ma_no);
	blk_cleanup_queue(my_request_queue);
	vfree(ramdisk);
}
 
module_init(my_init);     //指定了驱动的入口，入口函数是my_init
module_exit(my_exit);
 
MODULE_AUTHOR("Benshushu");
MODULE_LICENSE("GPL v2");
