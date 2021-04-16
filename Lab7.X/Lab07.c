/*
 * Archivo:   Lab07.c
 * Dispositivo: PIC16F887
 * Autor: Margareth Vela 
 * 
 * Programa: Programación en C
 * Hardware:Leds en PORTA, push buttons en PORTB, 7 seg en PORTC y 
 *          transistores en PORTD
 * 
 * Creado: Abril 12, 2021
 * Última modificación: Abril 15, 2021
 */

//------------------------------------------------------------------------------
//                          Importación de librerías
//------------------------------------------------------------------------------    
 
#include <xc.h>

//------------------------------------------------------------------------------
//                          Directivas del compilador
//------------------------------------------------------------------------------
#define reset_tmr0 236 // valor de n para t=5ms

//------------------------------------------------------------------------------
//                          Variables
//------------------------------------------------------------------------------
char tab7seg[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67}; //Tabla
char var_temp; //Variable temporal 
char unidades = 0; //Variables para obtener el valor del contador en decimal
char decenas = 0;
char decenas_temp = 0; //Variable temporal de decenas
char centenas = 0;
char unidad_display = 0; //variables para displays
char decena_display = 0;
char centena_display = 0;

//------------------------------------------------------------------------------
//                          Palabras de configuración
//------------------------------------------------------------------------------    
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT//Oscillator Selection bits(INTOSCIO 
                              //oscillator: I/O function on RA6/OSC2/CLKOUT pin, 
                              //I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled and 
                          //can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR  
                                //pin function is digital input, MCLR internally 
                                //tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code 
                                //protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code 
                                //protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit 
                                //Internal/External Switchover mode is disabled
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit 
                                //(Fail-Safe Clock Monitor is disabled)
#pragma config LVP = ON         //Low Voltage Programming Enable bit(RB3/PGM pin 
                                //has PGM function, low voltage programming 
                                //enabled)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out 
                                //Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits 
                                //(Write protection off)

//------------------------------------------------------------------------------
//                          Prototipos
//------------------------------------------------------------------------------
void setup(void);  //Configuración
void contador(void); //Función para volver el contador binario a decimal
//------------------------------------------------------------------------------
//                          Loop principal
//------------------------------------------------------------------------------

void main(void) {
    setup(); //Configuración
    while(1){
        contador(); //Contador binario a decimal
    }
    return;
}


void __interrupt() isr(void){

    if (INTCONbits.T0IF){ //Int TMR0
        PORTD = 0x00; //Se limpia el valor de los transistores
        if(PORTE == 0){
            PORTC = unidad_display; //Se muestra el valor de unidades
            PORTDbits.RD0 = 1; //Se enciende el transistor con el display de unidades
            PORTE++; //Se incrementa PORTE
        }
        else if(PORTE == 1){
            PORTC = decena_display; //Se muestra el valor de decenas
            PORTDbits.RD1 = 1; //Se enciende el transistor con el display de decenas
            PORTE++; //Se incrementa PORTE
        }
        else {
            PORTC = centena_display; //Se muestra el valor de centenas
            PORTDbits.RD2 = 1; //Se enciende el transistor con el display de centenas
            PORTE = PORTE + 2; //Se incrementa PORTE
        }
        TMR0 = reset_tmr0; //Se reinicia el TMR0
        INTCONbits.T0IF = 0; //Se limpia la bandera
    }
    if(INTCONbits.RBIF){
        if (PORTBbits.RB0 == 0){ //Si el botón de incremento está presionado,
            PORTA++; //se incrementa PORTA
        }
        if(PORTBbits.RB1 == 0) {//Si el botón de decremento está presionado,
            PORTA--; //se decrementa PORTA
        }

        INTCONbits.RBIF = 0; //Se limpia la bandera
    }
    return;
}

//------------------------------------------------------------------------------
//                          Configuración
//------------------------------------------------------------------------------
void setup(){
    
    //Configuracion reloj
    OSCCONbits.IRCF2 = 1; //Frecuencia a 4MHZ
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCCONbits.SCS = 1;
    
    //Configurar entradas y salidas
    ANSELH = 0x00;
    ANSEL = 0x00;
    
    
    TRISA = 0x00; //Para salida de contador binario
    TRISD = 0x00; //Para salida de transistores
    TRISB = 0x03; //Para push buttons
    TRISC = 0x00; //Para salida de displays
    TRISE = 0x8;  //Para multiplexado de displays
            
    //Habilitar pullups
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0x03;
    
    PORTA = 0x00; //Se limpian los puertos
    PORTD = 0x00;
    PORTB = 0x03; //Para condición de mismatch     
    PORTC = 0x00;
    PORTE = 0x00;
    
    //Configurar la interrupcion
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    
    //Interrupcion PORTB
    INTCONbits.RBIE = 1; 
    IOCB = 0x03;
    INTCONbits.RBIF = 0; 	
   
    //Configurar TMR0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS2 = 1; //Prescaler 1:256
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;
    TMR0 = reset_tmr0;  
    return;
 } 

void contador(void){
    var_temp = PORTA; //Se coloca el valor de PORTA a la variable temporal
        
    centenas = var_temp/100; //Se divide por 100 para obtener las centenas
    decenas_temp = var_temp%100;//El residuo se almacena en la variable temporal de decenas
    decenas = decenas_temp/10;//Se divide en 10 el valor de decenas_temp para obtener el valor a desplegar en el display
    unidades = var_temp%10;//El residuo se almacena en unidades 
    unidad_display = tab7seg[unidades]; //Se obtienen los valores de la tabla 
    decena_display = tab7seg[decenas];  //para los displays
    centena_display = tab7seg[centenas];
}