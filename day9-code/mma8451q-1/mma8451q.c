#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>

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

int mma8451q_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	global_client = client;
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);

	ret = i2c_read_reg(0x0d);

	printk("WHO_AM_I(0x1a) = %#x\n",ret);

	return 0;
}
int mma8451q_remove(struct i2c_client *client)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
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


