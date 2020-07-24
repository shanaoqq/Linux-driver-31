#include <linux/init.h>
#include <linux/module.h>

extern int add(int a,int b);
//入口
static int __init demo_init(void)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	printk("sum = %d\n",add(100,200));
	return 0;
}

//出口
static void  __exit demo_exit(void)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
