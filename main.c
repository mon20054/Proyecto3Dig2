#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"

#define XTAL 16000000

//**************************************************************************************************************
// Variables Globales
//**************************************************************************************************************

char charIn = '0';
uint8_t modo = 0;
uint8_t estado_envio = 0;
uint8_t segmentos = 0;
uint8_t contadorh = 0;
int8_t contador4b = 0;
uint32_t ui32Period;
uint32_t PeriodB;
uint8_t DIP1 = 0;
uint8_t DIP2 = 0;
uint8_t secuencia1 = 0;
uint8_t secuencia2 = 0;
uint32_t lecturaADC;
uint8_t conversion = 0;
uint8_t unidades = 1;
uint8_t decenas = 0;

//**************************************************************************************************************
// Prototipos de Función
//**************************************************************************************************************
void config (void);
void InitUART(void);
void UART0ReadIntHandler(void);
void Timer0AIntHandler(void);
void Timer1AIntHandler(void);
void GPIOIntHandler (void);
unsigned short map(uint32_t val, uint32_t in_min, uint32_t in_max,
            unsigned short out_min, unsigned short out_max);
//**************************************************************************************************************
// Código Principal
//**************************************************************************************************************
int main(void)
{

    // Configruaciones
    config();

    //**********************************************************************************************************
    // Loop Principal
    //**********************************************************************************************************
    while (1)
    {

        // Bandera para DIP 1 encendido
        if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&0b00000010)==0b00000010)
        {
            DIP1 = 1;
        }

        // Bandera para DIP1 apagado
        else if((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)&0b00000010)==0b00000000)
        {
            DIP1 = 0;
        }

        // Bandera para DIP2 encendido
        if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&0b00000100)==0b00000100)
        {
            DIP2 = 1;
        }

        // Bandera para DIP 2 apagado
        else if((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)&0b00000100)==0b00000000)
        {
            DIP2 = 0;
        }

        // Establecer modo de operacion 1 con LED indicador rojo
        if ((DIP1 == 0)&&(DIP2 == 0))
        {
            modo = 1;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0b10);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        }

        // Establecer modo de operacion 2 con LED indicador azul
        else if ((DIP1 == 0)&&(DIP2 == 1))
        {
            modo = 2;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0b100);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        }

        // Establecer modo de operacion 3 con LED indicador verde
        else if ((DIP1 == 1)&&(DIP2 == 0))
        {
            modo = 3;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0b1000);
        }

        // Establecer modo de operacion 4 con LED indicador blanco
        else if ((DIP1 == 1)&&(DIP2 == 1))
        {
            modo = 4;
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0b10);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0b100);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0b1000);
        }


        //modo = 1;

        if (modo == 1)
        {

            // Se recibio una a por UART
            if (charIn == 'a')
            {
                charIn = 'x';
                // Cambiar el estado del LED 1
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0b00010000);
                }
            }

            // Se recibio una b por UART
            if (charIn == 'b')
            {
                charIn = 'x';
                // Cambiar el estado del LED 2
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0b00100000);
                }
            }

            // Se recibio una c por UART
            if (charIn == 'c')
            {
                charIn = 'x';
                // Cambiar el estado del LED 3
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0b01000000);
                }
            }

            // Se recibio una d por UART
            if (charIn == 'd')
            {
                charIn = 'x';
                // Cambiar el estado del LED 4
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0b10000000);
                }
            }

            // Se recibio 0 por UART
            if (charIn == '0')
            {
                charIn = 'x';
                // 0 en display de 7 segmentos
                segmentos = 0;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111110);
            }

            // Se recibio 1 por UART
            if (charIn == '1')
            {
                charIn = 'x';
                // 1 en display de 7 segmentos
                segmentos = 1;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001000);
            }

            // Se recibio 2 por UART
            if (charIn == '2')
            {
                charIn = 'x';
                // 2 en display de 7 segmentos
                segmentos = 2;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111101);
            }

            // Se recibio 3 por UART
            if (charIn == '3')
            {
                charIn = 'x';
                // 3 en display de 7 segmentosestado_envio = 1;
                segmentos = 3;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101101);
            }

            // Se recibio 4 por UART
            if (charIn == '4')
            {
                charIn = 'x';
                // 4 en display de 7 segmentos
                segmentos = 4;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001011);
            }

            // Se recibio 5 por UART
            if (charIn == '5')
            {
                charIn = 'x';
                // 5 en display de 7 segmentos
                segmentos = 5;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01100111);
            }

            // Se recibio 6 por UART
            if (charIn == '6')
            {
                charIn = 'x';
                // 6 en display de 7 segmentos
                segmentos = 6;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110111);
            }

            // Se recibio 7 por UART
            if (charIn == '7')
            {
                charIn = 'x';
                // 7 en display de 7 segmentos
                segmentos = 7;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001100);
            }

            // Se recibio 8 por UART
            if (charIn == '8')
            {
                charIn = 'x';
                // 8 en display de 7 segmentos
                segmentos = 8;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111111);
            }

            // Se recibio 9 por UART
            if (charIn == '9')
            {
                charIn = 'x';
                // 9 en display de 7 segmentos
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001111);
            }

            // Se recibio cualquier otro caracter por UART
            else
            {
                charIn = charIn;
            }

            // Se presiono un boton y se envia el estado por UART
            if (estado_envio == 1)
            {
                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }
                else if (!GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7))
                {
                    UARTCharPut(UART0_BASE, 'L');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'O');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 0)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '0');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 1)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 2)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 3)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 4)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 5)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '5');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 6)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '6');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 7)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 8)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '8');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (segmentos == 9)
                {
                    UARTCharPut(UART0_BASE, 'S');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'g');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '9');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                estado_envio = 0;
            }

        }

        // Modo 2

        else if (modo == 2)
        {

            // Mostrar valor de contador HEX en display de 7 segmentos
            if (contadorh == 0)
            {
                segmentos = 0;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111110);
            }

            if (contadorh == 1)
            {
                segmentos = 1;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001000);
            }

            if (contadorh == 2)
            {
                segmentos = 2;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111101);
            }

            if (contadorh == 3)
            {
                segmentos = 3;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101101);
            }

            if (contadorh == 4)
            {
                segmentos = 4;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001011);
            }

            if (contadorh == 5)
            {
                segmentos = 5;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01100111);
            }

            if (contadorh == 6)
            {
                segmentos = 6;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110111);
            }

            if (contadorh == 7)
            {
                segmentos = 7;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001100);
            }

            if (contadorh == 8)
            {
                segmentos = 8;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111111);
            }

            if (contadorh == 9)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001111);
            }

            if (contadorh == 10)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01011111);
            }

            if (contadorh == 11)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110011);
            }

            if (contadorh == 12)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00110110);
            }

            if (contadorh == 13)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111001);
            }

            if (contadorh == 14)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00110111);
            }

            if (contadorh == 15)
            {
                segmentos = 9;
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00010111);
            }

            // Se recibio un + por UART
            if (charIn == '+')
            {
                charIn = 'x';
                // Incrementar contador de 4 bits
                contador4b++;
                // Overflow
                if (contador4b == 16)
                {
                    contador4b = 0;
                }
            }

            // Se recibio un - por UART
            if (charIn == '-')
            {
                charIn = 'x';
                // Decrementar contador de 4 bits
                contador4b--;
                // Underflow
                if (contador4b == -1)
                {
                    contador4b = 15;
                }
            }

            // Mostrar contador de 4 bits en los LEDs
            if (contador4b == 0)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
            }
            else if (contador4b == 1)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x10);
            }
            else if (contador4b == 2)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x20);
            }
            else if (contador4b == 3)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x30);
            }
            else if (contador4b == 4)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x40);
            }
            else if (contador4b == 5)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x50);
            }
            else if (contador4b == 6)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x60);
            }
            else if (contador4b == 7)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x70);
            }
            else if (contador4b == 8)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x80);
            }
            else if (contador4b == 9)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x90);
            }
            else if (contador4b == 10)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xA0);
            }
            else if (contador4b == 11)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xB0);
            }
            else if (contador4b == 12)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xC0);
            }
            else if (contador4b == 13)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xD0);
            }
            else if (contador4b == 14)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xE0);
            }
            else if (contador4b == 15)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0xF0);
            }

            // Enviar el estado de ambos contadores por UART
            if (estado_envio == 1)
            {
                if (contadorh == 0)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '0');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 1)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 2)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 3)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 4)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 5)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '5');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 6)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '6');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 7)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 8)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '8');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 9)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '9');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 10)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'A');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 11)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'B');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 12)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 13)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'D');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 14)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contadorh == 15)
                {
                    UARTCharPut(UART0_BASE, 'H');
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'X');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 0)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '0');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 1)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 2)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 3)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 4)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 5)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '5');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 6)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '6');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 7)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '7');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 8)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '8');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 9)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '9');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 10)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '0');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 11)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 12)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '2');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 13)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '3');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 14)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '4');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                if (contador4b == 15)
                {
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, '1');
                    UARTCharPut(UART0_BASE, '5');
                    UARTCharPut(UART0_BASE, 10);
                    UARTCharPut(UART0_BASE, 13);
                }

                estado_envio = 0;
            }

        }

        // Modo 3

        else if (modo == 3)
        {

            // Animacion de display de 7 segmentos

            if (secuencia1 == 0)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00010001);
            }

            if (secuencia1 == 1)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10000010);
            }

            if (secuencia1 == 2)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00000101);
            }

            if (secuencia1 == 3)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10001000);
            }

            if (secuencia1 == 4)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01000001);
            }

            if (secuencia1 == 5)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b10100000);
            }

            // Animacion de 4 LEDs

            if (secuencia2 == 0)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b00010000);
            }

            if (secuencia2 == 2)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b00100000);
            }

            if (secuencia2 == 4)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b01000000);
            }

            if (secuencia2 == 6)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0b10000000);
            }

        }

        // Modo 4

        else if (modo == 4)
        {
            // Iniciamos conversión de ADC
            ADCProcessorTrigger(ADC0_BASE, 3);
            // Esperamos a que termine la conversión
            while(!ADCIntStatus(ADC0_BASE, 3, false)){}
            // Limpiamos bandera de interrupción
            ADCIntClear(ADC0_BASE, 3);
            // Guardamos el valor de la lectura en una variable
            ADCSequenceDataGet(ADC0_BASE, 3, &lecturaADC);

            // Mapeo del valor ADC
            conversion = map(lecturaADC, 0, 4096, 0, 100);

            // Obtener decenas y unidades de valor ADC 0-99
            decenas = (conversion/10);
            unidades = (conversion-(decenas*10));

            // Mostrar unidades en display de 7 segmentos
            if (unidades == 0)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111110);
            }

            if (unidades == 1)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001000);
            }

            if (unidades == 2)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b00111101);
            }

            if (unidades == 3)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01101101);
            }

            if (unidades == 4)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001011);
            }

            if (unidades == 5)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01100111);
            }

            if (unidades == 6)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01110111);
            }

            if (unidades == 7)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001100);
            }

            if (unidades == 8)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01111111);
            }

            if (unidades == 9)
            {
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0x00);
                GPIOPinWrite(GPIO_PORTB_BASE, 0xFF, 0b01001111);
            }


            // Mostrar decenas en 4 LEDs en binario
            if (decenas == 0)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x00);
            }
            if (decenas == 1)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x10);
            }
            if (decenas == 2)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x20);
            }
            if (decenas == 3)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x30);
            }
            if (decenas == 4)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x40);
            }
            if (decenas == 5)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x50);
            }
            if (decenas == 6)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x60);
            }
            if (decenas == 7)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x70);
            }
            if (decenas == 8)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x80);
            }
            if (decenas == 9)
            {
                GPIOPinWrite(GPIO_PORTC_BASE, 0xF0, 0x90);
            }


        }

    }
}
//**************************************************************************************************************
// Configuracion
//**************************************************************************************************************
void config (void)
{
    // Se setea oscilador externo de 16MHz
    SysCtlClockSet(
    SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Se inicializa la comunicación UART
    InitUART();

    // Se habilita el puerto F, B, C, D y E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Se establecen como salidas los pines deseados
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);

    // Se establecen como entradas los pines deseados
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4);

    // Configuracion ADC
    // Habilitamos el módulo ADC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    // Habilitamos PORTE
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    // Configuramos PE3 como entrada analógica
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    // Configuramos el muestreador que utilizará al módulo ADC (1 muestra por conversión)
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    // Configuramos secuencia de muestreo del ADC
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    // Habilitamos la secuencia de muestreo del ADC
    ADCSequenceEnable(ADC0_BASE, 3);
    // Borramos la interrupcion del ADC
    ADCIntClear(ADC0_BASE, 3);

    // Se habilita el temporizador 0 y 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Habilitar interrupciones
    IntEnable(INT_UART0);
    // Habilitar el FIFO en 1/8 datos recibidos
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
    // Habilitar interrupción de recepción de UART0
    UARTIntEnable(UART0_BASE, UART_INT_RX);
    // Habilitar interrupción de recepción de GPIO

    // Habilitamos interrupciones para Puerto D y E
    IntEnable(INT_GPIOD);
    IntEnable(INT_GPIOE);
    // Interrupcion en flanco de subida para pin 6 del Puerto D
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_INT_PIN_6, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);
    // Interrupcion en flanco de subida para pin 4 del Puerto E
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_INT_PIN_4, GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_INT_PIN_4);


    // Configuración del Timer 0 como temporizador periodico
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // Configuración del Timer 1 como temporizador periodico
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    // Se calcula el período para los temporizadores
    ui32Period = (SysCtlClockGet());
    PeriodB = (SysCtlClockGet() / 10);

    // Establecer el periodo de los temporizadores
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
    TimerLoadSet(TIMER1_BASE, TIMER_A, PeriodB - 1);

    // Se habilita la interrupción por los temporizadores
    IntEnable(INT_TIMER0A);
    IntEnable(INT_TIMER1A);

    // Se establece que exista la interrupción por Timeout
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Se habilitan los temporizadores
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);

    // Habilitar interrupciones globales
    IntMasterEnable();

}

//**************************************************************************************************************
// Funciones
//**************************************************************************************************************

// Inicializar UART
void InitUART(void)
{
    // Habilitar Puerto A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Habilitar modulo UART 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Pines UART perifericos
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configuracion UART
    UARTConfigSetExpClk(
            UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

}

// Mapeo de numeros
unsigned short map(uint32_t x, uint32_t x0, uint32_t x1,
            unsigned short y0, unsigned short y1){
    return (unsigned short)(y0+((float)(y1-y0)/(x1-x0))*(x-x0));
}

//**************************************************************************************************************
// Interrupciones
//**************************************************************************************************************

// UART0
void UART0ReadIntHandler(void)
{
    // Boramos interrupcion UART
    UARTIntClear(UART0_BASE, UART_INT_RX);

    // Guardamos char recibido en una variable
    charIn = UARTCharGet(UART0_BASE);

    // Imprimimos valor recibido en consola
    UARTCharPut(UART0_BASE, 'U');
    UARTCharPut(UART0_BASE, 's');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, 'r');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'i');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 'p');
    UARTCharPut(UART0_BASE, 'u');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, ':');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, charIn);
    UARTCharPut(UART0_BASE, 10);
    UARTCharPut(UART0_BASE, 13);

}

// GPIO
void GPIOIntHandler(void)
{

   // Verificamos si fue el Pin 6 del puerto D
   if ((GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6)&0b01000000)==0b01000000)
   {
       // Borramos interrupcion
       GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

       if (modo == 1)
       {
           // Enviar estado de LEDs y segmentos por UART
           estado_envio = 1;
       }

       else if (modo == 2)
       {
           // Incrementar el contador de 4 bits
           contador4b++;
           // Overflow
           if (contador4b == 16)
           {
               contador4b = 0;
           }
       }

   }

   // Verificamos si fue el Pin 4 del puerto E
   if ((GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_4)&0b00010000)==0b00010000)
      {
           // Borramos la interrupcion
           GPIOIntClear(GPIO_PORTE_BASE, GPIO_INT_PIN_4);

           if (modo == 1)
          {
              //Enviar estado de LEDs y segmentos por UART
              estado_envio = 1;
          }

          else if (modo == 2)
          {
              // Decrementar el contador de 4 bits
              contador4b--;
              // Underflow
              if (contador4b == -1)
              {
                  contador4b = 15;
              }
          }
      }

}

// Timer 0
void Timer0AIntHandler(void)
{
    // Borrar la interrupcion del timer
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if (modo == 2)
    {
        // Incrementar el contador hexadecimal
        contadorh++;
        // Enviar dato de contadores por UART
        estado_envio = 1;

        // Overflow
        if (contadorh == 0x10)
        {
            contadorh = 0;
        }
    }
}

// Timer 1
void Timer1AIntHandler(void)
{
    // Borrar la interrupcion del timer
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    secuencia1++;   // Velocidad 1
    secuencia2++;   // Velocidad 2

    // Reiniciar contador para animacion de 7 segmentos
    if (secuencia1 == 6)
    {
        secuencia1 = 0;
    }

    // Reiniciar contador para animacion de 4 leds
    if (secuencia2 == 8)
    {
        secuencia2 = 0;
    }

}


