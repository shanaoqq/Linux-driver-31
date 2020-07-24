#include <linux/init.h>
#include <linux/module.h>

int light=100;
module_param(light,int,0664);
MODULE_PARM_DESC(light,"this is lcd light(rang 0-100,default 100)");

char tt=50; //arm编译器认为char是一个无符号的类型，
			//ubuntu编译认识它是有符号的
module_param(tt,byte,0775);
MODULE_PARM_DESC(tt,"this is char var");

char *p="hello driver";
module_param(p,charp,0664);
MODULE_PARM_DESC(p,"this is a char point");

int ww[5] = {0};
int len;
module_param_array(ww,int,&len,0664);
MODULE_PARM_DESC(ww,"this is int [5] array");

//入口
static int __init demo_init(void)
{
	int i;
	printk("init :light = %d\n",light);
	printk("tt = %c\n",tt);
	printk("p = %s\n",p);
	
	for(i=0;i<len;i++){
		printk("ww[%d] = %d\n",i,ww[i]);
	}

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
