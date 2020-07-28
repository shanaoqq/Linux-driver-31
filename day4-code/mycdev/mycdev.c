#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define CDEVNAME "mycdev"
struct cdev *cdev = NULL;
const int count = 3;
unsigned int major = 600;
unsigned int minor = 0;
struct class *cls = NULL;
struct device *dev = NULL;
char kbuf[128] = {0};
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
		return -EIO ; //è¿”å›žé”™è¯¯ç 
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
		return -EIO ; //è¿”å›žé”™è¯¯ç 
	}
	printk("kbuf = %s\n",kbuf);

	return size;
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
	.release = mycdev_close,
};

static int __init mycdev_init(void)
{
	int ret,i;
	dev_t devno;
	//1.·ÖÅäÇý¶¯¶ÔÏó
	cdev = cdev_alloc();
	if(cdev == NULL){
		printk("alloc cdev memory error\n");
		ret = -ENOMEM;
		goto ERR_STP0;
	}
	
	//2.×Ö·ûÉè±¸Çý¶¯µÄ³õÊ¼»¯
	cdev_init(cdev, &fops);
	
	//3.ÉêÇëÉè±¸ºÅ
	if(major > 0){
		//¾²Ì¬ÉêÇëÉè±¸ºÅ
		ret = register_chrdev_region(MKDEV(major,minor),count,CDEVNAME);
		if(ret){
			printk("static:alloc device number error\n");
			ret = -EBUSY;
			goto ERR_STP1;
		}
	}else{
		//¶¯Ì¬ÉêÇëÉè±¸ºÅ
		ret = alloc_chrdev_region(&devno,0,
			count,CDEVNAME);
		if(ret){
			printk("dynamic:alloc device number error\n");
			ret = -EINVAL;
			goto ERR_STP1;
		}
		major = MAJOR(devno);
		minor = MINOR(devno);
	}

		
	//4.×Ö·ûÉè±¸Çý¶¯µÄ×¢²á
	ret = cdev_add(cdev,MKDEV(major,minor),count);
	if(ret){
		printk("register char device driver error\n");
		ret = -EAGAIN;
		goto ERR_STP2;
	}

	//5.´´½¨Éè±¸½Úµã
	cls = class_create(THIS_MODULE,CDEVNAME);
	if(IS_ERR(cls)){
		printk("class create error\n");
		ret = PTR_ERR(cls);
		goto ERR_STP3;
	}

	for(i=0;i<count;i++){
		dev = device_create(cls,NULL,
				MKDEV(major,i),NULL,"mycdev%d",i);
		if(IS_ERR(dev)){
			printk("device create mycdev%d error\n",i);
			ret = PTR_ERR(dev);
			goto ERR_STP4;
		}
		
	}   //mycdev0 mycdev1 mycdev2
	
	return 0; //×¢Òâ£¬Ç§Íò²»ÄÜÍü¼ÇÐ´
	
ERR_STP4:
	for(--i;i>=0;i--){
		device_destroy(cls,MKDEV(major,i));
	}
	class_destroy(cls);
ERR_STP3:
	cdev_del(cdev);
ERR_STP2:
	unregister_chrdev_region(MKDEV(major,minor),count);
ERR_STP1:
	kfree(cdev);
ERR_STP0:
	return ret;
	
}

static void __exit mycdev_exit(void)
{
	int i=3;

	//1.Éè±¸½ÚµãµÄ×¢Ïú
	for(--i;i>=0;i--){
		device_destroy(cls,MKDEV(major,i));
	}
	class_destroy(cls);	
	//2.×Ö·ûÉè±¸Çý¶¯µÄ×¢Ïú
	cdev_del(cdev);
	//3.ÊÍ·ÅÉè±¸ºÅ
	unregister_chrdev_region(MKDEV(major,minor),count);
	//4.ÊÍ·Å¶ÔÏóµÄÄÚ´æ
	kfree(cdev);
	
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("farsight xxxx@xxx.com");


