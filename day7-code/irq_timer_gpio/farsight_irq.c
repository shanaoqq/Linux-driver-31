#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define GPIONO(m,n) (m*32+n)
#define GPIOB8  GPIONO(1,8)
#define GPIOB16  GPIONO(1,16)
#define GPIOA28  GPIONO(0,28)  //���
#define GPIOB12  GPIONO(1,12)  //����
#define GPIOE13  GPIONO(4,13)  //�̵�
unsigned int gpio_io[3] = {GPIOA28,GPIOB12,GPIOE13};
unsigned int irqno[2] = {GPIOB8,GPIOB16};
char *irq_name[2] = {"interrupter-gpiob8","interrupt-gpiob16"};

struct timer_list mytimer;
//��ʱ��������
void irq_timer_function(unsigned long data)
{
	unsigned int red,green;
	unsigned int gpiob8_status = gpio_get_value(GPIOB8);
	unsigned int gpiob16_status = gpio_get_value(GPIOB16);
	//�жϹܽ��Ƿ��ǵ͵�ƽ
	//�ڹܽŵ��κ�ģʽ�¶�֧�ֶ�
	if(gpiob8_status == 0){
		//���ȡ��
		red = gpio_get_value(GPIOA28);
		red = !red;
		gpio_set_value(GPIOA28,red);
	
	}
	//�жϹܽ��Ƿ��ǵ͵�ƽ
	if(gpiob16_status == 0){
		//�̵�ȡ��
		green = gpio_get_value(GPIOE13);
		green = !green;
		gpio_set_value(GPIOE13,green);
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

	//RGB�Ƶĳ�ʼ��
	for(i=0;i<ARRAY_SIZE(gpio_io);i++){
		gpio_free(gpio_io[i]);
	}
	
	for(i=0;i<ARRAY_SIZE(gpio_io);i++){
		ret = gpio_request(gpio_io[i],NULL);
		if(ret){
			printk("request gpio %d error\n",gpio_io[i]);
			return ret;
		}

		gpio_direction_output(gpio_io[i],0);
	}

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

	for(i=0;i<ARRAY_SIZE(gpio_io);i++){
		gpio_free(gpio_io[i]);
	}
}
module_init(farsight_irq_init);
module_exit(farsight_irq_exit);
MODULE_LICENSE("GPL");










