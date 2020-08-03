#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

struct resource *res;
int irqno;
#define ADCNAME "myadc"
int major = 0;
unsigned int *adc_base = NULL;
struct class *cls = NULL;
struct device *dev = NULL;
wait_queue_head_t wq;
int condition = 0;

irqreturn_t adc_irq_handle(int adcirq, void *args)
{
	//唤醒
	condition = 1;
	wake_up_interruptible(&wq);
	//清除adc的中断状态标志位
	writel(1,adc_base+3);

	return IRQ_HANDLED;
}

int myadc_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}

ssize_t myadc_read(struct file *file, char __user *ubuf, 

		size_t size, loff_t *offs)
{

	int ret;
	int data;

	//1.开始adc转化
	writel((readl(adc_base) |(1<<0)),adc_base);

	//2.如果数据没有准备好就阻塞
	if(file->f_flags & O_NONBLOCK){
		return -EINVAL;
	}else{
		ret = wait_event_interruptible(wq,condition);
		if(ret){
			printk("wait error\n");
			return ret;
		}
	}

	//3.读取硬件的数据
	data = readl(adc_base+1);
	data = data & 0xfff;

	//4.拷贝数据到用户空间
	if(size > sizeof(data)) size = sizeof(data);
	ret = copy_to_user(ubuf,&data,size);
	if(ret){
		printk("copy data to user error");
		return -EIO ; 
	}

	//5.将条件设置为0
	condition = 0;
	
	return size;
}


int myadc_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}


static struct file_operations fops = {
	.open    = myadc_open,
	.read    = myadc_read,
	.release = myadc_close,

};

int pdrv_probe(struct platform_device *pdev)
{
	int ret;

	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	//1.获取资源
	res = platform_get_resource(pdev,IORESOURCE_MEM,0);
	if(res == NULL){
		printk("platform get resource error\n");
		return -EAGAIN;
	}
	
	irqno = platform_get_irq(pdev,0);
	if(irqno < 0){
		printk("platform get irq error\n");
		return irqno;
	}
	
	printk("addr = %#x,irqno = %d\n",res->start,irqno);


	//2.注册adc的字符设备驱动
	major = register_chrdev(major,ADCNAME,&fops);
	if(major <= 0){
		printk("register char device driver error\n");
		ret = major;
		goto ERR1;
	}

	//3.ADC硬件的地址映射和初始化
	adc_base = ioremap(res->start,(res->end-res->start));
	if(adc_base == NULL){
		printk("phy to virt ioremap error\n");
		ret = -ENOMEM;
		goto ERR2;
	}
	//writel(要写的值,地址);
	//读到的数据 = readl(地址);
	writel((readl(adc_base+4) & (~(0x3ff))),adc_base+4);
	writel((readl(adc_base+4) | (149<<0)),adc_base+4);
	writel((readl(adc_base+4) | (1<<15)),adc_base+4);
	writel(1,adc_base+2);  //开启adc中断
	writel(1,adc_base+3);  //清除adc的pending状态
	writel(0,adc_base);
	writel((readl(adc_base) |(6<<6)),adc_base);

	
	//3.注册中断
	ret = request_irq(irqno,adc_irq_handle,IRQF_DISABLED,ADCNAME,NULL);
	if(ret){
		printk("request irq error\n");
		goto ERR3;
	}

	//初始化等待队列头
	init_waitqueue_head(&wq);
	
	//4.创建设备节点
	cls = class_create(THIS_MODULE,ADCNAME);
	if(IS_ERR(cls)){
		printk("class create adc error\n");
		ret = PTR_ERR(cls);
		goto ERR4;
	}

	dev = device_create(cls,NULL,MKDEV(major,0),NULL,ADCNAME);
	if(IS_ERR(dev)){
		printk("device create adc error\n");
		ret = PTR_ERR(dev);
		goto ERR5;
	}

	return 0;
	
ERR5:
	class_destroy(cls);
ERR4:
	free_irq(irqno,NULL);
ERR3:
	iounmap(adc_base);
ERR2:
	unregister_chrdev(major,ADCNAME);
ERR1:
	return ret;
}
int pdrv_remove(struct platform_device *pdev)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	device_destroy(cls,MKDEV(major,0));
	class_destroy(cls);
	free_irq(irqno,NULL);
	iounmap(adc_base);
	unregister_chrdev(major,ADCNAME);	
	return 0;
}

struct platform_device_id adc_idtable[] = {
	{"platform_adc",},
	{"platform_adc1",},	
	{"platform_adc2",},
	{/*结束*/},
};

struct platform_driver padc = {
	.probe = pdrv_probe,
	.remove = pdrv_remove,
	.driver = {
		.name = "adc",
	},
	.id_table = adc_idtable,
};

module_platform_driver(padc);
//完成platform入口，出口，注册和注销功能

MODULE_LICENSE("GPL");


