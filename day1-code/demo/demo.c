#include <linux/init.h>
#include <linux/module.h>

//入口
static int __init demo_init(void)
{
	printk(KERN_ERR "hello every DC20031\n");
	printk("hello every DC20031##########\n"); 
	//如果不写打印级别就是默认打印级别
	printk(KERN_ERR "%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}

//出口
static void  __exit demo_exit(void)
{
	printk(KERN_ERR "bye every DC20031\n");
	printk(KERN_ERR "%s:%s:%d\n",__FILE__,__func__,__LINE__);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
