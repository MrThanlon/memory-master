/*
因为有多个动态显示的操作，
所以不使用延迟函数
*/
#include "memmaster.h"

#include<stdlib.h>

void delay(u16 i){
    while(i--){
        u16 a=200;
        while(a--);
    }
}

void main(void){
    
    //u16 last_secs=secs20;
    //u16 tmpp=0;
    //u16 ttp=0;
    //const u16 mods=50;
    
    init();
    //P1=0xff;

	/*
    EA=0;
    while(P2==0x0f);
    EA=1;
    */
    
    
    //srand(10232);
    //display_reload_num(3233,2);
    while(1){
        //display_cycle();
        cycle();

        EA=0;
        t_secs20=secs20;
        t_min20=min20;
        EA=1;

        runtime();
        
        /*
        
        EA=0;
        tmpp = secs20;
        EA=1;
        
        if(tmpp%50==0 && tmpp!=last_secs)
        {
            
            //if(secs20%50!=0)
                //continue;
            
            P1=~P1;
            
            last_secs=tmpp;
        }
        
        */
    }
}