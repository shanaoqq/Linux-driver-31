#include <linux/init.h>
#include <linux/module.h>


struct gendisk *mydisk;

static int __init mydisk_init(void)
{
	//1.分配内存
	mydisk = kzalloc(sizeof(*mydisk),GFP_KERNEL);
	if(mydisk == NULL){
		printk("alloc memory error\n");
		return -ENOMEM;
	}

	//2.初始化


	//3.硬件相关的操作


	//4.注册

	return 0;
}
static void __exit mydisk_exit(void)
{
}

module_init();
module_exit();
MODULE_LICENSE("GPL");

