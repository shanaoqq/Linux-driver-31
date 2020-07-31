#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/delay.h>

#define GPIONO(m,n) (m*32+n)
#define GPIOB8  GPIONO(1,8)
#define GPIOB16  GPIONO(1,16)

unsigned int irqno[2] = {GPIOB8,GPIOB16};
char *irq_name[2] = {"interrupter-gpiob8","interrupt-gpiob16"};

struct work_struct work;

void work_func(struct work_struct *work)
{
	int i=30;
	while(--i){
		printk("i = %d\n",i);
		mdelay(100);
	}
}

irqreturn_t handle_farsight_irq(int irqno, void *args)
{
	schedule_work(&work);
	return IRQ_HANDLED;
}


static int __init farsight_irq_init(void)
{
	int ret,i;
	//初始化work
	INIT_WORK(&work, work_func);
	//注册中断
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
	//注销中断
	for(i=0;i<ARRAY_SIZE(irqno);i++){
		free_irq(gpio_to_irq(irqno[i]),NULL);
	}

}
module_init(farsight_irq_init);
module_exit(farsight_irq_exit);
MODULE_LICENSE("GPL");










