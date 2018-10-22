#include "memmaster.h"

//全局初始化
void init(){
    UART_init();
    display_init();
    timer_init();
    led_init();
}

void cycle(){
    //display_cycle();
    //led_cycle();

    //尝试数码管和点阵交互循环

    //数码管位1
    P1=0xff;
    P35=1;
    //P34=0;
    //P33=0;
    P32=0;
    P1=c_dp[0];
    P0=c_led[0];//点阵第一行
    P1=0xff;//关闭数码管

    P35=0;
    P34=1;
    //P33=0;
    //P32=0;
    P1=c_dp[1];
    P0=c_led[1];
    P1=0xff;

    //P35=0;
    P34=0;
    P33=1;
    //P32=0;
    P1=c_dp[2];
    P0=c_led[2];
    P1=0xff;

    //P35=0;
    //P34=0;
    P33=0;
    P32=1;
    P1=c_dp[3];
    P0=c_led[3];
    P1=0xff;

    key_scan();

    P0=0x00;//点阵关闭
}

//数码管段选编码，共阳
const u8 display_num[]={
    0xc0,//0=abcd,ef=,gh
    0xf9,//1=bc,=ad,efgh
    0xa4,//2=abd,eg=c,fh
    0xb0,//3=abcd,g=,efh
    0x99,//4=bc,fg=ad,eh
    0x92,//5=acd,fg=b,eh
    0x82,//6=acd,efg=b,h
    0xf8,//7=abc,=d,efgh
    0x80,//8=abcd,efg=,h
    0x90,//9=abcd,fg=,eh

    0x40,//0=abcd,efh=,g
    0x79,//1=bc,=adh,efg
    0x24,//2=abd,egh=c,f
    0x30,//3=abcd,gh=,ef
    0x19,//4=bc,fgh=ad,e
    0x12,//5=acd,fgh=b,e
    0x02,//6=acd,efgh=b,
    0x78,//7=abc,h=d,efg
    0x00,//8=abcd,efgh=,
    0x10,//9=abcd,fgh=,e

    0xff,//熄灯,序号20

    //0x92//S=acd,fg=b,eh
    //用于特殊功能的 F,A,L,G,o,d
    0x8e,//F=a,efg,=bcd,h
    0x88,//A=abc,efg=d,h
    0xc7,//L=d,ef=abc,gh
    0xc2,//G=acd,ef=b,gh
    0xa3,//o=cd,eg=ab,fh
    0xa1//d=bcd,eg=a,fh

};

//数码管段选，共阴，目前没用
/*
const u8 display_num_neg[]={
    0x3f,//0=abcd,ef
    0x06,//1=bc,
    0x5b,//2=abd,eg
    0x4f,//3=abcd,g
    0x66,//4=bc,fg
    0x6d,//5=acd,fg
    0x7d,//6=acd,efg
    0x08,//7=abc,
    0x7f,//8=abcd,efg
    0x6f,//9=abcd,fg

    0xbf,
    0x86,
    0xdb,
    0xcf,
    0xe6,
    0xed,
    0xfd,
    0x88,
    0xff,
    0xef,

    0x00
};
*/


void display_init(){
    display_reload(0,0,0,0);
}

//cycle数码管变量
u8 c_dp[4]={0xff,0xff,0xff,0xff};

//四位数码管动态显示
void display_cycle(){
    P1=0xff;
    P35=1;
    //P34=0;
    //P33=0;
    P32=0;
    P1=c_dp[0];
    P1=0xff;

    P35=0;
    P34=1;
    //P33=0;
    //P32=0;
    P1=c_dp[1];
    P1=0xff;

    //P35=0;
    P34=0;
    P33=1;
    //P32=0;
    P1=c_dp[2];
    P1=0xff;

    //P35=0;
    //P34=0;
    P33=0;
    P32=1;
    P1=c_dp[3];
    P1=0xff;
}

//cycle点阵变量
u8 c_led[]={0x08,0x04,0x02,0x01};

void led_cycle(){
    P0=c_led[0];
    P0=c_led[1];
    P0=c_led[2];
    P0=c_led[3];
    P0=0x00;
}

//点阵初始化
void led_init(){
    led_reload(0);
}

//计时器初始化
//计时器使用T0，16位方式
//计时间隔为20ms，12分频率，+1用时为1us
//16位共65535，20ms=20000us，则初始值为45536
void timer_init(){
    TMOD=0x31;//关闭定时器1，定时器0使用16位模式
    EA=1;
    ET0=1;
    //TH0=177;
    //TL0=224;
    TH0=178;
    TL0=10;//修正后
    TR0=1;
}

//20ms次数，60000归0
//可以记录1200000ms，20分钟
u16 secs20=0;

//20min的次数，不精确，原理同上
//理论上可记录21845小时，大约2.4年
//主要用于产生随机数
u16 min20=0;

//上面两个的复制
u16 t_secs20=0;
u16 t_min20=0;

void Timer0() interrupt 1 {
    //TH0=177;
    TH0=178;
    //TL0=224;
    TL0=10;//测试修正

    secs20++;
    if(secs20==60001){
        secs20=0;
        min20++;
    }
}

//键盘初始化
void key_init(){
    P2=0x0f;
}

//定义几个全局变量来使用
//按下状态，
//0=没有，1=按下，消抖等待阶段，
//2=按下，计算按键阶段，3=按下，等待放开阶段，
//4=放开，消抖阶段
//4结束后回到0
u8 press_status=0;

//按下的按键，0=没有或者未知，1-16
u8 press_key=0;

//完整按下次数计数
u16 press_count=0;

//用于计时
u16 clock0=0;
//u16 clock1=0;

//用于计算按下的按键的列位置，5=按了多个按键，6=没按
//正常情况有7,11,13,14
u8 key_values[]={
    5,5,5,5,5,5,5,3,5,5,5,2,5,1,0,6
};
/*
键盘扫描值可以分别为以下数值
但是打表的话得256个数组了，得不偿失
0x7e
0x7d
0x7b
0x77

0xbe
0xbd
0xbb
0xb7

0xde
0xdd
0xdb
0xd7

0xee
0xed
0xeb
0xe7
*/

u8 tmp=0;
u8 k1=0;
u8 k2=0;

//键盘扫描流程
void key_scan(){
    P2=0x0f;
    if(P2==0x0f&&press_status==0){//没有按键按下
        press_key=0;
        return;
    }
    //此时有按下
    switch(press_status){
        case 0:{
            press_status=1;
            //secs20=0;
            clock0=t_secs20;
            press_key=0;
            return;//回到动态流程，等待消抖结束
        }
        case 1:{
            if(t_secs20!=clock0){
                //20ms过去，结束消抖
                press_status=2;
                press_key=0;
                return;
            }
            return;
        }
        case 2:{
            //计算按下的按键
            P2=0x0f;
            tmp=P2;
            k1=key_values[tmp];
            if(k1==5){
                //按下多个按键的处理
                //认为没按下吧
                press_status=0;
                press_key=0;
                return;
            }
            if(k1==6){
                //放开了按键的处理（太强了吧，一瞬间啊）
                //那就认为是没按下吧23333
                press_status=0;
                return;
            }
            P2=0xf0;
            tmp=P2>>4;
            k2=key_values[tmp];
            k2=3-k2;
            if(k2==5){
                //同上
                press_status=0;
                return;
            }
            if(k2==6){
                //同上
                press_status=0;
                return;
            }
            //此时可以确定只按了一个按键
            press_key=(k2<<2)+k1+1;
            press_count++;
            press_status=3;
            return;

            /*
            //另一种写法
            P2=0x0f;
            tmp=P2;
            P2=0xf0;
            tmp+=P2;
            switch(tmp){
                case 0x7e:press_key=1;break;
                case 0x7d:press_key=2;break;
                case 0x7b:press_key=3;break;
                case 0x77:press_key=4;break;

                case 0xbe:press_key=5;break;
                case 0xbd:press_key=6;break;
                case 0xbb:press_key=7;break;
                case 0xb7:press_key=8;break;

                case 0xde:press_key=9;break;
                case 0xdd:press_key=10;break;
                case 0xdb:press_key=11;break;
                case 0xd7:press_key=12;break;

                case 0xee:press_key=13;break;
                case 0xed:press_key=14;break;
                case 0xeb:press_key=15;break;
                case 0xe7:press_key=16;break;

                default:press_key=0;press_status=0;return;
            }
            press_count++;
            press_status=3;
            */
        }
        case 3:{
            //等待放开
            P2=0xf0;
            if(P2!=0xf0)//未放开
                return;
            //此时放开
            press_status=4;
            clock0=t_secs20;
            return;
        }
        case 4:{
            if(t_secs20==clock0)
                return;
            press_status=0;
            //press_count++;
            return;
        }
    }
    return;
}


//串口初始化，波特率9600
void UART_init()
{
    SCON = 0x50;        //串口方式1
    TMOD = 0x20;        // 定时器使用方式2自动重载
    TH1 = 0xFD;    //9600波特率对应的预设数，定时器方式2下，TH1=TL1
    TL1 = 0xFD;
    TR1 = 1;//开启定时器，开始产生波特率
}

//**********运行时函数**********
code const dp_time=30;//显示灯时间，单位：0.1秒
code const ctr_time=80;//单次游戏时间，单位同上

code const dp_st=3;//显示灯时间，单位：1秒
code const ctr_st=8;//单次游戏时间，单位同上

//处理逻辑操作
void runtime(){
    static u16 last_secs;//用于计时
    static u16 last_min;//同上
    //static u16 dp;
    static u16 led_nums;//led值
    static u16 led_nums2;//可以只用一个，就是略麻烦
    //游戏状态，0=未开始游戏，1=显示灯倒计时，2=操作倒计时
    static u8 game_status;

    static u8 counts;//倒计时初值

    static u8 hards;//难度等级，灯数4-8

    switch (game_status){
        case 0:{
            hards=hards?hards:4;//难度初始化，应该会有更好的写法
            if(press_key){//按下任意键游戏开始
                led_nums=led_random_gen(hards);//生成灯数
                led_reload(led_nums);
                
                //counts=dp_time-(hards-4)*4;//倒计时初值
                counts=dp_st-(hards-4)/3;
                
                //display_reload_num(counts,2);
                display_reload(20,20,20,counts);

                press_key=0;
                game_status=1;
            }
            break;
        }
        case 1:{
            //启动倒计时
            //if(t_secs20>=last_secs+5||t_min20==last_min+1){//精度0.1s
            if(t_secs20>=last_secs+50||t_min20==last_min+1){
                last_secs=t_secs20;
                last_min=t_min20;

                counts--;
                if(counts==0){
                    //倒计时结束
                    led_reload(0);//关灯
                    led_nums2=0;

                    //counts=ctr_time-(hards-4)*5;//精度0.1s
                    counts=ctr_st-(hards-4);

                    game_status=2;
                    
                }
                //display_reload_num(counts,2);//精度0.1s
                display_reload(20,20,20,counts);
            }
            break;
        }
        case 2:{
            //游戏操作阶段
            //操作倒计时
            //if(t_secs20>=last_secs+5||t_min20==last_min+1){//精度0.1s
            if(t_secs20>=last_secs+50||t_min20==last_min+1){
                last_secs=t_secs20;
                last_min=t_min20;

                counts--;
                if(counts==0){
                    //游戏失败
                    led_reload(38505);//显示叉叉

                    //display_reload_num(0,2);
                    display_reload(21,22,1,23);//显示FA1L

                    hards=hards==4?hards:hards-1;
                    game_status=0;
                    return;
                }
                //display_reload_num(counts,2);//精度0.1s
                display_reload(20,20,20,counts);
            }

            //操作逻辑
            if(press_key){
                //按下操作
                led_mirror(press_key,&led_nums2);
                led_reload(led_nums2);
                if(led_nums2==led_nums){
                    //游戏成功，全亮
                    led_reload(0xffff);
                    
                    //display_reload_num(ctr_time-(hards-4)*5-counts,2);//显示用时，0.1
                    //display_reload(20,20,ctr_st-(hards-4)-counts,21);//显示用时
                    display_reload(24,25,25,26);

                    hards=hards==8?hards:hards+1;
                    game_status=0;
                }
                press_key=0;
            }

            break;
        }
        default:game_status=0;break;//应该不会有这种情况吧？
    }
    

    /*
    if(press_key){
        //按下
        led_mirror(press_key,&led_nums);
        led_reload(led_nums);

        if(press_count>99)
            press_count=0;

        display_reload_num(press_key+press_count*100,0);

        press_key=0;
    }
    */
}

//*********以下为可操作函数**********

//发送一个字符
void UART_send_byte(u8 dat)
{
    SBUF = dat;       //把数据放到SBUF中
    while (TI == 0);//未发送完毕就等待
    TI = 0;    //发送完毕后，要把TI重新置0
}

//发送一个字符串
void UART_send_string(u8* buf)
{
    while (*buf != '\0')
    {
        UART_send_byte(*buf++);
    }
}

//数码管重装
/*
接收4个参数分别表示四位，
大于9表示带点，数值等于减去9，
20表示熄灭

例如
2=2
12=2.
10=0.
20=熄灭
*/
void display_reload(u8 a,u8 b,u8 c,u8 d){
    c_dp[0]=display_num[a];
    c_dp[1]=display_num[b];
    c_dp[2]=display_num[c];
    c_dp[3]=display_num[d];
}

//数码管重装，直接输入四位数整数就行加浮点位
void display_reload_num(u16 dp_num,u8 dot_num){
    u8 a=dp_num/1000;
    u8 b=dp_num%1000/100;
    u8 c=dp_num%100/10;
    u8 d=dp_num%10;

    /*
    a=a?a:20;
    b=b?b:20;
    c=c?c:20;
    d=d?d:20;
    */

    switch(dot_num){
        case 4:a+=10;break;
        case 3:b+=10;break;
        case 2:c+=10;break;
        case 1:
        default:break;
    }

    display_reload(a,b,c,d);
}

//点阵重装
//参数是一个16位二进制的整数，分别对应16个灯
//可以认为是4个十六进制数，每个16进制数编码点亮的灯
//每行灯中，第一个灯为1，第二个为2，第三个为4，第四个为8
//点亮哪个灯就把数值加上去
//比如点亮(2,1)和(2,3)，则数值为16*(1+4)=80
//比如点亮(1,4),(2,2),(4,3)，则数值为8+16*(2)+4096*(4)=16424
void led_reload(u16 led_num){
    //位操作法
    c_led[0]=0x08+ 0x10*(~(led_num&0x000f)>>0);//第一行
    c_led[1]=0x04+ 0x10*(~(led_num&0x00f0)>>4);//第二行
    c_led[2]=0x02+ 0x10*(~(led_num&0x0f00)>>8);
    c_led[3]=0x01+ 0x10*(~(led_num&0xf000)>>12);
}

//这个函数没有调试过，不太清楚能不能用
u16 led_load(){
    return 
    ((~c_led[0]&0xf0)>>8)+
    (~c_led[1]&0xf0)+
    ((~c_led[2]&0xf0)>>4)+
    ((~c_led[3]&0xf0)>>8);
}

//填充点阵的计算，方便使用，尽量别用，计算值放在第三个参数
//第一个参数为第几个灯，1-16，
//第二个参数为灯状态，亮=1，灭=0，其他值会未知结果
//第三个参数为当前led编码值，需要加取地址符，不能使用常量
//比如  led_insert(12,1,led_num);
void led_insert(u8 number,u8 status,u16* led_num){
    //某一位与1异或的时候刚好发生反转，与0异或的时候不变
    //我感觉还可以优化一下的说
    /*
    if(status){
        led_num=led_num^(1<<(number-1))|led_num;
    }
    else{
        led_num=led_num^(1<<(number-1))&led_num;
    }
    */

    //优化完毕，一步操作
    *led_num=*led_num-(*led_num&(1<<(number-1)))+(status<<(number-1));
}

//反转点阵中的number位
void led_mirror(u8 number,u16* led_num){
    *led_num=*led_num^(1<<(number-1));
}

//随机生成一定数量灯数的点阵值
//参数为亮灯的数量
u16 led_random_gen(u8 leds){
    u16 led_num=0;
    u8 i=0;

    u8 order[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    u8 sels=0;

    srand(t_secs20+t_min20);//随便啦，我当然知道不是实际时间
    
    for(i=16;i>16-leds;i--){
        sels=rand()%i;
        led_insert(order[sels],1,&led_num);
        order[sels]=order[i-1];
    }
    return led_num;
}
