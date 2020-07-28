#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#define CDEVNAME "mycdev"

int major = 0;
char kbuf[128] = {0};
struct class *cls = NULL;
struct device *dev = NULL;
int mycdev_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}
ssize_t mycdev_read(struct file *file, char __user *ubuf, 
		size_t size, loff_t *offs)
{
	int ret;
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	if(size > sizeof(kbuf)) size = sizeof(kbuf);
	ret = copy_to_user(ubuf,kbuf,size);
	if(ret){
		printk("copy data to user error");
		return -EIO ;
	}

	return size;
}
ssize_t mycdev_write(struct file *file, 
		const char __user *ubuf,
		size_t size, loff_t *offs)
{
	int ret;
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	if(size > sizeof(kbuf))size = sizeof(kbuf);
	ret = copy_from_user(kbuf,ubuf,size);
	if(ret){
		printk("copy data from user error");
		return -EIO ; 
	}
	printk("kbuf = %s\n",kbuf);

	return size;
}
long mycdev_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
	switch(cmd){
		case 1:
			printk("cmd = %d,args = %ld\n",cmd,args);
			break;
		case 2: //ioctl在传递的时候不能使用2
			printk("cmd = %d,args = %ld\n",cmd,args);
			break;
		case 3:
			printk("cmd = %d,args = %ld\n",cmd,args);
			break;
	}

	return 0;
}

int mycdev_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}

static struct file_operations fops = {
	.open    = mycdev_open,
	.read    = mycdev_read,
	.write   = mycdev_write,
	.unlocked_ioctl = mycdev_ioctl,
	.release = mycdev_close,
};

static int __init demo_init(void)
{
	int i;
	major = register_chrdev(major,CDEVNAME,&fops);
	if(major < 0){
		printk("register char devices driver error\n");
		return major; 
	}

	cls = class_create(THIS_MODULE,CDEVNAME);
	if(IS_ERR(cls)){
		printk("class create error\n");
		return PTR_ERR(cls);
	}

	for(i=0;i<3;i++){
		dev=device_create(cls,NULL,MKDEV(major,i),NULL,"myled%d",i);
		if(IS_ERR(dev)){
			printk("create device myled%d error\n",i);
			return PTR_ERR(dev);
		}
	}
	return 0;
}

static void  __exit demo_exit(void)
{
	int i;
	for(i=0;i<3;i++){
		device_destroy(cls,MKDEV(major,i));
	}
	class_destroy(cls);
	unregister_chrdev(major,CDEVNAME);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
