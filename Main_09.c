/* UNIVERSIDAD DEL VALLE DE GUATEMALA
 * DEPARTAMENTO DE INGENIERIA ELCTRONICA & MECATRONICA
 * CURSO DE PROGRAMACION DE MICROCONTROLADORES
 * LABORATORIO No.9
 * 
 * File:   main9.c
 * Author: Selvin E. Peralta Cifuentes 
 *
 * Created on 27 de abril de 2021, 09:21 AM
 * Ultima Actualizacion:   
 */
//-----------------------------------------------------------------------------------------------------------------------------------
// CONFIG1
//-----------------------------------------------------------------------------------------------------------------------------------
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = ON      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)
//-----------------------------------------------------------------------------------------------------------------------------------
// CONFIG2
//-----------------------------------------------------------------------------------------------------------------------------------
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#define _XTAL_FREQ 4000000     // Frecuencia del osilador
//__________________________
// PROTOTIPOS DE FUNCIONES
void Setup(void);       // Llamamos las configuraciones de los pines 
//__________________________
// FUNCION DE INTERRUPCIONES
void __interrupt() isr(void){
//----------------------------- INTERRUPCION DEL ADC ----------------------------
    if(PIR1bits.ADIF){           //Verificamos si la bandera del IoCH es 1 
       //Verificamos si el pin esta precionado 
        if(ADCON0bits.CHS == 0){ 
            CCPR1L   = (ADRESH>>1)+124 ;         //Aumentamos el Puerto C cuando se preciona el boton
            CCP1CONbits.DC1B1 = (ADRESL>>6);
            CCP1CONbits.DC1B0 = (ADRESL>>7);
          }
        else{       //Verificamos si el pin esta precionado 
            CCPR2L   = (ADRESH>>1)+124 ;         //Aumentamos el Puerto C cuando se preciona el boton
            CCP2CONbits.DC2B1 = (ADRESL>>6);
            CCP2CONbits.DC2B0 = (ADRESL>>7);
        }
        PIR1bits.ADIF = 0;
      }
}
//__________________________
// FUNCION PRINCIPAL (MAIN)
//__________________________
void main(void){
    Setup();
     while(1){
       if(ADCON0bits.GO == 0){
            if (ADCON0bits.CHS == 0){
             ADCON0bits.CHS = 1;
            }
            else{
             ADCON0bits.CHS = 0;
            }
          __delay_us(50);
          ADCON0bits.GO = 1;
       }
    }
}

//__________________________
// FUNCION DE CONFIGURACION

void Setup(void){
//------------------- CONFIGURACION DE ENTRADAS Y SALIDAS ----------------------
    ANSEL  = 0b00000011;    // Configuracion a pines Digitale 
    ANSELH = 0;
 //Configuramos que pines queremos como salidas o entradas 
    TRISC = 0x00;
    TRISA = 0x03;
 //Limpiamos todos los puertes 
    PORTC = 0x00;
    PORTA = 0x00;
//---------------------- CONFIGURACION DE RELOJ A 8MHZ -------------------------
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS   = 1;
 //---------------------- CONFIGURACION DE ADC ------- -------------------------
    ADCON1bits.ADFM  = 0;    // Justificacion a la izquierda
    ADCON1bits.VCFG0 = 0;    //   Vref en VSS y VDD
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS  = 0b10;   //Configuracion del reloj del modulo 
    ADCON0bits.CHS   = 0;   //Seleccionamos el canal donde querramos empezar
    __delay_us(100);
    ADCON0bits.ADON  = 1;   //Encendemos el modulo ADC
    __delay_us(50);
//---------------------- CONFIGURACION DE PWM ------- -------------------------
    TRISCbits.TRISC2 = 1;    // RC2/CCP1 encendido
    TRISCbits.TRISC1 = 1;    // RC1/CCP2 encendido
    
    
    PR2 = 255;               // Configurando el periodo
    CCP1CONbits.P1M = 0;     // Configurar el modo PWM
    
    CCP1CONbits.CCP1M = 0b1100;
    CCP2CONbits.CCP2M = 0b1100;
    
    CCPR1L = 0x00;
    CCPR2L = 0x00;
    
    CCP1CONbits.DC1B = 0;
    CCP2CONbits.DC2B0 = 0;
    CCP2CONbits.DC2B1 = 0;
    
    
    PIR1bits.TMR2IF  =  0;
    T2CONbits.T2CKPS =  0b11;
    T2CONbits.TMR2ON =  1;
    
    while(PIR1bits.TMR2IF  ==  0);
    PIR1bits.TMR2IF  =  0;
    
    TRISCbits.TRISC2 = 0; 
    TRISCbits.TRISC1 = 0; 
//-------------------------- CONFIGURACION DEL TMRO ----------------------------
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA  = 0;
    OPTION_REGbits.PS2  = 1;
    OPTION_REGbits.PS1  = 1;
    OPTION_REGbits.PS0  = 1;
//---------------------- CONFIGURACION DE INTERRUPCIONES -----------------------
    INTCONbits.GIE  = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE   = 1;
    PIR1bits.ADIF   = 0;
}


