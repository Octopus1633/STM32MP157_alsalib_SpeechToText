/***************************************************************************
文件名 : function.c
作者 : Octopus
博客 : https://blog.csdn.net/Octopus1633?
描述 : 根据用户语音内容控制开发板硬件
交叉编译示例 : $ {CC} pcm_arm_1.c token.c asrmain.c common.c function.c -o voice -lasound -lcurl -lpthread
程序运行示例 : $ ./voice record.pcm
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "function.h"

/************************************
 LED相关文件宏定义
 ************************************/
#define LED_TRIGGER    "/sys/class/leds/user-led/trigger"
#define LED_BRIGHTNESS "/sys/class/leds/user-led/brightness"

/************************************
 蜂鸣器相关文件宏定义
 ************************************/
#define BEEP_TRIGGER    "/sys/class/leds/beep/trigger"
#define BEEP_BRIGHTNESS "/sys/class/leds/beep/brightness"

void Voice_Controll(char result[])
{
    /*检测语音和灯的控制有关*/
    if(strstr(result,"灯")!=NULL)
    {
        if(strstr(result,"开")!=NULL && strstr(result,"关")!=NULL)
            return;
        else if(strstr(result,"开")!=NULL)  
            Led_Controll(1);  
        else if(strstr(result,"关")!=NULL)  
            Led_Controll(0);  
    }

    /*检测语音和蜂鸣器的控制有关*/
    else if(strstr(result,"蜂鸣器")!=NULL)
    {
        if(strstr(result,"开")!=NULL && strstr(result,"关")!=NULL)
            return;
        else if(strstr(result,"开")!=NULL)  
            Beep_Controll(1);  
        else if(strstr(result,"关")!=NULL)  
            Beep_Controll(0);  
    }
}

void Led_Controll(int ONOFF)
{
    int fd1,fd2;

    /*打开LED触发文件*/
    fd1 = open(LED_TRIGGER, O_WRONLY);
    if (0 > fd1) {
        perror("open error");
        exit(-1);
    }

    /*打开LED开关文件*/
    fd2 = open(LED_BRIGHTNESS, O_WRONLY);
    if (0 > fd2) {
        perror("open error");
        exit(-1);
    }

    /*根据传递参数控制LED*/
    if(ONOFF == 1)
    {
        write(fd1, "none", 4);  //先将触发模式设置为 none
        write(fd2, "1", 1);     //点亮 LED
    }
    else
    {
        write(fd1, "none", 4);  //先将触发模式设置为 none
        write(fd2, "0", 1);     //熄灭 LED
    }

    /*关闭文件*/
    close(fd1);
    close(fd2);
}

void Beep_Controll(int ONOFF)
{
    int fd1,fd2;

    /*打开蜂鸣器触发文件*/
    fd1 = open(BEEP_TRIGGER, O_WRONLY);
    if (0 > fd1) {
        perror("open error");
        exit(-1);
    }

    /*打开蜂鸣器开关文件*/
    fd2 = open(BEEP_BRIGHTNESS, O_WRONLY);
    if (0 > fd2) {
        perror("open error");
        exit(-1);
    }

    /*根据传递参数控制蜂鸣器*/
    if(ONOFF == 1)
    {
        write(fd1, "none", 4);  //先将触发模式设置为 none
        write(fd2, "1", 1);     //蜂鸣器开
    }
    else
    {
        write(fd1, "none", 4);  //先将触发模式设置为 none
        write(fd2, "0", 1);     //蜂鸣器关
    }

    /*关闭文件*/
    close(fd1);
    close(fd2);
}
