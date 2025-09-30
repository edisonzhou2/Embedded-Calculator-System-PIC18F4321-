#include <xc.h>
#include <stdio.h>
#include "LiquidCrystal.h"

#pragma config OSC = INTIO2
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config MCLRE = ON

#define _XTAL_FREQ 1000000

#define SW1 PORTDbits.RD3
#define SW2 PORTDbits.RD4

void adc_init(void);
int adc_read(unsigned char channel);
void delay_ms(unsigned int ms);
unsigned char get_switch_state(void);
int convert_to_signed(int adc_value);

void main(void) {
    TRISB = 0x00;
    TRISD = 0x18;
    TRISA = 0x03;

    pin_setup((volatile PORTx_t*)&PORTB, (volatile PORTx_t*)&PORTD);
    begin(16, 2, LCD_5x8DOTS);

    adc_init();

    int left_value, right_value;
    long result;
    unsigned char state;
    char display_buffer[17];

    while (1) {
        state = get_switch_state();

        switch (state) {
            case 0:
                noDisplay();
                break;

            case 1:
                display();
                left_value = convert_to_signed(adc_read(0));
                right_value = convert_to_signed(adc_read(1));
                result = (long)left_value + (long)right_value;
                clear();
                sprintf(display_buffer, "(%d)+(%d)=%ld", left_value, right_value, result);
                print((unsigned char*)display_buffer);
                break;

            case 2:
                display();
                left_value = convert_to_signed(adc_read(0));
                right_value = convert_to_signed(adc_read(1));
                result = (long)left_value - (long)right_value;
                clear();
                sprintf(display_buffer, "(%d)-(%d)=%ld", left_value, right_value, result);
                print((unsigned char*)display_buffer);
                break;

            case 3:
                display();
                left_value = convert_to_signed(adc_read(0));
                right_value = convert_to_signed(adc_read(1));
                result = (long)left_value * (long)right_value;
                clear();
                setCursor(0, 0);
                sprintf(display_buffer, "(%d)*(%d)", left_value, right_value);
                print((unsigned char*)display_buffer);
                setCursor(0, 1);
                sprintf(display_buffer, "=%ld", result);
                print((unsigned char*)display_buffer);
                break;
        }

        delay_ms(100);
    }
}

void adc_init(void) {
    ADCON1 = 0x0D;
    ADCON2 = 0xA9;
    ADCON0 = 0x01;
    delay_ms(1);
}

int adc_read(unsigned char channel) {
    ADCON0 = (ADCON0 & 0xC3) | (channel << 2);
    delay_ms(1);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ((ADRESH << 8) + ADRESL);
}

int convert_to_signed(int adc_value) {
    return adc_value - 512;
}

unsigned char get_switch_state(void) {
    return (SW2 << 1) | SW1;
}

void delay_ms(unsigned int ms) {
    while (ms--) {
        __delay_ms(1);
    }
}
