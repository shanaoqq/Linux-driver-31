#define wait_event(wq, condition) 					
do {									
	if (condition)   	 						
		break;		 
	//�������Ϊ�棬�˳�����					
	__wait_event(wq, condition);
	//��������оͻ����߽���
} while (0)


#define __wait_event(wq, condition) 					
	do {									
		DEFINE_WAIT(__wait);
		//������һ���ȴ����������
		//��ǰ�Ľ��̷ŵ��ȴ���������
									
		for (;;) {							
			prepare_to_wait(&wq, &__wait, TASK_UNINTERRUPTIBLE);
			//1.���ȴ�������ŵ��ȴ�����ͷ��
			//2.�����̵�״̬������̬��Ϊ����
			//�жϵĵȴ�̬��
			if (condition)						
				break;	//����Ϊ���˳�ѭ��		
			
			schedule(); 	
			//��������cpu����������
			//�������ѵ�ʱ�����ʹ������������ִ��
			//�ڻ��ѵ�ͬʱ��Ҫ��condition����Ϊ1
		}								
		finish_wait(&wq, &__wait);		
		//1.�����̵�״̬����Ϊ����
		//2.���ȴ�������ӵȴ�����ͷ��ɾ��

		
	} while (0)




