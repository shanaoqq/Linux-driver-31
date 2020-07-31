#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define GPIONO(m,n) (m*32+n)
#define GPIOB8  GPIONO(1,8)
#define GPIOB16  GPIONO(1,16)

unsigned int irqno[2] = {GPIOB8,GPIOB16};
char *irq_name[2] = {"interrupter-gpiob8","interrupt-gpiob16"};

irqreturn_t handle_farsight_irq(int irqno, void *args)
{
	if(irqno == gpio_to_irq(GPIOB8)){
		printk("left button down............\n");
	}
	if(irqno == gpio_to_irq(GPIOB16)){
		printk("right button down###########\n");
	}
	return IRQ_HANDLED;
}
static int __init farsight_irq_init(void)
{
	int ret,i;
	//×¢²áÖÐ¶Ï
	for(i=0;i<ARRAY_SIZE(irqno);i++){
		ret = request_irq(gpio_to_irq(irqno[i]),handle_farsight_irq,
		IRQF_TRIGGER_FALLING,irq_name[i],NULL);
		if(ret){
			printk("request irq %s : %d error\n",irq_name[i],gpio_to_irq(irqno[i]));
			return ret;
		}
	}

	return 0;
}

static void __exit farsight_irq_exit(void)
{
	int i;
	//×¢ÏúÖÐ¶Ï
	for(i=0;i<ARRAY_SIZE(irqno);i++){
		free_irq(gpio_to_irq(irqno[i]),NULL);
	}
}
module_init(farsight_irq_init);
module_exit(farsight_irq_exit);
MODULE_LICENSE("GPL");










