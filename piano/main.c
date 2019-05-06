#include"lcd.h"
#include"bmp.h"
#include "thread_pool.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
//mp3文件
char *mp3[12]={"./mp3/d1.mp3",
			   "./mp3/d2.mp3",
			   "./mp3/d3.mp3",
			   "./mp3/d4.mp3",
			   "./mp3/d5.mp3",
			   "./mp3/d6.mp3",
			   "./mp3/d7.mp3",
			   "./mp3/d8.mp3",
			   "./mp3/d9.mp3",
			   "./mp3/d10.mp3",
			   "./mp3/d11.mp3",
			   "./mp3/d12.mp3"
				};
//获取触碰点
void *mp3_player(void*arg);
int Input()
{
	int x,y;
	int dev_fd=open("/dev/input/event0",O_RDONLY);
	if(dev_fd==-1)
	{
		perror("dev error:");
		return -1;
	}
	struct input_event event;
	int pre_i=-1;
	int i;
	int m,n;
	thread_pool*pool = (thread_pool*)malloc(sizeof(*pool));
	init_pool(pool,4);
	while(1)
	{
		int r = read(dev_fd,&event,sizeof(struct input_event));
		if(r != sizeof(struct input_event))
		{
			continue;
		}
		if(event.type == EV_ABS && event.code == ABS_X)
		{
			x = event.value;
			printf("x=%d\n",x);
		}
		
		if(event.type == EV_ABS && event.code == ABS_Y)
		{
			y = event.value;
			printf("y=%d\n",y);
			if(x>10&&x<75&&y>200&&y<480)				
			{
				i = 0;
				bmp_display("./key_on.bmp",10);
			}
			if(x>75&&x<140&&y>200&&y<480)				
			{
				i = 1;
				bmp_display("./key_on.bmp",75);	
			}
			if(x>140&&x<205&&y>200&&y<480)				
			{
				i = 2;
				bmp_display("./key_on.bmp",140);	
			}	
			if(x>205&&x<270&&y>200&&y<480)						
			{
				i = 3;
				bmp_display("./key_on.bmp",205);
			}
			if(x>270&&x<335&&y>200&&y<480)						
			{
				i = 4;
				bmp_display("./key_on.bmp",270);	
			}		
			if(x>335&&x<400&&y>200&&y<480)						
			{
				i = 5;
				bmp_display("./key_on.bmp",335);	
			}
			if(x>400&&x<465&&y>200&&y<480)						
			{
				i = 6;
				bmp_display("./key_on.bmp",400);	
			}
			if(x>465&&x<530&&y>200&&y<480)						
			{
				i = 7;
				bmp_display("./key_on.bmp",465);
			}
			if(x>530&&x<595&&y>200&&y<480)						
			{
				i = 8;
				bmp_display("./key_on.bmp",530);
			}
			if(x>595&&x<660&&y>200&&y<480)						
			{
				i = 9;
				bmp_display("./key_on.bmp",595);	
			}
			if(x>660&&x<725&&y>200&&y<480)						
			{
				i = 10;
				bmp_display("./key_on.bmp",660);	
			}
			if(x>725&&x<790&&y>200&&y<480)						
			{
				i = 11;
				bmp_display("./key_on.bmp",725);
			}
			
			if(pre_i != i&& pre_i != -1)
			{
				m=10+(65*pre_i);
				bmp_display("./key_off.bmp",m);
			}
			if(i != pre_i)
			{
				add_task(pool,mp3_player,(void*)mp3[i]);
			}
			pre_i = i;
		}
		//判断按下的是哪个键 i=0
		//on(i);
		//如果pre_i不等于I {off(pre_i)}
		//pre_i=i;
		if(event.type == EV_KEY&& event.value == 0)
		{
			n=10+(65*i);
			bmp_display("./key_off.bmp",n);
			return 0;
		}
	}	
	destroy_pool(pool);
	close(dev_fd);
	return 0;
}

//子线程播放音乐
void *mp3_player(void*arg)
{
	system("killall -9 madplay");
	char buf[256];
	sprintf(buf,"madplay %s",(char*)arg);
	system(buf);
	
}

int main()
{
	Init_lcd();
	
	if(0 == strcmp("./background.bmp"+strlen("./background.bmp")-4,".bmp"))
	{
		bmp_display("./background.bmp",0);
	}
	while(1)
	{
		Input();
	}
	
	close_lcd();
}