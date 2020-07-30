#define wait_event(wq, condition) 					
do {									
	if (condition)   	 						
		break;		 
	//如果条件为真，退出休眠					
	__wait_event(wq, condition);
	//这个函数中就会休眠进程
} while (0)


#define __wait_event(wq, condition) 					
	do {									
		DEFINE_WAIT(__wait);
		//定义了一个等待队列项，并将
		//当前的进程放到等待队列项中
									
		for (;;) {							
			prepare_to_wait(&wq, &__wait, TASK_UNINTERRUPTIBLE);
			//1.将等待队列项放到等待队列头后
			//2.将进程的状态从运行态改为不可
			//中断的等待态。
			if (condition)						
				break;	//条件为真退出循环		
			
			schedule(); 	
			//主动放弃cpu，进程休眠
			//王被唤醒的时候它就从这里接着往下执行
			//在唤醒的同时需要将condition设置为1
		}								
		finish_wait(&wq, &__wait);		
		//1.将进程的状态设置为运行
		//2.将等待队列项从等待队里头上删除

		
	} while (0)




