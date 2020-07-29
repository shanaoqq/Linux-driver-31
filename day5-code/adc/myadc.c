#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define CDEVNAME "myadc"
struct cdev *cdev = NULL;
const int count = 3;
unsigned int major = 600;
unsigned int minor = 0;
struct class *cls = NULL;
struct device *dev = NULL;

volatile unsigned int* adc_base = NULL;

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
	//1.¿ªÆôadc×ª»¯
	*adc_base |= (1<<0);
	
	//2.µÈ´ýadc×ª»¯Íê³É
	while((*adc_base)&0x1);

	//3.¶ÁÈ¡ADCDAT¼Ä´æÆ÷µÄÖµ
	data = *(adc_base+1);
	data = data & 0xfff;

	//4.½«¶ÁÈ¡µ½µÄÊý¾Ý¿½±´µ½ÓÃ»§¿Õ¼ä¼´¿É
	if(size > sizeof(data)) size = sizeof(data);
	ret = copy_to_user(ubuf,&data,size);
	if(ret){
		printk("copy data to user error");
		return -EIO ; //è¿”å›žé”™è¯¯ç 
	}

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

static int __init myadc_init(void)
{
	int ret,i;
	dev_t devno;

	cdev = cdev_alloc();
	if(cdev == NULL){
		printk("alloc cdev memory error\n");
		ret = -ENOMEM;
		goto ERR_STP0;
	}
	
	cdev_init(cdev, &fops);
	
	if(major > 0){
		ret = register_chrdev_region(MKDEV(major,minor),count,CDEVNAME);
		if(ret){
			printk("static:alloc device number error\n");
			ret = -EBUSY;
			goto ERR_STP1;
		}
	}else{
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

	ret = cdev_add(cdev,MKDEV(major,minor),count);
	if(ret){
		printk("register char device driver error\n");
		ret = -EAGAIN;
		goto ERR_STP2;
	}

	cls = class_create(THIS_MODULE,CDEVNAME);
	if(IS_ERR(cls)){
		printk("class create error\n");
		ret = PTR_ERR(cls);
		goto ERR_STP3;
	}

	for(i=0;i<count;i++){
		dev = device_create(cls,NULL,
				MKDEV(major,i),NULL,"myadc%d",i);
		if(IS_ERR(dev)){
			printk("device create myadc%d error\n",i);
			ret = PTR_ERR(dev);
			goto ERR_STP4;
		}
		
	}   

	//1.Ó³ÉäadcµÄ¼Ä´æÆ÷µØÖ·
	adc_base = ioremap(0xc0053000,20);
	if(adc_base == NULL){
		printk("ioremap error\n");
		ret = -ENOMEM;
		goto ERR_STP4;
	}

	//2.adcµÄ¼Ä´æÆ÷µÄ³õÊ¼»¯
	*(adc_base + 4) &= ~(0x3ff);
	*(adc_base + 4) |= (149<<0); //ÉèÖÃÊ±ÖÓ·ÖÆµÖµ
	*(adc_base + 4) |= (1<<15);  //·ÖÆµÊ±ÖÓÊ¹ÄÜ
	//2.2.2ÉèÖÃadccon¼Ä´æÆ÷
	*adc_base = 0;
	*adc_base |= (6<<6);
	
	return 0; 
	
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

static void __exit myadc_exit(void)
{
	int i=3;

	*adc_base |= (1<<2); //adc¿ØÖÆÆ÷¶Ïµç

	iounmap(adc_base);

	for(--i;i>=0;i--){
		device_destroy(cls,MKDEV(major,i));
	}
	class_destroy(cls);	

	cdev_del(cdev);

	unregister_chrdev_region(MKDEV(major,minor),count);

	kfree(cdev);
	
}

module_init(myadc_init);
module_exit(myadc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("farsight xxxx@xxx.com");


