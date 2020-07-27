#ifndef __CMD_H__
#define __CMD_H__

#define RED_ON  _IO('a',0)
#define RED_OFF _IO('a',1)

#define ACCESS_DATA_R _IOR('a',0,int)
#define ACCESS_DATA_W _IOW('a',0,int)

#define ACCESS_STRING_W _IOW('a',0,char [50])
#define ACCESS_STRING_R _IOR('a',0,char [50])



#endif



