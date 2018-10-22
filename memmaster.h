/*header file*/
#ifndef __STC89C5xRC_RDP_H_
#include <STC89C5xRC.H>
#endif

#ifndef __STDLIB_H__
#include<stdlib.h>
#endif

#define i8 char
#define i16 int
#define u8 unsigned char
#define u16 unsigned int

//全局初始化
void init();

//动态扫描循环
void cycle();

//数码管初始化
void display_init();

//cycle数码管动态显示
void display_cycle();

//cycle，led动态显示
void led_cycle();

//点阵初始化
void led_init();

//计时器初始化
void timer_init();

//按键初始化，好像没什么必要
void key_init();

//键盘扫描
void key_scan();

//串口初始化，波特率9600
void UART_init();

//发送一个字符
void UART_send_byte(u8);

//发送一个字符串
void UART_send_string(u8*);

//*********元件操作*********

//数码管重装
void display_reload(u8,u8,u8,u8);

//数码管重装，整数
void display_reload_num(u16,u8);

//点阵重装
void led_reload(u16);

//点阵值读取
u16 led_load();

//填充点阵的计算
void led_insert(u8,u8,u16*);

//反转点阵中的某一位
void led_mirror(u8,u16*);

//随机生成一定数量灯数的点阵值
u16 led_random_gen(u8);

//*******外部变量声明***********
extern const u8 display_num[20];
extern const u8 display_num_dot[10];
extern u8 c_dp[4];
extern u8 c_led[4];
extern u16 secs20;
extern u16 min20;
extern u16 t_secs20;
extern u16 t_min20;
extern u8 press_status;
extern u8 press_key;
extern u16 press_count;
extern u16 clock0;
extern u8 key_value[16];
extern u8 tmp;
extern u8 k1;
extern u8 k2;

//*******用户自定义外部变量********
//extern u16 led_num;
