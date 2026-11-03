#include <math.h>
#include <string.h>

#include "oled.h"
#include "delay.h"

/* GPIOS 宏定义 */
#define SDA_PORT GPIOB
#define SDA_PIN	GPIO_Pin_9

#define SCL_PORT GPIOB
#define SCL_PIN	GPIO_Pin_8

/* 写入方式 */
typedef enum {
	OLED_CMD = 0,//OLED的指令操作
	OLED_DAT = 1,//OLED的数据操作
}OLED_WriteType;

static OLED _oled;
static unsigned char _init_status = 0;

/* 函数声明 */
static void _oledCls(void);
static void _oledReset(void);
static void _oledGpioInit(void);
static void _oledUpDisBuffToDevice(void);
static void _oledRegisterConfiguration(void);
static void _oledSetPoint(unsigned char x,unsigned char y);
static void _oledWriteByte(OLED_WriteType type,unsigned char data);
static void _oledWriteBmpToDispalyMemory(unsigned int x,unsigned int y,BMP *bmp);
static void _oledWriteCharacterToDispalyMemory(unsigned int x,unsigned int y,char ch,FONT_SIZE size);
static void _oledWriteStringToDispalyMemoryEx(unsigned int line,char *str,FONT_SIZE size,ALIGN align);
static void _oledWriteStringToDispalyMemory(unsigned int x,unsigned int y,char *str,FONT_SIZE size,FunctionalState wordWrap);

//屏幕尺寸信息
#if OLED_SIZE == OLED_128_64
	static unsigned int _screen_hight=64;
	static unsigned int _screen_width=128;
	static unsigned char _oled_dis_buff[128][8];
#endif
/**
 *初始化MCU与OLED屏幕通讯的引脚
 *@param void 
 *@return void
 */
static void _oledGpioInit(void)
{	
	GPIO_InitTypeDef oledInitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//开启GPIO时钟
	oledInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//选择推挽输出模式
	oledInitStruct.GPIO_Speed = GPIO_Speed_50MHz;//选择输出速率
	
	//SDA
	oledInitStruct.GPIO_Pin = SDA_PIN;
	GPIO_Init(SDA_PORT,&oledInitStruct);
	GPIO_SetBits(SDA_PORT,SDA_PIN);
	
	//SCL
	oledInitStruct.GPIO_Pin = SCL_PIN;
	GPIO_Init(SCL_PORT,&oledInitStruct);
	GPIO_SetBits(SCL_PORT,SCL_PIN);
}

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_8)//CLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_8)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_9)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define IIC_DELAY() Delay_Us(2)


void IIC_Start()
{

	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}

void IIC_Wait_Ack()
{

	//GPIOB->CRH &= 0XFFF0FFFF;	//设置PB12为上拉输入模式
	//GPIOB->CRH |= 0x00080000;
//	OLED_SDA = 1;
//	delay_us(1);
	//OLED_SCL = 1;
	//delay_us(50000);
/*	while(1)
	{
		if(!OLED_SDA)				//判断是否接收到OLED 应答信号
		{
			//GPIOB->CRH &= 0XFFF0FFFF;	//设置PB12为通用推免输出模式
			//GPIOB->CRH |= 0x00030000;
			return;
		}
	}
*/
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
			m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
			da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		}


}



/********************* OLED控制函数 *********************/
// 写命令
void OLED_WriteCmd(uint8_t cmd)
{
   IIC_Start();
   Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
   Write_IIC_Byte(cmd); 
	IIC_Wait_Ack();	
   IIC_Stop();
}

// 写数据
void OLED_WriteData(uint8_t data)
{
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
   Write_IIC_Byte(data);
	IIC_Wait_Ack();	
   IIC_Stop();
}


/**
 *向OLED写入一个字节，命令或者数据
 *@param OLED_WriteType type 写入方式枚举
 *		 unsigned char data 数据值
 *@return void
 */
static void _oledWriteByte(OLED_WriteType type,unsigned char data)
{	
	if(type == OLED_CMD){
		OLED_WriteCmd(data);
	}else{
		OLED_WriteData(data);
	}
	
//	if(type == OLED_CMD)//写入方式
//		GPIO_ResetBits(DC_PORT,DC_PIN);
//	else
//		GPIO_SetBits(DC_PORT,DC_PIN);
//	
//	GPIO_ResetBits(CS_PORT,CS_PIN);//使能设备
//	
//	for(int index=0;index<8;index++)//循环发送数据  高位先行
//	{
//		GPIO_ResetBits(SCL_PORT,SCL_PIN);//拉低时钟线
//		if((data&0x80) == 0x80)//发送最高位	
//			GPIO_SetBits(SDA_PORT,SDA_PIN);
//		else
//			GPIO_ResetBits(SDA_PORT,SDA_PIN);
//		GPIO_SetBits(SCL_PORT,SCL_PIN);//拉高时钟线 完成一位的发送
//		data <<= 1;//数据左移一位
//	}GPIO_SetBits(CS_PORT,CS_PIN);//释放设备
}

/**
 *OLED屏幕的硬件复位
 *@param void
 *@return void
 */
static void _oledReset(void)
{
	//上电重置(低电平脉冲)
//	GPIO_WriteBit(RES_PORT,RES_PIN,Bit_SET);
//	GPIO_WriteBit(RES_PORT,RES_PIN,Bit_RESET);
//	GPIO_WriteBit(RES_PORT,RES_PIN,Bit_SET);	
}

/**
 *OLED默认寄存器配置
 *@param void
 *@return 默认唯一的控制结构体
 */
static void _oledRegisterConfiguration(void)
{
	//基本命令
	_oledWriteByte(OLED_CMD,0xAE);//关显示
	_oledWriteByte(OLED_CMD,0x81);//设置对比度（双字节：配合下一字节）
	_oledWriteByte(OLED_CMD,0xFF);//配置对比度为0x7F
	_oledWriteByte(OLED_CMD,0xA4);//恢复RAM内容的显示
	_oledWriteByte(OLED_CMD,0xA6);//正常显示（0关 1开）
	
	//滚屏命令
	_oledWriteByte(OLED_CMD,0x2E);//关闭滚屏功能
	
	//地址设定命令
    _oledWriteByte(OLED_CMD,0x00);//设置低四位页地址
	_oledWriteByte(OLED_CMD,0x10);//设置高四位页地址
	_oledWriteByte(OLED_CMD,0x20);//设置内存地址模式（双字节：配合下一字节）
	_oledWriteByte(OLED_CMD,0x00);//00：水平寻址	01：垂直寻址
	_oledWriteByte(OLED_CMD,0x21);//设置列地址 设置列的开始和结束地址（三字节：配合下两字节）
    _oledWriteByte(OLED_CMD,0x00);//设置开始地址 0x00
	_oledWriteByte(OLED_CMD,0x7f);//设置结束地址 0x7f
	_oledWriteByte(OLED_CMD,0x22);//设置页地址 设置页的开始和结束地址 (三字节：配合下两字节）
	_oledWriteByte(OLED_CMD,0x00);//设置开始地址 0x00
	_oledWriteByte(OLED_CMD,0x07);//设置结束地址 0x07
	_oledWriteByte(OLED_CMD,0xB0);//设置页开始地址
	
	//硬件配置命令
	_oledWriteByte(OLED_CMD,0x40);//设置显示RAM显示起始行寄存器 RAM0 -> COM0
	_oledWriteByte(OLED_CMD,0xA1);//设置段再现 用于左右反向（0：1）	水平翻转
	_oledWriteByte(OLED_CMD,0xA8);//设置多用复用率（双字节：配合下一字节）
	_oledWriteByte(OLED_CMD,0x3f);//--1/64 duty
	_oledWriteByte(OLED_CMD,0xC8);//设置COM扫描输出方向	 从COM0 到 COM[N–1] 垂直翻转
	_oledWriteByte(OLED_CMD,0xD3);//设置显示抵消(双字节：配合下一字节）
	_oledWriteByte(OLED_CMD,0x00);//偏移0
    _oledWriteByte(OLED_CMD,0xDA);//设置COM引脚（双字节：配合下一字节）
	_oledWriteByte(OLED_CMD,0x12);
	
	//设置时间和驱动方案命令
	_oledWriteByte(OLED_CMD,0xD5);//--set display clock divide ratio/oscillator frequency
	_oledWriteByte(OLED_CMD,0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	_oledWriteByte(OLED_CMD,0xD9);//--set pre-charge period
	_oledWriteByte(OLED_CMD,0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	_oledWriteByte(OLED_CMD,0xDB);//--set vcomh
	_oledWriteByte(OLED_CMD,0x40);//start line
	_oledWriteByte(OLED_CMD,0x8D);//设置充电泵启用/禁用			
		
	_oledSetPoint(0,7);
	for(int row=0;row<_screen_hight/8;row++)
		for(int col=0;col<_screen_width;col++)
			_oledWriteByte(OLED_DAT,0x00); 
	
	_oledWriteByte(OLED_CMD,0xAF);//开显示	
}

/**
 *设置显示位置
 *@param unsigned char x 
		 unsigned char y
 *@return void
 */
static void _oledSetPoint(unsigned char x,unsigned char y)
{
	if((x>=_screen_width) || y>=(_screen_hight/8))return;//非法地址

	x += 2;
	_oledWriteByte(OLED_CMD,0xB0+y%8);//设置页(即Y轴坐标)
	_oledWriteByte(OLED_CMD,0x00+(x&0x0f));//设置列低4bit
	_oledWriteByte(OLED_CMD,0x10+(x>>4));//设置列高4bit	
}

/**
 *同步显存到屏幕
 *@param void
 *@return void
 */
static void _oledUpDisBuffToDevice(void)
{
	unsigned int colMax=_screen_width;
	unsigned int rowMax=_screen_hight/8;
	
	//将显存数据同步到屏幕
	for(int row=0;row<rowMax;row++){
		_oledSetPoint(0x00,row);//设置指针
		for(int col=0;col<colMax;col++)
			_oledWriteByte(OLED_DAT,_oled_dis_buff[col][row]); 
	}
}

/**
 *OLED清屏操作
 *@param void
 *@return void
 */
static void _oledCls(void)
{
	memset(_oled_dis_buff,0x00,_screen_hight*_screen_width/8);
}

/**
 *OLED清除操作
 *@param unsigned int line 行0->127
 *		 unsigned int column 列0->7
 *@return void
 */
static void _oledClear(unsigned int line,unsigned int column)
{
	//地址判断
	if((line>=(_screen_hight/8)) || (column>=_screen_width))return;
	
	if((line==All_LINE) && (column==All_COLUMN))
		_oledCls();
	else if(line == All_LINE){
		for(int i=0;i<line;i++)
			_oled_dis_buff[column][i] = 0x00;
	}else if(column == All_COLUMN){
		for(int i=0;i<column;i++)
			_oled_dis_buff[i][line] = 0x00;
	}else
		_oled_dis_buff[column][line] = 0x00;
}

/**
 *在显存中写入字符数据
 *@param unsigned int x 横坐标值0->(_screen_width-1)
 *		 unsigned int y 纵坐标值0->(_screen_hight/8-1)
 *		 char ch 字符码
 *		 WordStock wordStock 字库类型
 *@return void
 */
static void _oledWriteCharacterToDispalyMemory(unsigned int x,unsigned int y,char ch,FONT_SIZE size)
{	
	if((x>=_screen_width) || (y>=_screen_hight))return;//非法地址

	FontAdapter *font = getFontAdatper(size,ch);///获得字体
	if(font == NULL)return;//判断字体是否有效
	
	//如果写入位置y是X的整数时，采用直接写入效率更高，用BMP绘制的形式更加通用
	if((y%0x08) == 0x00)
	{		
		_oled.showBmp(x,y,(BMP*)font);
	}else _oled.showBmp(x,y,(BMP*)font);
}

/**
 *在显存中写入字符串数据
 *@param unsigned int x 横坐标值0->(_screen_width-1)
 *		 unsigned int y 纵坐标值0->(_screen_hight/8-1)
 *		 char str 字符串
 *		 WordStock wordStock 字库类型
*  		 FunctionalState wordWrap 自动回车使能
 *@return void
 */
static void _oledWriteStringToDispalyMemory(unsigned int x,unsigned int y,char *str,FONT_SIZE font_size,FunctionalState wordWrap)
{
	//指针存在异常	
	if(str == NULL)return;
	
	//非法地址
	if((x>=_screen_width) || (y>=_screen_hight))return;
	
	//获取字体
	FontAdapter *font = getFontAdatper(font_size,str[0]);
	
	//判断字体是否有效
	if(font == NULL)return;
		
	BMP bitmap;
	int str_length= strlen(str);
	for(int i=0;i<str_length;i++)
	{
		if((unsigned char)str[i] >= 0x80){
			//chinese
			font = getFontAdatper(font_size,(str[i]<<8)|str[i+1]);	
			i++;
		}else{
			//ascii
			font = getFontAdatper(font_size,str[i]);	
		}
		
		if(NULL == font){return;}
				
		bitmap.width = font->width;
		bitmap.height = font->height;
		bitmap.bmp = font->buffer;
	
		_oled.showBmp(x,y,(BMP*)&bitmap);
		
		x += font->width;//字符x坐标递增	
		if((x + font->width) >= _screen_width)//如果剩余宽度不足以显示一个字符则回车或者退出
		{
			if(wordWrap == ENABLE){
				if(y > 7)return;//已经到底
				x = 0;y++;
				_oledSetPoint(x,y);//重新设置显示地址				
			}else
				return;
		}
	}
}

/**
 *在显存中写入字符串数据
 *@param unsigned int line 行数
 *		 char str 字符串
 *		 FONT_SIZE size	字体大小
 *  		 ALIGN allign 对齐方式，可以使用该枚举变量
 *@return void
 */
static void _oledWriteStringToDispalyMemoryEx(unsigned int line,char *str,FONT_SIZE font_size,ALIGN align)
{
	unsigned int x=0;
	int strLength = strlen((char*)str);//获得字符串长度
	FontAdapter *font = getFontAdatper(font_size ,str[0]);//获得字体规格
	
	switch(align)
	{
		case Align_Left:break;//左对齐
		case Align_Center:x = (_screen_width-(strLength*font->width))/2;break;//居中对齐
		case Align_Right:x = _screen_width-(strLength*font->width+1);break;//右对齐
		default:return;
	}_oledWriteStringToDispalyMemory(x,line,str,font_size,DISABLE);//显示字符串	
}

/**
 *在显存中写入位图，注意取模应采用 逐列、逆向阴码的形式
 *@param unsigned int x 横坐标值0->_screen_width-1
 *		 unsigned int y 纵坐标值0->_screen_hight-1
 *	     BMP *bmp 位图结构体
 *@return void
 */
static void _oledWriteBmpToDispalyMemory(unsigned int x,unsigned int y,BMP *bmp)
{
	long long cache = 0x00;
	unsigned int bytes = bmp->height/0x08;
	
	//采用逐列扫描写入的方式
	for(int i=0;i<bmp->width;i++)
	{
		unsigned int add = (unsigned int)bmp->bmp+i*bytes;//获取数据地址
		if(bytes > 0x04){
			unsigned int add_2 = (unsigned int)bmp->bmp+i*bytes+4;//获取数据地址2
			cache = (((long long)(*(unsigned int*)add_2))<<32)|(*(unsigned int*)add);//装载数据	
		}else		
			cache = *(unsigned int*)add;//装载数据					
			
		cache &= ((long long)1<<bmp->height)-1;//掩码操作						
			
		cache <<= y; //移位操作
		*(long long*)(&_oled_dis_buff[x+i][0]) |= cache;//写入显存
	}
}

/**
 *在显存中写入点
 *@param unsigned int x 横坐标值0->_screen_width-1
 *		 unsigned int y 纵坐标值0->_screen_hight-1
 *@return void
 */
static void _oledWriteDotToDispalyMemory(unsigned int x,unsigned int y)
{
	if((x > _screen_width) || (y > _screen_hight))return;//无效位置
	
	_oled_dis_buff[x][y/8] |= 0x01<<(y%8);
}

/**
 *在显存中写入线
 *@param unsigned int s_x 开始横坐标值0->_screen_width-1
 *		 unsigned int s_y 开始纵坐标值0->_screen_hight-1
 *		 unsigned int e_x 结束横坐标值0->_screen_width-1
 *		 unsigned int e_y 结束纵坐标值0->_screen_hight-1
 *@return void
 */
static void _oledWriteLineToDispalyMemory(unsigned int s_x,unsigned int s_y,unsigned int e_x,unsigned int e_y)
{
	float stepValue;
	
	//判断两轴之差的大小，决定谁作为主轴绘制
	if(fabs((int)s_x-(int)e_x) > fabs((int)s_y-(int)e_y))
	{
		//如果结束x坐标小于起始x坐标则调换
		if(e_x < s_x)
		{
			unsigned int temp = e_x;
			e_x = s_x;s_x = temp;
			temp = e_y;
			e_y = s_y;s_y = e_y;		
		}
		
		//从s_x开始遍历
		stepValue = (int)(e_y - s_y)/(float)((int)(e_x - s_x));//步进值
		for(int i=s_x;i<e_x;i++)
			_oledWriteDotToDispalyMemory(i,s_y + (i-s_x)*stepValue);	
	}else{
		//如果结束y坐标小于起始y坐标则调换
		if(e_y < s_y)
		{
			unsigned int temp = e_y;
			e_y = s_y;s_y = temp;
			temp = e_x;
			e_x = s_x;s_x = e_x;		
		}
		
		//从s_x开始遍历
		stepValue = (int)(e_x - s_x)/(float)((int)(e_y - s_y));//步进值
		for(int i=s_y;i<e_y;i++)
			_oledWriteDotToDispalyMemory(s_x + (i-s_y)*stepValue,i);
	}
}

/**
 *在显存中写入矩形
 *@param unsigned int x 开始横坐标值0->_screen_width-1
 *		 unsigned int y 开始纵坐标值0->_screen_hight-1
 *		 unsigned int length 长度0->_screen_width-1
 *		 unsigned int width 宽度0->_screen_hight-1
 *@return void
 */
static void _oledWriteRectangleToDispalyMemory(unsigned int x,unsigned int y,unsigned int length,unsigned int width)
{
	//矩形不过四条线
	width--;length--;
	
	_oledWriteLineToDispalyMemory(x,y,x,y+width);
	_oledWriteLineToDispalyMemory(x,y,x+length,y);	
	
	_oledWriteLineToDispalyMemory(x,y+width,x+length,y+width);
	_oledWriteLineToDispalyMemory(x+length,y+width+1,x+length,y);
}

/**
 *在显存中写入圆
 *@param unsigned int x 圆心横坐标值0->_screen_width-1
 *		 unsigned int y 圆心纵坐标值0->_screen_hight-1
 *		 unsigned int radii 半径0->_screen_width-1
 *@return void
 */
static void _oledWriteCircleToDispalyMemory(unsigned int x,unsigned int y,unsigned int radii)
{
	//遍历x坐标画点
	for(int i=(0-radii);i<(int)radii;i++)
	{
		//利用勾股定理求第三遍
		float temp = sqrt((radii*radii)-(i*i));
		_oledWriteDotToDispalyMemory(x+i,y-temp);
		_oledWriteDotToDispalyMemory(x+i,y+temp);
	}
	
	//遍历y坐标画点
	for(int i=(0-radii);i<(int)radii;i++)
	{
		//利用勾股定理求第三遍
		float temp = sqrt((radii*radii)-(i*i));
		_oledWriteDotToDispalyMemory(x-temp,y+i);
		_oledWriteDotToDispalyMemory(x+temp,y+i);
	}
}

/**
 *绘制波形到显存当中
 *@param unsigned char *data 数据指针
 * 		 unsigned int length 缓存区长度
 *@return 无
 */
static void _oledWriteWaveformToDispalyMemory(unsigned char *data,unsigned int length)
{
	for(int i=0;i<length;i++)
		_oledWriteLineToDispalyMemory(i,63-data[i],i+1,63-data[i+1]);
}

/**
 *获取默认控制结构体,这是本文件唯一的对外接口,获取结构体指针后，
 *进而可以操作整个OLED屏幕。
 *@param void
 *@return 默认唯一的控制结构体
 */
OLED* getOledHandle(void)
{
	//如果该指针已经被获取过了则退出
	if(_init_status != 0x00)
	{
		return &_oled;
	}else _init_status = 0x01;
	
	// 使能 GPIOB 和 AFIO 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	// 禁用 JTAG 功能，使能 SWD
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);		
	
	_oledGpioInit();
	_oledReset();
	_oledRegisterConfiguration();
		
	//为函数指针赋值
	_oled.cls  =_oledCls;
	_oled.clear = _oledClear;

	_oled.memorySynchronous = _oledUpDisBuffToDevice;
	
	_oled.showString = _oledWriteStringToDispalyMemory;
	_oled.showStringEx = _oledWriteStringToDispalyMemoryEx;
	_oled.showCharacter = _oledWriteCharacterToDispalyMemory;	
	
	_oled.showBmp = _oledWriteBmpToDispalyMemory;
	_oled.showDot = _oledWriteDotToDispalyMemory;
	_oled.showLine = _oledWriteLineToDispalyMemory;
	_oled.showCircle = _oledWriteCircleToDispalyMemory;
	_oled.showRectangle = _oledWriteRectangleToDispalyMemory;
	_oled.showWaveform = _oledWriteWaveformToDispalyMemory;
		
	return &_oled;
}
