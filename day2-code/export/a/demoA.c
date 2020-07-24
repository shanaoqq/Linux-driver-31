#include <linux/init.h>
#include <linux/module.h>

int add(int a,int b)
{
	return (a+b);
}
EXPORT_SYMBOL_GPL(add);

//入口
static int __init demo_init(void)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
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
