#include"lcd.h"

int *plcd=NULL;
int lcd_fb;

/*画点*/
void Draw_point(int x,int y,int color)
{
	*(plcd+x*800+y)=color;
}

//初始化lcd
int Init_lcd()
{
	lcd_fb=open("/dev/fb0",O_RDWR);
	if(lcd_fb==-1)
	{
		perror("openlcd error:");
		return -1;
	}
	
	plcd=(int *)mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fb,0);
	if(plcd==MAP_FAILED)
	{
		perror("mmap error:");
		close(lcd_fb);
		return -1;
	}
}

//解映射，关闭文件
void close_lcd()
{
	munmap(plcd,800*480*4);
	close(lcd_fb);
}