#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#define MODENAME "mcp2515_dev"
#define CMD_RESET 0xc0
#define CMD_READ 0x3
#define CMD_WRITE 0x2
#define CMD_BIT_CHANGE 0x5

#define CF1 0x2a
#define CF2 0x29
#define CF3 0x28

#define CANCTRL 0xf
#define CANSTAT 0xe

#define RXB0CTRL 0x60
#define TXBnCTRL 0x30

#define CANINTE 0x2b
#define CANINTF 0x2c
struct mcp2515_dev
{
	int major;
	struct class *cls;
	struct device *dev;
	struct spi_device *spi_dev;
};

struct mcp2515_dev * mcp2515;
int mcp2515_bit_change(char reg,char mask,char data)
{
	
	int ret;
	char buf[] = {CMD_BIT_CHANGE,reg,mask,data};
	ret = spi_write(mcp2515->spi_dev,buf,sizeof(buf));
	if(ret){
		printk("spi write data fail\n");
		return ret;
	}
	return 0;
}

int spi_mcp2515_read(char reg)
{
	int ret;
	char buf[] = {CMD_READ,reg,0xff};
	char rxbuf[3];
	ret = spi_write_then_read(mcp2515->spi_dev,buf,sizeof(buf),rxbuf,sizeof(rxbuf));
	if(ret){
		printk("mcp2515 read data fail\n");
		return -EINVAL;
	}
	return rxbuf[2];
}

int spi_mcp2515_write(char reg,char val)
{
	int ret;
	char buf[] = {CMD_WRITE,reg,val};
	ret = spi_write(mcp2515->spi_dev,buf,sizeof(buf));
	if(ret){
		printk("spi write data fail\n");
		return ret;
	}
	mdelay(1);

	return 0;

}
int spi_mcp2515_reset(void)
{
	int ret;
	char buf[] = {CMD_RESET};
	ret = spi_write(mcp2515->spi_dev,buf,sizeof(buf));
	if(ret){
		printk("spi write data fail\n");
		return ret;
	}

	return 0;
}
static int mcp2515_open(struct inode *inode, struct file *file)
{
	int ret;
	printk("%s:%d\n",__func__,__LINE__);
	//ͨ��spi��mcp2515�Ĵ������г�ʼ��
	
	//1.��λmcp2515��������ģʽ
	spi_mcp2515_reset();
	mdelay(100);

retry:
	//2.��ȡCANCTRL����ģʽ��ȷ�Ͻ�������ģʽ�ɹ�
	ret = spi_mcp2515_read(CANSTAT);
	if(ret < 0){
		return -EINVAL;
	}
	if((ret & 0xe0) != 0x80) {
		printk("retrying..........\n");
		goto retry;

	}

	//3.ͨ��λ����ȷ�Ͻ�������ģʽ
	mcp2515_bit_change(CANCTRL,0xe0,0x80);

	//4.����λ��ʱ��עcan���ߵ�Ƶ�����Ϊ1mhz
	spi_mcp2515_write(CF1,0x3);
	spi_mcp2515_write(CF2,0x91);
	spi_mcp2515_write(CF3,0x3);

	//5.���ջ�����������������
	mcp2515_bit_change(RXB0CTRL,0x64,0x60);
	
	//6.ʹ�ܷ��ͻ������գ����ջ��������ж�
	spi_mcp2515_write(CANINTE,0x05);

	//7.������ģʽ�޸�Ϊ����ģʽ,���Ե�ʱ������ûػ�ģʽ
	//mcp2515_bit_change(CANCTRL,0xe0,0x40);  //�ػ�ģʽ
	mcp2515_bit_change(CANCTRL,0xe0,0x00);
	return 0;
}

ssize_t mcp2515_read(struct file *file, char __user *ubuf, size_t size, loff_t * offs)
{
	int i;
	char krbuf[14];
	printk("%s:%d\n",__func__,__LINE__);
	while(!(spi_mcp2515_read(CANINTF) & (1<<0)));   //�жϻ������Ƿ���

	mcp2515_bit_change(CANINTF,0x1,0x0); //����жϱ�־λ

	for(i=0; i<sizeof(krbuf); i++){
		krbuf[i] = spi_mcp2515_read(RXB0CTRL+i);
	}   
	if(copy_to_user(ubuf,&krbuf,sizeof(krbuf))){
		printk("copy_to_user fail ...%s,%d\n",__func__,__LINE__);
		return -EAGAIN;
	}

	return sizeof(krbuf);

}
ssize_t mcp2515_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offs)
{
	int i;
	char kwbuf[14];
	printk("%s:%d\n",__func__,__LINE__);
	if(copy_from_user(kwbuf,ubuf,sizeof(kwbuf))){
		printk("copy data from user error\n");
		return -EINVAL;
	}
	/* ������Ϣ�ı��ķ�װ����:
	*	��ʼ֡���ٲó�(��ʶ������չ��ʶ)�������ֶΣ����8�ֽڵ�����
	*   eg: 0x0b,0x23,0x08,0x11,0x00,0x08,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x99
	*/

	for(i=1; i<sizeof(kwbuf); i++){
		spi_mcp2515_write(TXBnCTRL+i,kwbuf[i]); //�����Ϣ
	}
	
	mcp2515_bit_change(TXBnCTRL,kwbuf[0],kwbuf[0]); //������Ϣ

	while(!(spi_mcp2515_read(CANINTF) & (1<<2)));   //�ж���Ϣ�Ƿ������

	mcp2515_bit_change(CANINTF,0x4,0x0); //����жϱ�־λ
	return size;

}

static int mcp2515_release(struct inode *inode, struct file *file)
{
	printk("%s:%d\n",__func__,__LINE__);
	return 0;
}


static struct file_operations fops = {
    .owner    = THIS_MODULE,
    .open           = mcp2515_open,
    .read           = mcp2515_read,
    .write          = mcp2515_write,
    .release        = mcp2515_release,
};

int spi_drv_probe(struct spi_device *spi)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);

	//1.����ṹ��ռ�
	mcp2515 = kzalloc(sizeof(*mcp2515),GFP_KERNEL);
	if(mcp2515 == NULL){
		printk("alloc mcp2515 is fail\n");
		return -ENOMEM;
	}
	mcp2515->spi_dev = spi;
	
	//2.ע���ַ�����
	mcp2515->major = register_chrdev(0,MODENAME,&fops);
	if(mcp2515->major == 0){
		printk("register chrdev error\n");
		return -EAGAIN;
	}

	//3.����class
	mcp2515->cls = class_create(THIS_MODULE,MODENAME);
	if(IS_ERR(mcp2515->cls)){
		printk("class create is fail\n");
		return PTR_ERR(mcp2515->cls);
	}
	//4.����device
	mcp2515->dev = device_create(mcp2515->cls,NULL,MKDEV(mcp2515->major,0),NULL,MODENAME);
	if(IS_ERR(mcp2515->dev)){
		printk("device create is fail\n");
		return PTR_ERR(mcp2515->dev);
	}
	return 0;
}
int spi_drv_remove(struct spi_device *spi)
{
	printk("%s:%d\n",__func__,__LINE__);	
	device_destroy(mcp2515->cls ,MKDEV(mcp2515->major,0));
	class_destroy(mcp2515->cls );
	unregister_chrdev(mcp2515->major,MODENAME);
	kfree(mcp2515);
	return 0;

}

static struct spi_device_id spi_id_tab[] = {
	{"mcp2515",0},
	{}
};

static struct spi_driver spi_driver_t = {
	.probe  = spi_drv_probe,
	.remove = spi_drv_remove,
	.driver = {
		.name = "mcp2515",
	},
	.id_table = spi_id_tab,
};

static int __init demo_init(void)
{
	return spi_register_driver(&spi_driver_t);
}

static void __exit demo_exit(void)
{
	spi_unregister_driver(&spi_driver_t);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");
