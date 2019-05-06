#include "bmp.h"

int bmp_display(char *s,int pi_x)
{
	//step1:打开这个bmp文件
	int bmp_fd = open(s,O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open bmp file error:");
		return -1;
	}
	
	//step2:判断是否是一个bmp文件
	char buf[2];
	read(bmp_fd,buf,2);
	if(buf[0] != 0x42 || buf[1] != 0x4d)
	{
		close(bmp_fd);
		printf("no\n");
		return -1;
	}
	
	//step3:获取位图宽和高
	int w,h;
	lseek(bmp_fd,0x12,SEEK_SET);
	read(bmp_fd,&w,4);
	read(bmp_fd,&h,4);
	
	
	int size;
	//step4:像素数组的起始位置
	lseek(bmp_fd,10,SEEK_SET);
	read(bmp_fd,&size,4);
	
	//step5:获取色深
	short depth;
	lseek(bmp_fd,0x1c,SEEK_SET);
	read(bmp_fd,&depth,2);
	
	//读取颜色数据
	char bmpbuf[w*h*depth/8];
	char *p = bmpbuf;
	lseek(bmp_fd,54,SEEK_SET);
	read(bmp_fd,bmpbuf,w*h*depth/8);
	
	//画点
	
	int x,y;
	for(x=0;x<h;x++)
	{
		for(y=pi_x;y<pi_x+w;y++)
		{
			unsigned char r,g,b,a;
			int color;
			b=*(p++);
			g=*(p++);
			r=*(p++);
			a=(depth==24?0:*(p++));
			color=a<<24|r<<16|g<<8|b;
			Draw_point(480-1-x,y,color);
		}
		if(w*depth/8%4 != 0)
		{
			p += 4-w*depth/8%4;
		}
	}
	close(bmp_fd);
}