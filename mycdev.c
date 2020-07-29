#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>

#define CDEVNAME "mycdev"
struct cdev *cdev = NULL;
const int count = 3;
unsigned int major = 600;
unsigned int minor = 0;
struct class *cls = NULL;
struct device *dev = NULL;
char kbuf[128] = {0};
int condition = 0; //条件
wait_queue_head_t wq; //定义等待队列头

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

	//1.判断用户打开的方式
	if(file->f_flags & O_NONBLOCK){
		return -EINVAL;
	}else{
		//2.条件不满足，休眠
		ret = wait_event_interruptible(wq,condition);
		if(ret){
			printk("wait error\n");
			return ret;
		}
	}

	//3.如果被唤醒了就将数据拷贝到用户空间
	if(size > sizeof(kbuf)) size = sizeof(kbuf);
	ret = copy_to_user(ubuf,kbuf,size);
	if(ret){
		printk("copy data to user error");
		return -EIO ; //杩斿洖閿欒鐮�
	}

	condition  = 0; //将条件设置为假

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
		return -EIO ; //杩斿洖閿欒鐮�
	}
	printk("kbuf = %s\n",kbuf);

	condition = 1; //将条件设置为真
	wake_up_interruptible(&wq); //唤醒队列
	
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
	//1.分配驱动对象
	cdev = cdev_alloc();
	if(cdev == NULL){
		printk("alloc cdev memory error\n");
		ret = -ENOMEM;
		goto ERR_STP0;
	}
	
	//2.字符设备驱动的初始化
	cdev_init(cdev, &fops);
	
	//3.申请设备号
	if(major > 0){
		//静态申请设备号
		ret = register_chrdev_region(MKDEV(major,minor),count,CDEVNAME);
		if(ret){
			printk("static:alloc device number error\n");
			ret = -EBUSY;
			goto ERR_STP1;
		}
	}else{
		//动态申请设备号
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

		
	//4.字符设备驱动的注册
	ret = cdev_add(cdev,MKDEV(major,minor),count);
	if(ret){
		printk("register char device driver error\n");
		ret = -EAGAIN;
		goto ERR_STP2;
	}

	//5.创建设备节点
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

	//6初始化等待队列头
	init_waitqueue_head(&wq);
	
	return 0; //注意，千万不能忘记写
	
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

	//1.设备节点的注销
	for(--i;i>=0;i--){
		device_destroy(cls,MKDEV(major,i));
	}
	class_destroy(cls);	
	//2.字符设备驱动的注销
	cdev_del(cdev);
	//3.释放设备号
	unregister_chrdev_region(MKDEV(major,minor),count);
	//4.释放对象的内存
	kfree(cdev);
	
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("farsight xxxx@xxx.com");


