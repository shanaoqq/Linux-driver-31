#include <linux/init.h>
#include <linux/module.h>


struct gendisk *mydisk;

static int __init mydisk_init(void)
{
	//1.�����ڴ�
	mydisk = kzalloc(sizeof(*mydisk),GFP_KERNEL);
	if(mydisk == NULL){
		printk("alloc memory error\n");
		return -ENOMEM;
	}

	//2.��ʼ��


	//3.Ӳ����صĲ���


	//4.ע��

	return 0;
}
static void __exit mydisk_exit(void)
{
}

module_init();
module_exit();
MODULE_LICENSE("GPL");

