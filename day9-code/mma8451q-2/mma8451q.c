#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define MMANAME "mma8451q"
/**********************mma8451 reg********************************/
#define XYZ_STATUS 0x00
#define OUT_X_MSB 0x01
#define OUT_X_LSB 0x02
#define OUT_Y_MSB 0x03
#define OUT_Y_LSB 0x04
#define OUT_Z_MSB 0x05
#define OUT_Z_LSB 0x06

#define WHO_AM_I 0x0d
#define CHIP_ID  0x1a
#define XYZ_DATA_CFG 0x0e
#define CTRL_REG1 0x2a
#define CTRL_REG2 0x2b

/*****************************************************************/

int major = 0;
struct class *cls = NULL;
struct device *dev = NULL;
struct i2c_client *global_client = NULL;

int i2c_write_reg(char reg,char data)
{
	int ret;
	char w_buf[] = {reg,data};
	//封装消息
	struct i2c_msg w_msg = {
		.addr  = global_client->addr,
		.flags = 0,
		.len   = 2,
		.buf   = w_buf,
	};

	//发送消息
	ret = i2c_transfer(global_client->adapter,&w_msg,1);
	if(ret != 1){
		printk("i2c write reg error\n");
		return -EAGAIN;
	}
	
	return 0;
}

int i2c_read_reg(char reg)
{
	int ret;
	char r_data[1];
	char r_buf[] = {reg};
	//[0] = 7bitslave addr (write0)     reg
	//[1] = 7bitslave addr (read1)      data 
	struct i2c_msg r_msg[] = {
		[0] = {
			.addr  = global_client->addr,
			.flags = 0,
			.len   = 1,
			.buf   = r_buf,
		},
		[1] = {
			.addr  = global_client->addr,
			.flags = I2C_M_RD,
			.len   = 1,
			.buf   = r_data,
		},
	};

	//发送消息
	ret = i2c_transfer(global_client->adapter,r_msg,ARRAY_SIZE(r_msg));
	if(ret != ARRAY_SIZE(r_msg)){
		printk("i2c read reg error\n");
		return -EAGAIN;
	}

	return r_data[0];
}

int mma8451q_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}

ssize_t mma8451q_read(struct file *file, char __user *ubuf, 

		size_t size, loff_t *offs)
{
	int ret;
	int data[6],i,base_addr = 0x1;
	//1.等待数据准备好
	while(!(i2c_read_reg(XYZ_STATUS) & 0x08));

	//2.读取数据
	for(i=0;i<ARRAY_SIZE(data);i++){
		data[i] = i2c_read_reg(base_addr+i);
	}
	
	//3.将数据拷贝到用户空间
	if(size > sizeof(data)) size = sizeof(data);
	ret = copy_to_user(ubuf,data,size);
	if(ret){
		printk("copy data to user error");
		return -EIO ; 
	}


	return size;
}


int mma8451q_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}


static struct file_operations fops = {
	.open    = mma8451q_open,
	.read    = mma8451q_read,
	.release = mma8451q_close,

};

int mma8451q_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	global_client = client;
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);

	//1.校验who_am_i
	ret = i2c_read_reg(0x0d);
	if(ret == 0x1a){
		printk("check WHO_AM_I  ok....\n");
	}else{
		printk("check WHO_AM_I error...\n");
		return -EAGAIN;
	}

	//2.寄存器的初始化
	i2c_write_reg(CTRL_REG2,0x40); //软件复位
	i2c_write_reg(XYZ_DATA_CFG,0x2); //土8g
	i2c_write_reg(CTRL_REG1,0x1);  //使能，其他位采用默认配置
	i2c_write_reg(CTRL_REG2,0x2);  //设置高精度模式

	//3.注册字符设备驱动
	major = register_chrdev(0,MMANAME,&fops);
	if(major <= 0){
		printk("register char device driver error\n");
		return major;
	}	

	//4.创建设备节点
	cls = class_create(THIS_MODULE,MMANAME);
	if(IS_ERR(cls)){
		printk("class create error\n");
		return PTR_ERR(cls);
	}

	dev = device_create(cls,NULL,MKDEV(major,0),NULL,MMANAME);
	if(IS_ERR(dev)){
		printk("device create error\n");
		return PTR_ERR(dev);
	}

	return 0;
}
int mma8451q_remove(struct i2c_client *client)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	device_destroy(cls,MKDEV(major,0));
	class_destroy(cls);
	unregister_chrdev(major,MMANAME);
	return 0;
}
const struct i2c_device_id mma8451q_idtable[] = {
	{"mma8451q",},
	{/*end*/}
};

struct i2c_driver mma8451q = {
	.probe = mma8451q_probe,
	.remove = mma8451q_remove,
	.driver = {
		.name = "hellohello",
	},
	.id_table = mma8451q_idtable,
};

module_i2c_driver(mma8451q);
MODULE_LICENSE("GPL");


