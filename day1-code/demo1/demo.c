#include <linux/init.h>
#include <linux/module.h>

int light=100;

module_param(light,int,0664);
MODULE_PARM_DESC(light,"this is lcd light(rang 0-100,default 100)");
//入口
static int __init demo_init(void)
{
	printk("init :light = %d\n",light);
	return 0;
}

//出口
static void  __exit demo_exit(void)
{
	printk("exit :light = %d\n",light);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
