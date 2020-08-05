#include <linux/init.h>
#include <linux/module.h>
#include <linux/genhd.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>

#define BLKNAME "mydisk"
#define RAMSIZE (1*1024*1024)

struct gendisk *mydisk;
int major;
struct request_queue *req;
spinlock_t lock;
char *ram_addr;

int mydisk_open(struct block_device *blk, fmode_t mod)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}
int mydisk_close(struct gendisk *disk, fmode_t mod)
{
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}

int mydisk_getgeo(struct block_device *blk, struct hd_geometry *hd)
{
	hd->heads = 4; //ָ���ڴ�Ĵ�ͷ�ĸ���
	hd->cylinders = 16;//ָ���ڴ�Ĵŵ��ĸ���
	hd->sectors = RAMSIZE/(hd->heads)/(hd->cylinders)/512; //���������ĸ���
	
	printk("%s:%s:%d\n",__FILE__,__func__,__LINE__);
	return 0;
}

//�����Ƕ����ж�д�Ĵ�����
void request_fn_handle_proc(struct request_queue *q)
{
	//1.�Ӷ�����ȡ��һ������
 	struct request *rq = blk_fetch_request(q);
	while(rq){
		//Ҫ��������ʼ���ֽ�
		unsigned long start = blk_rq_pos(rq) << 9;
		//����Ҫ�������ֽڵĳ���
		unsigned long len  = blk_rq_cur_bytes(rq);

		if(rq_data_dir(rq) == READ){
			//�����û���
			//Դ  ram_addr  |   Ŀ��rq->buffer   |����len
			printk("------------------\n");
			memcpy(rq->buffer, ram_addr+start, len);

		}else{
			//�����û�д
			//Ŀ��  ram_addr  |   Դrq->buffer   |����len
			printk("+++++++++++++++++++\n");
			memcpy(ram_addr+start,rq->buffer, len);
		}
		
		if (!__blk_end_request_cur(rq, 0))
			rq = blk_fetch_request(q);
	}

}

const struct block_device_operations fops = {
	.open    = mydisk_open,
	.release = mydisk_close,
	.getgeo  = mydisk_getgeo,
};

static int __init mydisk_init(void)
{
	//1.�����ڴ�
	mydisk = alloc_disk(4);
	if(mydisk == NULL){
		printk("alloc gendisk memory error\n");
		return -ENOMEM;
	}

	//2.�������豸��
	major = register_blkdev(0,BLKNAME);
	if(major <= 0){
		printk("alloc blk device number error\n");
		return -EAGAIN;
	}

	//3.��ʼ������
	spin_lock_init(&lock);
	req = blk_init_queue(request_fn_handle_proc,&lock);
	if(req == NULL){
		printk("blk queue init error\n");
		return -EAGAIN;
	}
	
	//4.��ʼ��gendisk
	mydisk->major = major;
	mydisk->first_minor = 0;
	strcpy(mydisk->disk_name,BLKNAME);
	mydisk->fops = fops;
	mydisk->queue = req;
	set_capacity(mydisk,RAMSIZE/512); //���÷���0�������ĸ���
		
	//5.Ӳ����صĲ���
	ram_addr = vmalloc(RAMSIZE);
	if(ram_addr == NULL){
		printk("alloc 1m memory error\n");
		return -ENOMEM;
	}
	
	//4.ע��
	add_disk(mydisk);

	return 0;
}
static void __exit mydisk_exit(void)
{
	del_gendisk(mydisk);
	vfree(ram_addr);
	blk_cleanup_queue(req);
	unregister_blkdev(major,BLKNAME);
	put_disk(mydisk);
}

module_init(mydisk_init);
module_exit(mydisk_exit);
MODULE_LICENSE("GPL");

