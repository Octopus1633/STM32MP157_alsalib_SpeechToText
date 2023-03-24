# STM32MP157_alsalib_SpeechToText
---
# 前言
`本篇分享：`

Linux应用编程之音频编程，使用户可以录制一段音频并进行识别(语音转文字)

`环境介绍：`

系统：Linux
硬件：正点原子STM32MP157开发板
声卡：开发板自带

---
# STM32MP157语音识别

实现目标 ：用户可以录制一段音频并进行识别(语音转文字)
知识点 : `C语言`、`文件IO`、~~`文件描述符重定向`~~ 、`alsa-lib 库`、`CURL命令`、`API调用`、`字符串拼接与解析`、`进程创建`、`exec函数族`、`多线程`。

## alsa-lib简介：

`alsa-lib`是一套 Linux 应用层的 C 语言函数库，为音频应用程序开发提供了一套统一、标准的接口，应用程序只需调用这一套 API 即可完成对底层声卡设备的操控，譬如播放与录音。
用户空间的`alsa-lib`对应用程序提供了统一的API 接口，这样可以隐藏驱动层的实现细节，简化了应用 程序的实现难度、无需应用程序开发人员直接去读写音频设备节点。所以，主要就是学习`alsa-lib`库函数的使用、如何基于`alsa-lib`库函数开发音频应用程序。 
`alsa-lib`官方说明文档：https://www.alsa-project.org/alsa-doc/alsa-lib/

## 移植alsa-lib库：



正点STM32MP157开发板出厂已移植(非广告!)，需要请参考其他教程。

要在嵌入式Linux系统上运行使用`alsa-lib`库的程序，需要移植`alsa-lib`库，可以参考网上移植`alsa-lib`库的方法，或自行下载`alsa-lib`资源包，自行编译移植。

开源ALSA架构的官网地址：https://www.alsa-project.org/wiki/Main_Page

## 安装CURL

在使用`curl`指令之前，需要先安装`curl`软件包。在大多数Linux发行版中，在ubuntu中可以使用以下命令来安装`curl`：

```
sudo apt-get install curl
```
## 移植CURL

正点STM32MP157开发板出厂已移植(非广告!)，需要请参考其他教程。
可以使用`curl`指令测试是否安装该软件包。

curl官方网站：<https://curl.se/> 

## API调用

该程序使用的是百度语音识别API

![在这里插入图片描述](https://img-blog.csdnimg.cn/09d16ccbb47149d5ae56c33a66ead797.png#pic_center)


注册后领取免费额度及创建中文普通话应用（创建前先领取免费额度（180 天免费额度，可调用约 5 万次左右） ）

![在这里插入图片描述](https://img-blog.csdnimg.cn/654a2b490b9049bd8830bab50c7805f4.png#pic_center)

创建好应用后，可以得到`API key`和`Secret Key`（填写到程序中的相应位置）

![在这里插入图片描述](https://img-blog.csdnimg.cn/70165c420ad24f8187e2f676f07de1be.png#pic_center)

调用API相关说明，Demo代码中有多种语言的调用示例可以参考，使用c语言的话也可以直接在本程序上面再次更改：

![在这里插入图片描述](https://img-blog.csdnimg.cn/6e820ae41dbf4aec976f1704612e237e.png#pic_center)


## 录音

查看[Linux应用编程-音频应用编程-语音转文字项目](https://blog.csdn.net/Octopus1633/article/details/128857164)中相应的标题。

## 文件IO

我们需要将录制的音频文件保存到本地，就需要用到文件IO相关知识，打开音频文件以及向音频文件写数据。

### 打开音频文件

函数：

```c
函数原型:
FILE *fopen(const char *filename, const char *mode)

参数:
filename -- 字符串，表示要打开的文件名称。
mode -- 字符串，表示文件的访问模式。

作用:
以指定的方式打开文件。
```

代码：

```c
/*创建一个保存PCM数据的文件*/
if ((pcm_data_file = fopen(argv[1], "wb")) == NULL)
{
    printf("无法创建%s音频文件.\n", argv[1]);
    exit(1);
}
printf("用于录制的音频文件已打开.\n");

参数:
argv[1]:程序执行时传递的参数,例./voice record.cpm，则该参数为"record.cpm"
"wb":只写打开或新建一个二进制文件，只允许写数据。
```

### 打开缓冲文件

除了要保存音频数据的文件，还要有保存CURL执行返回结果的文件作为程序缓冲文件，进而从缓冲文件中提取关键信息。

函数：

```c
头文件:
#include <fcntl.h>

函数原型:
int open(const char *pathname, int flags, mode_t mode);

参数:
pathname -- 文件路径名或文件名。
flags -- 打开文件所采用的操作。
mode -- 设置文件访问权限的初始值。

作用:
以指定的方式打开文件。
```

代码：

```c
int fd = open(buffer_FileName,O_WRONLY|O_CREAT|O_TRUNC,0777);

参数:
buffer_FileName -- 文件路径名或文件名。
O_WRONLY -- 以只读方式打开。
O_CREAT -- 若不存在则创建该文件。
O_TRUNC -- 若以只读方式打开，并存在该文件，则清空文件原内容。
0777 -- 打开全部权限。
```

## 文件描述符重定向（已删除）

重定向的作用是使"一个文件描述符"指向"另一文件描述符所指向的文件"，这里需要使终端不出现无关输出，又需要将CURL指令执行返回结果保存（CURL指令执行后会返回结果）。
==**后续发现curl参数-o可以实现相同功能，代码已更改，这部分就当复习好了...**==

例如：

获取Token：

![在这里插入图片描述](https://img-blog.csdnimg.cn/d2a512c212db4abdb02a5072e152e073.png#pic_center)


识别音频文件：
![在这里插入图片描述](https://img-blog.csdnimg.cn/65c38dd712c74757a21ba61cec2783b7.png#pic_center)


我们当然不希望这些返回结果直接打印在终端上，而后续程序又需要利用这些返回结果，这里就需要用到文件描述符的重定向。
学习过文件IO相关知识应该知道，终端标准输出对应的文件描述符是1（标准输入0，标准错误2），所以，我们只需要创建一个数据缓冲文件，得到数据缓冲文件的文件描述符fd，再将标准输出重定向到fd即可，这样终端输出的数据都会存储到fd指向的文件中。

函数：

```c
函数原型:
int dup(int oldfd)

参数:
oldfd -- 待拷贝的文件描述符。

作用:
使用现有的文件描述符，拷贝生成一个新的文件描述符，且函数调用前后这个两个文件
描述符指向同一文件，即oldfd指向的文件。

函数原型:
int dup2(int oldfd, int newfd)

参数:
oldfd -- 待拷贝的文件描述符。
newfd -- 新文件描述符。

作用:
使新文件描述符指向待拷贝文件描述符所指向的文件。
```

代码：

```c
int OUT_fd = dup(STDOUT_FILENO);
int fd = open(buffer_FileName,O_WRONLY|O_CREAT|O_TRUNC,0777);
dup2(fd,STDOUT_FILENO);

1.首先创建一个OUT_fd保存原标准输出，为了后续需要终端输出提示时能重定向回来。
2.打开数据缓冲文件，得到文件描述符fd。
3.重定向标准输出到fd。
```

## CURL命令

获取Token：

```c
格式:
curl -i -s 'https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=【百度云应用的AK】&client_secret=【百度云应用的SK】' -o 【数据缓冲文件】

示例:
curl -i -k 'https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=xxxxxxxxx&client_secret=xxxxxxxxx' -o buffer.txt

参数:
-i -- 显示传输文档，经常用于测试连接本身，含是否调用成功信息。
-s -- 静默模式，不输出任何东西。
-o -- 把输出写到该文件中。
```

音频文件识别：

```c
格式:
curl -i -X POST -H【固定头部header】"【语音识别请求地址】?dev_pid=【语言参数】&cuid=【用户唯一标识】&token=【token】" --data-binary "@【音频文件路径名】"

示例:
curl -i -X POST -H "Content-Type: audio/pcm;rate=16000" "http://vop.baidu.com/server_api?dev_pid=1537&cuid=xxxxx&token=1.a6b7dbd428f731035f771b8d********.86400.1292922000-2346678-124328" --data-binary "@/home/test/test.pcm"
```

## 字符串拼接与解析

当我们需要执行`CURL`指令时，需要对多个字符串进行拼接。

函数：

```c
函数原型:
int snprintf(char *str, size_t size, const char *format, ...)

参数:
str -- 拼接的结果保存在该字符串中。
size -- 如果平格式化后的字符串长度 < size，则将此字符串全部复制到str中，并给其后添加一个字符串结束符('\0')；如果格式化后的字符串长度 >= size，则只将其中的(size-1)个字符复制到str中，并给其后添加一个字符串结束符('\0')。
format -- 字符串格式。

示例:
snprintf(buf, 200, "%s,%d", string,num);

作用:
格式化字符串。
```

代码（构建获取Token的命令）：

```c
void Get_Token(char api_Key[],char secret_Key[])
{
    char API_TOKEN_URL[] = "https://aip.baidubce.com/oauth/2.0/token";
    char url_pattern[] = "%s?grant_type=client_credentials&client_id=%s&client_secret=%s";
    char url_common[200];
    pid_t pid;

    /*构建命令参数*/
    snprintf(url_common, 200, url_pattern, API_TOKEN_URL, api_Key, secret_Key);
}
```

## exec函数族

在程序中如果需要调用如`"ls、mv、cp"`等相关命令，就可以用`exec函数族`中的`execl 函数`来实现该功能。这里使用`execl函数`实现`curl`命令的调用。

函数：

```c
函数原型:
int execl(const char *path, const char *arg, ...);

语法:
int execl("绝对路径", "标识符",  "需要的参数（需要多少传入多少）" ,NULL);

示例:
execl("ls","ls","-l",NULL);

参数:
绝对路径 -- 文件存储的绝对路径，使用程序名在 PATH 中搜索。在终端可使用的命令这里直接输入命令即可。
标识符 -- 命令。
参数 -- 执行命令所需参数。

作用:
加载一个进程，通过路径+程序名来加载。
```

代码：

以执行`CURL`命令获取Token为例说明：

```c
void Get_Token(char api_Key[],char secret_Key[])
{
    ...

    /*构建命令参数*/
    snprintf(url_common, 200, url_pattern, API_TOKEN_URL, api_Key, secret_Key);

    /*调用curl命令*/
    execlp("curl","curl","-i","-s","-k",url_common,NULL);
    
    ...
}
```

## 进程创建

由于`exec函数族`的函数一旦调用成功即执行新的程序，不返回。 只有失败才返回，错误值-1。这样就会导致调用`execl函数`后整个程序终止，后续代码无法执行。
所以我们需要创建一个子进程去调用该函数，函数结束，子进程也就结束。

函数：

```c
函数原型:
pid_t fork(void)

作用:
创建一个子进程。

返回值:
失败返回-1；成功返回：父进程返回子进程的ID(非负)；子进程返回 0。注意返回值，不是 fork 函数能返回两个值，而是 fork 后，fork 函数变为两个，父子【各自】返回一个。
```

代码：

```c
void Get_Token(char api_Key[],char secret_Key[])
{
    ...

    /*创建子进程执行curl指令 执行完毕子进程结束*/
    pid = fork();
    if(pid == 0) execlp("curl","curl","-i","-s",url_common,"-o",buffer_FileName,NULL);

    /*回收子进程 不回收会存在僵尸进程*/
    wait(NULL);
    
    ...
}
```

## 多线程

函数:

```c
函数原型:
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)

thread -- 传出参数，保存系统为我们分配好的线程 ID
attr -- 通常传 NULL，表示使用线程默认属性。若想使用具体属性也可以修改该参数。
start_routine -- 函数指针，指向线程主函数，该函数运行结束，则线程结束。
arg -- 线程主函数执行期间所使用的参数。

作用:
创建一个新线程。

函数原型:
int int truncate(const char *path,off_t length);

参数:
path -- 文件路径名。
length --  截断长度，若文件大小>length大小，额外的数据丢失。若文件大小<length大小，那么，这个文件将会被扩展，扩展的部分将补以null,也就是‘\0’。

作用:
截断或扩展文件。
```

代码:

```c
/*创建子线程检测按键是否按下*/
pthread_t tid;
ret = pthread_create(&tid, NULL, button_tfn, NULL);
if (ret != 0) perror("pthread_create failed");

void *button_tfn(void *arg)
{
	struct input_event in_ev = {0};
	int fd;
	int value = -1;

	/*打开按键事件对应的文件*/
	if (0 > (fd = open("/dev/input/event0", O_RDONLY)))
	{
		perror("open error");
		exit(-1);
	}

	while(1)
	{
		/*循环读取数据*/
		if (sizeof(struct input_event) != read(fd, &in_ev, sizeof(struct input_event)))
		{
			perror("read error");
			exit(-1);
		}
		if (EV_KEY == in_ev.type && in_ev.code == 114)//114为KEY0 
		{ 
			/*按键事件*/
			switch (in_ev.value)
			{
				/*KEY0松开*/
				case 0:
					/**
					 * 1.更新按键状态为松开
					 * 2.延时等待主循环判断，否则可能出现主循环先判断标志位为1而出现PCM设备停止还在继续读数据
					 * 3.停止PCM设备
					*/
					key_flag_now = 0;
					sleep(1);
					snd_pcm_drop(capture_handle);
					break;
				/*KEY0按下*/
				case 1:
					/**
					 * 1.清空文件，使文件从头开始写，等于重新录制音频
					 * 2.同样注意顺序，先使设备恢复进入准备状态，避免出现主循环先检测到标志位为1而读取声卡设备
					 * 3.更新按键状态为按下
					*/
					truncate(pcm_file_name,1);
					snd_pcm_prepare(capture_handle);
					key_flag_now = 1;
					break;
			}
		}
		else if(EV_KEY == in_ev.type && in_ev.code == 115)//115为KEY1
		{
			/*按键事件*/
			switch (in_ev.value)
			{
				/*KEY1按下*/
				case 1:
					/*退出程序*/
					exit_program();
					break;
			}
				
		}
	}
}
```

## 主循环

主循环内判断声卡设备状态是否改变(按键状态决定)，若当前声卡为运行状态则进行音频采集，若当前声卡为停止状态则调用API进行识别。

```c
while (1)
{
    /*判断按键状态是否更新*/
    if(key_flag_now != key_flag_old)
    {
        /*视当前状态为旧状态*/
        key_flag_old = key_flag_now;

        /*若按键按下*/
        if(key_flag_now == 1)
            printf("开始采集音频数据...\n");
        /*若按键松开*/
        else
        {
            printf("采集结束!\n");

            /*识别音频 成功输出结果 出错退出程序*/
            ret = Speech_Recognition(argv[1],result);
            if(ret == 0)
                printf("识别的结果为:%s\n",result);
            else
                exit_program();

            printf("请长按KEY0按键开始采集音频数据!单击KEY1退出程序!\n");
        }
    }

    /*若按键按下*/
    if(key_flag_now == 1)
    {
        /*从声卡设备读取一帧音频数据:2048字节*/
        ret = snd_pcm_readi(capture_handle, buffer, buffer_frames);
        if(0 > ret)
        {
            printf("从音频接口读取失败(%s)\n", snd_strerror(ret));
            exit(1);
        }

        /*写数据到文件: 音频的每帧数据样本大小是16位=2个字节*/
        fwrite(buffer, (ret * AUDIO_CHANNEL_SET), frame_byte, pcm_data_file);
    }
}
```

## 实现效果及注意事项

### 实现效果

![在这里插入图片描述](https://img-blog.csdnimg.cn/d238b13e3227426c86eecf059e69c217.png#pic_center)


如图所示长按KEY0按键开始音频录制，松开即音频录制结束，再调用百度语言API进行识别，并向用户展示识别的结果。之后用户可自行选择继续识别或退出程序。

### 注意事项

该程序在声卡不进行录音时是将声卡设备给停止工作了的，**==在停止声卡设备前需要加入一小段的延时等待==**，若不添加延时等待，可能会出现子线程使声卡设备停止的同时主线程在读取声卡设备，从而导致下图中出现的错误：

![在这里插入图片描述](https://img-blog.csdnimg.cn/97b1f67c9b0b4ed1912c702fd3cb6189.png#pic_center)

---

# 源代码(转载请注明出处)
![在这里插入图片描述](https://img-blog.csdnimg.cn/a88529055df245c6b2daf345b28e0a06.png#pic_center)
