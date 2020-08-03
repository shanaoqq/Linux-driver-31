#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

struct resource *res[2];
unsigned int type[] = {IORESOURCE_MEM,IORESOURCE_IRQ};
int irqno;
int pdrv_probe(struct platform_device *pdev)
{
	int i;
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	for(i=0;i<ARRAY_SIZE(res);i++){
		res[i] = platform_get_resource(pdev,type[i],0);
		if(res[i] == NULL){
			printk("platform get resource[%d] error\n",i);
			return -EAGAIN;
		}
	}

	printk("addr = %#x,irqno = %d\n",res[0]->start,res[1]->start);

	irqno = platform_get_irq(pdev,0);
	if(irqno < 0){
		printk("platform get irq error\n");
		return irqno;
	}
	printk("irqno = %d\n",irqno);
	return 0;
}
int pdrv_remove(struct platform_device *pdev)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}


struct platform_driver pdrv = {
	.probe = pdrv_probe,
	.remove = pdrv_remove,
	.driver = {
		.name = "hello_adc",
	},
};

static int __init pdrv_init(void)
{
	return platform_driver_register(&pdrv);
}
static void __exit pdrv_exit(void)
{
	platform_driver_unregister(&pdrv);
}
module_init(pdrv_init);
module_exit(pdrv_exit);
MODULE_LICENSE("GPL");


