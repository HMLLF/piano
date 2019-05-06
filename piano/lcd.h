#ifndef _LCD_H_
#define _LCD_H_

#include<stdio.h>
#include <sys/ioctl.h>
#include<sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include<linux/fb.h>
#include <unistd.h>
#include<sys/mman.h>

int Init_lcd();
void Draw_point(int x,int y,int color);
void close_lcd();

#endif