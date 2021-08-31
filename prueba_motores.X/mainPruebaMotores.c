/*------------------------------------------------------------------------------
Archivo: mainsproject.s
Microcontrolador: PIC16F887
Autor: Pablo Herrarte y Andy Bonilla
Compilador: pic-as (v2.30), MPLABX v5.40
    
Programa: pic para motores de proyecto 1 de Electronica Digital 2
Hardware: PIC16F887
    
Creado: 15 de agosto de 2021    
Descripcion: 
------------------------------------------------------------------------------*/

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT   //configuracion de oscilador interno
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/*-----------------------------------------------------------------------------
 ----------------------------LIBRERIAS-----------------------------------------
 -----------------------------------------------------------------------------*/
#include <stdint.h>
#include <pic16f887.h>
#include <xc.h>
#include <proc/pic16f887.h>
#include "Osc_config.h"
//#include "ADC_CONFIG.h"
//#include "I2C.h"
/*-----------------------------------------------------------------------------
 ----------------------- VARIABLES A IMPLEMTENTAR------------------------------
 -----------------------------------------------------------------------------*/

//-------DIRECTIVAS DEL COMPILADOR
#define _XTAL_FREQ 4000000
//-------VARIABLES DE PROGRAMA
unsigned char cuenta, cuenta_total;
unsigned char antirrebote, botonazos;
/*-----------------------------------------------------------------------------
 ------------------------ PROTOTIPOS DE FUNCIONES ------------------------------
 -----------------------------------------------------------------------------*/
void setup(void);

/*-----------------------------------------------------------------------------
 --------------------------- INTERRUPCIONES -----------------------------------
 -----------------------------------------------------------------------------*/
void __interrupt() isr(void) //funcion de interrupciones
{
    if (INTCONbits.T0IF)
    {
        if (botonazos==1)  //variar oscilacion de 18ms
        {
            PORTEbits.RE0=1;
            __delay_ms(2);
            PORTEbits.RE0=0;
            TMR0 = 70;  
            INTCONbits.T0IF = 0;
        }
        if (botonazos==2)    //variar oscilacion de 18.5ms
        {
            PORTEbits.RE0=1;
            __delay_ms(1.5);
            PORTEbits.RE0=0;
            TMR0 = 72;
            INTCONbits.T0IF=0;
        }   
        T0IF=0;
    }
    //-------INTERRUPCION POR BOTONAZO
    if (INTCONbits.RBIF)
    {
        switch(PORTB)
        {
            default:
                antirrebote=0;
                break;
            case(0b11111101):
                antirrebote=1;
                break;
        }
        INTCONbits.RBIF=0;
    }
}
/*-----------------------------------------------------------------------------
 ----------------------------- MAIN LOOP --------------------------------------
 -----------------------------------------------------------------------------*/
void main(void)
{
    setup();
    
    while(1)
    {
        //-------ANTIRREBOTES DE BOTON 
        if (antirrebote==1 && PORTBbits.RB1==0  )
        {
            botonazos++;
            antirrebote=0;
            if(botonazos>2)
                botonazos=0;
        }

        PORTD=botonazos;
        
        
    }
       
}
/*-----------------------------------------------------------------------------
 ---------------------------------- SET UP -----------------------------------
 -----------------------------------------------------------------------------*/
void setup(void)
{
    //-------CONFIGURACION ENTRADAS ANALOGICAS
    ANSEL=0;
    ANSELH=0;
    //-------CONFIGURACION IN/OUT
    TRISBbits.TRISB1=1;                 //entrada boton prueba
    TRISD=0;
    TRISEbits.TRISE0=0;                 //salida para PWM de servo
    
    //-------LIMPIEZA DE PUERTOS
    PORTB=0;
    PORTD=0;
    PORTE=0;
    //-------CONFIGURACION DE RELOJ A 8MHz
    osc_config(4);
    //-------CONFIGURACION DE TIMER0
    OPTION_REGbits.T0CS = 0;    //Uso reloj interno
    OPTION_REGbits.PSA = 0;     //Uso pre-escaler
    OPTION_REGbits.PS = 0b111;  //PS = 111 / 1:256
    TMR0 = 78;                  //Reinicio del timmer
    //-------CONFIGURACION DE WPUB
    OPTION_REGbits.nRBPU=0;             //se activan WPUB
    WPUBbits.WPUB1=1;                   //RB0, boton prueba
    
    //-------CONFIGURACION DE INTERRUPCIONES
    INTCONbits.GIE=1;                   //se habilita interrupciones globales
    INTCONbits.PEIE = 1;                //habilitan interrupciones por perifericos
    INTCONbits.T0IE=1;                  //se habilita interrupcion timer0
    INTCONbits.T0IF=0;                  //se habilita interrupcion timer0
    INTCONbits.RBIE=1;                  //se  habilita IntOnChange B
    INTCONbits.RBIF=0;                  //se  apaga bandera IntOnChange B
    IOCBbits.IOCB1=1;                   //habilita IOCB RB0
}
/*-----------------------------------------------------------------------------
 --------------------------------- FUNCIONES ----------------------------------
 -----------------------------------------------------------------------------*/
