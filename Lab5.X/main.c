/*
 * File:   Lab 2
 * Author: Pablo Herrarte
 * Curso: Electrónica Digital 2 
 * Fecha: 27/07/2021
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

/**************************PALABRAS DE CONFIGURACIÓN**************************/
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
/******************************LIBRERÍAS***************************************/
#include <xc.h>
#include <stdint.h>
#include <pic16f887.h>
#include "ASCII.h"              //NUM ASCII
#include "USART.h"              //CONFIGURACIÓN USART
#define _XTAL_FREQ 8000000      //Frecuencia a trabajar
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/****************************VARIABLES***************************************/
char Cen1 = 0;  //Dígito de contador
char Dec1 = 0;
char Un1 = 0;   
char AC1 = 0;
char AD1 = 0;
char AU1 = 0;
char Cen2 = 0;  //Dígito de contador
char Dec2 = 0;
char Un2 = 0;   
char AC2 = 0;
char AD2 = 0;
char AU2 = 0;
uint8_t T = 1;  //Toggle para controlar dos señales analógicas
uint8_t toggleTX = 0;   //contador para mandar datos a la terminal virtual
uint8_t signo = 0;      //Control del signo para transmitir datos desde la terminal virtual
uint8_t sum = 0;        //Suma del contador
uint8_t res = 0;        //Resta del contador
uint8_t CONT = 0;       //Contador
uint8_t AR1 = 0; //Variables para el antirrebote 
uint8_t AR2 = 0;
uint8_t AR3 = 0;
uint8_t AR4 = 0;
char A = 0;
char B = 0;
char C = 0;
char ABC = 0;
char A1 = 0;
char A2 = 0;
char A3 = 0;
uint8_t cc = 0;
uint8_t BOTON = 0;

//PROTOTIPO FUNCIONES
void Setup(void);   //Setup
void LECT1(void);   //Separación de dígitos y lectura del contador
void LECT2(void);   //Lectura de botones
void envio(void);   //Datos que recibirá la terminal de lectura
void CONTADOR(void);    //Antirrebote del contador

/*****************************INTERRUPCIONES***********************************/
void __interrupt() ISR(void){
    if (PIR1bits.RCIF == 1){    //El recieve buffer del EUSART está lleno
        signo = RCREG; //El signo será el dato recivido
        CONTADOR();
    }
    if (PIR1bits.TXIF == 1){    //El transmit buffer del EUSART está vacío
        envio();    //Se mandará caracter por caracter con esta función
    }
    if (INTCONbits.RBIF == 1){ //Cambia puerto B
        INTCONbits.RBIF = 0;
        //Anti rebote de los dos botones
        AR1 = PORTBbits.RB0;
        if (AR1==0){
            AR2=0;    
        }
        if (AR2==0){
            if (AR1==1){
                BOTON++;
                AR2=1;
            }
        }
        AR3 = PORTBbits.RB1;
        if (AR3==0){
            AR4=0;
        }
        if (AR4==0){
            if (AR3==1){
                BOTON--;
                AR4=1;
            }
        }
    }
}    
/******************************CICLO*******************************************/
void main(void) {
    Setup();    //Setup
    USARTcon(); //Configuración de EUSART
    while(1){
        if(signo != 13 && signo != 43 && signo != 45){  //Si el caracter ingresado no es + - o enter, no se sumará ni restará
            sum = 0;
            res = 0;
        }
        if(57<signo && signo<48){
            cc = 0;
        }
        LECT1();    
        LECT2();
        PORTD = A1;
        PORTA = CONT;
    }
}

/***************************************FUNCIONES******************************/
void Setup(void){
    //CONFIG I&0
    ANSEL = 0;
    ANSELH = 0;
    ANSEL = 0; //Puertos analógicos y digitales
    TRISA = 0; //Inputs para las señales analógicas
    TRISB = 0b00000011;  //Outputs
    TRISC = 0b10000000; //Input para RX
    TRISD = 0; //Outputs
    TRISE = 0; //Outputs
    PORTA = 0; //Potenciometros
    PORTB = 0; 
    PORTC = 0; //RX y TX
    PORTD = 0; //Pines de LCD. (D0 a D7)
    PORTE = 0; //Pines de LCD. (RS, En, RW)
    INTCONbits.TMR0IF = 0; //Interrupciones
    INTCONbits.RBIF = 0;
    INTCONbits.GIE = 1; //Habilitar interrupciones
    INTCONbits.PEIE = 1; 
    INTCONbits.T0IE = 0; //Interrupción del timer 0
    INTCONbits.RBIE = 1;
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1;
    PIR1bits.ADIF = 0; //Función AD lisa para comenzar
    OSCCONbits.IRCF0 = 1; //Configuración del oscilador (8MHz)
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    ADCON1 = 0;
    TMR0 = 236; //Donde comienza el timer 0
    WPUBbits.WPUB0 = 1; //Weak pull-ups
    WPUBbits.WPUB1 = 1;
    OPTION_REG = 0b01010111; //Configuración de timer 0 y pull ups
    PIE1bits.ADIE = 0;  //ADC
    PIE1bits.RCIE = 1;  //Habilita EUSART Recieve Interrupt
    PIE1bits.TXIE = 1;  //Habilita EUSART Transmit Interrupt
}
void LECT1(void){
    Cen1 = CONT/100;    //Se separan los bits del contador
    Dec1 = (CONT-Cen1*100)/10;
    Un1 = (CONT-Cen1*100-Dec1*10);
    AC1 = num_ascii(Cen1);
    AD1 = num_ascii(Dec1);
    AU1 = num_ascii(Un1);
}
void LECT2(void){
    Cen2 = BOTON/100;    //Se separan los bits del contador
    Dec2 = (BOTON-Cen2*100)/10;
    Un2 = (BOTON-Cen2*100-Dec2*10);
    AC2 = num_ascii(Cen2);
    AD2 = num_ascii(Dec2);
    AU2 = num_ascii(Un2);
}
void envio(void){   //Lectura de la terminal virtual
    toggleTX++;     //Siempre se irá aumentando este contador para que mande datos uno por uno
    if (toggleTX == 1){ //centena contador
        TXREG = AC2;
    }
    if (toggleTX == 2){//decena contador
        TXREG = AD2;
    }
    if (toggleTX == 3){ //u contador
        TXREG = AU2;
    }
    if (toggleTX == 4){ //brk
        TXREG = 10;
        toggleTX = 0;
    }
}

void CONTADOR(void){
    if (signo == 43){   //Si el signo es "+" (43 en código ascii)
        sum = 1;    //Se levantará la bandera de suma
    }
    
    if (signo == 13 && sum == 1){   //Si se apacha enter al estar la bandera de suma levantada
            sum = 0;
            A1++;                 //El contador se le sumará 1
    }
    
    if (signo == 45){  //Si el signo es -
        res = 1;
    }

    if(signo ==13 && res == 1){ //Y se apacha enter
        res = 0;
        A1--; //Se restará
    }
    if(47<signo && signo<58){
        cc++;
        if (cc==1){
            A = num_ascii_dec(signo);
        }
        else if (cc==2){
            B = num_ascii_dec(signo);
        }
        else if (cc==3){
            C = num_ascii_dec(signo);
            ABC = A*100+B*10+C;
        }
    }
    if (signo==10 && cc==3){
        cc = 0;
        CONT=ABC;
    }
    if (cc==4){
        cc=0;
    }
}