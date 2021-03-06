/*-----------------------FIRMWARE PARA AQUECEDOR DO PIC-----------------------*/
/*
 * Arquivo:          main.c
 * Autor  :          Mateus Eus?bio
 * Microcontrolador: PIC16F877
 *
 * Created on 4 de Dezembro de 2020, 10:41
 * ?ltima atualiza??o: 24/07/21
 */

/*------------------------------Configura??es---------------------------------*/
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low Voltage In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#pragma config WRT = ON         // FLASH Program Memory Write Enable (Unprotected program memory may be written to by EECON control)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.



/*---------------------------DEFINI??ES DE PROJETO----------------------------*/
/*FREQU?NCIA DO CLOCK = 4MHz*/
#define _XTAL_FREQ 4000000 


/*--------------------------MAPEAMENTO DE HARDWARE----------------------------*/
//CONEX?ES DO M?DULO LCD
#define LCD_RS       PORTDbits.RD2
#define LCD_EN       PORTDbits.RD3
#define LCD_D4       PORTDbits.RD4
#define LCD_D5       PORTDbits.RD5
#define LCD_D6       PORTDbits.RD6
#define LCD_D7       PORTDbits.RD7      
#define LCD_RS_DIR   TRISD0
#define LCD_EN_DIR   TRISD1
#define LCD_D4_DIR   TRISD2
#define LCD_D5_DIR   TRISD3
#define LCD_D6_DIR   TRISD4
#define LCD_D7_DIR   TRISD5
//FIM DAS CONEX?ES DO M?DULO LCD


//PUSH-BOTTONS PARA INTERA??O DO USU?RIO
#define BT_Select       PORTBbits.RB0
#define BT_Ajuste       PORTBbits.RB1

//AS CHAVES DE N?VEL QUE INDICAM O N?VEL DA AGUA DO AQUECEDOR
#define Nivel_critico   PORTBbits.RB4

//SA?DA PARA O REL? QUE ATIVA O AQUECEDOR
#define Rele            PORTCbits.RC0


/*------------------------BIBLIOTECAS DO SISTEMA------------------------------*/
#include <xc.h>
#include <stdio.h>            // para o sprintf
#include "LCD_Lib.c"          // include LCD driver source file
#include <stdlib.h>
#include <math.h>               //round



/*--------------------------VARI?VEIS GLOBAIS---------------------------------*/
int TMR0_aux=0;
int Segundos=0;
int Minutos=0;
int Horas=0;
int Temperatura=0;
char LCD_Cstring[16];


void atualiza_ADC()
{
    ADCON0bits.GO_nDONE=1;
    while(ADCON0bits.GO_nDONE)
    {
        __delay_us(10);
    }
    return;
}

void imprime_tela_main()
{
    LCD_Goto(1, 1);                     // LCD va para Linha 1, Coluna 1
    sprintf(LCD_Cstring,"Ho.: %02d:%02d:%02d   ",Horas,Minutos,Segundos);//armazena a string em LCD_Cstring
    LCD_Print(LCD_Cstring);//imprime o rel?gio

    LCD_Goto(1, 2);// LCD va para Linha 2, Coluna 1
    sprintf(LCD_Cstring,"Te.: %02d\337c",Temperatura);
    LCD_Print(LCD_Cstring);//imprime o rel?gio
    
}

void imprime_tela_ajuste1(int temp)
{
   LCD_Goto(1, 1);                     // LCD va para Linha 1, Coluna 1
   sprintf(LCD_Cstring,"Temp. Des: %02d\337c  ",temp);//armazena a string em LCD_Cstring 
   LCD_Print(LCD_Cstring);//imprime LCD_Cstring no display
    
}

void imprime_tela_ajuste2()

{
   LCD_Goto(1, 1);                     // LCD va para Linha 1, Coluna 1
   sprintf(LCD_Cstring,"Horario Atual:");//armazena a string em LCD_Cstring 
   LCD_Print(LCD_Cstring);//imprime LCD_Cstring no display
   
   LCD_Goto(1, 2);                     // LCD va para Linha 2, Coluna 1
   sprintf(LCD_Cstring,"     %02d:%02d     ",Horas,Minutos);//armazena a string em LCD_Cstring 
   LCD_Print(LCD_Cstring);//imprime LCD_Cstring no display
    
    
}

void imprime_tela_ajuste3(int horas_min,int minutos_min)
{
   LCD_Goto(1, 1);                     // LCD va para Linha 1, Coluna 1
   sprintf(LCD_Cstring,"Start time:");//armazena a string em LCD_Cstring 
   LCD_Print(LCD_Cstring);//imprime LCD_Cstring no display
   
      LCD_Goto(1, 2);                     // LCD va para Linha 2, Coluna 1
   sprintf(LCD_Cstring,"     %02d:%02d     ",horas_min,minutos_min);//armazena a string em LCD_Cstring 
   LCD_Print(LCD_Cstring);//imprime LCD_Cstring no display
    
}
void imprime_tela_ajuste4(int horas_max,int minutos_max)
{
   LCD_Goto(1, 1);                     // LCD va para Linha 1, Coluna 1
   sprintf(LCD_Cstring,"Stop time:");//armazena a string em LCD_Cstring 
   LCD_Print(LCD_Cstring);//imprime LCD_Cstring no display
   
      LCD_Goto(1, 2);                     // LCD va para Linha 2, Coluna 1
   sprintf(LCD_Cstring,"     %02d:%02d     ",horas_max,minutos_max);//armazena a string em LCD_Cstring 
   LCD_Print(LCD_Cstring);//imprime LCD_Cstring no display
    
}

void ajustes(int *temp,int *hora_min,int *min_min,int *hora_max,int *min_max)
{
    LCD_Cmd(LCD_CLEAR);
    imprime_tela_ajuste1(*temp);
    __delay_ms(300);
    while(BT_Select!=0)
    {
        if(BT_Ajuste==0)
        {
            *temp=*temp+1;
            if (*temp>99)
            {
                *temp=0;
            }
            imprime_tela_ajuste1(*temp);
            __delay_ms(250);
        }
    }
    LCD_Cmd(LCD_CLEAR);
    imprime_tela_ajuste2();
    __delay_ms(300);
    while(BT_Select!=0)
    {
        if(BT_Ajuste==0)
        {
            Horas=Horas+1;
            if (Horas>23)
            {
                Horas=0;
            }
            imprime_tela_ajuste2();
            __delay_ms(250);
        }
    }
    __delay_ms(300);
    while(BT_Select!=0)
    {
        if(BT_Ajuste==0)
        {
            Minutos=Minutos+1;
            if (Minutos>59)
            {
                Minutos=0;
            }
            imprime_tela_ajuste2();
            __delay_ms(250);
        }
    }
    LCD_Cmd(LCD_CLEAR);
    imprime_tela_ajuste3(*hora_min,*min_min);
    __delay_ms(300);
    while(BT_Select!=0)
    {
        

        if(BT_Ajuste==0)
        {
            *hora_min=*hora_min+1;
            if (*hora_min>23)
            {
                *hora_min=0;
            }
            imprime_tela_ajuste3(*hora_min,*min_min);
            __delay_ms(250);
        }

    }
    __delay_ms(300);
    while(BT_Select!=0)
    {
        
        if(BT_Ajuste==0)
        {
            *min_min=*min_min+1;
            if (*min_min>59)
            {
                *min_min=0;
            }
            imprime_tela_ajuste3(*hora_min,*min_min);
            __delay_ms(250);
        }
    }
    LCD_Cmd(LCD_CLEAR);
    imprime_tela_ajuste4(*hora_max,*min_max);
    __delay_ms(250);
    while(BT_Select!=0)
    {
        
        
        if(BT_Ajuste==0)
        {
            *hora_max=*hora_max+1;
            if (*hora_max>23)
            {
                *hora_max=0;
            }
            imprime_tela_ajuste4(*hora_max,*min_max);
            __delay_ms(250);
        }   
    }
    __delay_ms(300);
    while(BT_Select!=0)
    {
        if(BT_Ajuste==0)
        {
            *min_max=*min_max+1;
            if (*min_max>59)
            {
                *min_max=0;
            }
            imprime_tela_ajuste4(*hora_max,*min_max);
            __delay_ms(250);
        }
    }
    __delay_ms(250);
    
    LCD_Cmd(LCD_CLEAR);
      
    return;   
}

int controle_aquecedor(int temp,int hora_min,int min_min,int hora_max,int min_max)
{     
    if(hora_min<hora_max)
    {
        if(Horas>hora_min)
        {
            if(Horas<hora_max)
            {
                return 1;
            }
            else if(Horas==hora_max)
            {
                if (Minutos<min_max)
                {
                      return 1;
                }
                return 0;
            }
            else 
            {
                    return 0;
            }
                    
        }
        else if(Horas==hora_min)
        {
            if (Minutos>min_min)
            {
                if(Horas<hora_max)
                {
                    return 1;
                }
                else if(Horas==hora_max)
                {
                    if (Minutos<min_max)
                    {
                        return 1;
                    }
                    return 0;
                }
                else 
                {
                    return 0;
                }
            }
        }
        else
        {
            return 0;
        }         
    }
    else
    {
        if (Horas>hora_min)
        {
            return 1;
        }
        else if(Horas==hora_min)
        {
            if (Minutos>min_min)
            {
                return 1;
            }
            else 
            {
                    return 0;
            }
        }
        else
        {
            if(Horas<hora_max)
            {
                return 1;
            }
            else if (Horas==hora_max)
            {
                if (Minutos<min_min)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
    }        
}

void __interrupt () my_isr_routine (void)
{
    if (INTCONbits.T0IF)//verifica o bit da interrup??o do timer0
    {
       // 500 ms = 64us(PRESCALLER) X 62(TMR0) X 125(TMR0_AUX) = 500ms
        TMR0=256-62;
        TMR0_aux++;
        
        
        if(TMR0_aux>125)
        {
            TMR0_aux=0;
            Segundos++;
            
            if(Segundos>59)
            {
                Segundos=0;
                Minutos++;
                
                if(Minutos>59)
                {
                    Minutos=0;
                    Horas++;
                    if(Horas>23)
                        Horas=0;
                }
                 
            }
             
        }
    INTCONbits.T0IF=0;//limpa o bit da interrup??o do timer0
    }
    
    return;
}



void main(void) 
{
    OPTION_REG  =0b01010101;//prescaller 64 para timer0, fonte do timer0 clock interno, Portb weak Pull-ups ativados
    INTCON      =0b10100000;//interrup??es globais e do timer0 somente
    TRISA       =0b00000001;//DEFINE RA0 COMO ENTRADA ***RAO ? A ENTRADA DO ADC
    TRISB       =0b00010011;//INPUTS RB0,RB1 E RB4 
    TRISC       =0b00000000;//portc outputs
    TRISD       =0b00000000;//portd outputs
    TRISE       =0b00000000;//porte outputs
    ADCON0      =0b00000001;//clock concersion=fosc/2,canal=RA0,go/done=0,modulo AD ligado
    ADCON1      =0b10001110;//resultado right jsutified(ADDRESH),somente RA0 pin anal?gico, refer?ncia=Vdd e Vss
    
    LCD_Begin();       // initialize LCD module    
   
    int temp     =25;
    int hora_min =0;
    int min_min  =0;
    int hora_max =0;
    int min_max  =0;
    while(1)
    {
        atualiza_ADC();
        
        //Temperatura=(int)ADRESH/2.56;
        Temperatura=round(((ADRESL + (ADRESH *256))/2.046)-50);
        
        
        imprime_tela_main();
        if(BT_Select==0)
            ajustes(&temp,&hora_min,&min_min,&hora_max,&min_max);
        
        if(Nivel_critico==0)
        {
            if (Temperatura<(temp-5))
                Rele = controle_aquecedor(temp,hora_min,min_min,hora_max,min_max);
            if (Temperatura>(temp+5))
                Rele = 0;
        }
        else
            Rele=0;
    }
    
    return;
}