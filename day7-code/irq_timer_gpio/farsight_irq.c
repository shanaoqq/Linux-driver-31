#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define GPIONO(m,n) (m*32+n)
#define GPIOB8  GPIONO(1,8)
#define GPIOB16  GPIONO(1,16)
#define GPIOA28  GPIONO(0,28)  //红灯
#define GPIOB12  GPIONO(1,12)  //蓝灯
#define GPIOE13  GPIONO(4,13)  //绿灯
unsigned int gpio_io[3] = {GPIOA28,GPIOB12,GPIOE13};
unsigned int irqno[2] = {GPIOB8,GPIOB16};
char *irq_name[2] = {"interrupter-gpiob8","interrupt-gpiob16"};

struct timer_list mytimer;
//定时器处理函数
void irq_timer_function(unsigned long data)
{
	unsigned int red,green;
	unsigned int gpiob8_status = gpio_get_value(GPIOB8);
	unsigned int gpiob16_status = gpio_get_value(GPIOB16);
	//判断管脚是否是低电平
	//在管脚的任何模式下都支持读
	if(gpiob8_status == 0){
		//红灯取反
		red = gpio_get_value(GPIOA28);
		red = !red;
		gpio_set_value(GPIOA28,red);
	
	}
	//判断管脚是否是低电平
	if(gpiob16_status == 0){
		//绿灯取反
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

	//RGB灯的初始化
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

	//定时器初始化
	mytimer.expires = jiffies + 10;
	mytimer.function = irq_timer_function;
	mytimer.data = 0;
	init_timer(&mytimer);
	add_timer(&mytimer); //定时器启动
	
	
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

	del_timer(&mytimer);

	for(i=0;i<ARRAY_SIZE(gpio_io);i++){
		gpio_free(gpio_io[i]);
	}
}
module_init(farsight_irq_init);
module_exit(farsight_irq_exit);
MODULE_LICENSE("GPL");










