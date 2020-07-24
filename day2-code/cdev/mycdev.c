#include <linux/init.h>
#include <linux/module.h>
#define CDEVNAME "mycdev"
int major = 0;

static struct file_operations fops = {
	.open    = mycdev_open,
	.read    = mycdev_read,
	.write   = mycdev_write,
	.release = mycdev_close,
};
//入口
static int __init demo_init(void)
{
	//注册字符设备驱动
	major = register_chrdev(major,CDEVNAME,&fops);
	return 0;
}

//出口
static void  __exit demo_exit(void)
{
	//注销字符设备驱动
	unregister_chrdev
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
