#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define GPIONO(m,n) (m*32+n)
#define GPIOB8  GPIONO(1,8)
#define GPIOB16  GPIONO(1,16)

unsigned int irqno[2] = {GPIOB8,GPIOB16};
char *irq_name[2] = {"interrupter-gpiob8","interrupt-gpiob16"};

struct timer_list mytimer;
//��ʱ��������
void irq_timer_function(unsigned long data)
{
	unsigned int gpiob8_status = gpio_get_value(GPIOB8);
	unsigned int gpiob16_status = gpio_get_value(GPIOB16);
	//�жϹܽ��Ƿ��ǵ͵�ƽ
	//�ڹܽŵ��κ�ģʽ�¶�֧�ֶ�
	if(gpiob8_status == 0){
		printk("left button down............\n");
	}
	//�жϹܽ��Ƿ��ǵ͵�ƽ
	if(gpiob16_status == 0){
		printk("right button down###########\n");
	}

}

irqreturn_t handle_farsight_irq(int irqno, void *args)
{
	mod_timer(&mytimer,jiffies+10);
	return IRQ_HANDLED;
}
static int __init farsight_irq_init(void)
{
	int ret,i;

	//��ʱ����ʼ��
	mytimer.expires = jiffies + 10;
	mytimer.function = irq_timer_function;
	mytimer.data = 0;
	init_timer(&mytimer);
	add_timer(&mytimer); //��ʱ������
	
	
	//ע���ж�
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
	//ע���ж�
	for(i=0;i<ARRAY_SIZE(irqno);i++){
		free_irq(gpio_to_irq(irqno[i]),NULL);
	}

	del_timer(&mytimer);
}
module_init(farsight_irq_init);
module_exit(farsight_irq_exit);
MODULE_LICENSE("GPL");










