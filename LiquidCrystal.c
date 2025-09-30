
#include <xc.h>
#include "LiquidCrystal.h"

volatile PORTx_t *dataPORT;
volatile PORTx_t *signalPORT;

void pin_setup(volatile PORTx_t *dp, volatile PORTx_t *sp) {
    dataPORT = dp;
    signalPORT = sp;
}

void pulseEnable(void) {
    signalPORT->_enable = 0;
    __delay_us(1);
    signalPORT->_enable = 1;
    __delay_us(1);
    signalPORT->_enable = 0;
    __delay_us(100);
}

void write8bits(unsigned char value) {
    dataPORT->_data = value;
}

void send(unsigned char value, unsigned char mode) {
    if (mode)
        signalPORT->_rs = 1;
    else
        signalPORT->_rs = 0;

    signalPORT->_rw = 0;
    write8bits(value);
    pulseEnable();
}

void sendCommand(unsigned char value) {
    send(value, 0);
}

void sendData(unsigned char value) {
    send(value, 1);
}

void clear() {
    sendCommand(LCD_CLEARDISPLAY);
    __delay_ms(2);
}

void home() {
    sendCommand(LCD_RETURNHOME);
    __delay_ms(2);
}

void setRowOffsets(unsigned char row0, unsigned char row1, unsigned char row2, unsigned char row3) {
    _rowOffsets[0] = row0;
    _rowOffsets[1] = row1;
    _rowOffsets[2] = row2;
    _rowOffsets[3] = row3;
}

void setCursor(unsigned char col, unsigned row) {
    if (row >= _numlines) row = _numlines - 1;
    sendCommand(LCD_SETDDRAMADDR | (col + _rowOffsets[row]));
}

void display() {
    _displaycontrol |= LCD_DISPLAYON;
    sendCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

void noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    sendCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

void begin(unsigned char cols, unsigned char lines, unsigned char dotsize) {
    _numlines = lines;
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

    if (lines > 1)
        _displayfunction |= LCD_2LINE;

    if ((dotsize != 0) && (lines == 1))
        _displayfunction |= LCD_5x10DOTS;

    __delay_ms(50);
    sendCommand(LCD_FUNCTIONSET | _displayfunction);
    __delay_ms(5);
    sendCommand(LCD_FUNCTIONSET | _displayfunction);
    __delay_us(150);
    sendCommand(LCD_FUNCTIONSET | _displayfunction);
    sendCommand(LCD_FUNCTIONSET | _displayfunction);

    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    clear();

    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    sendCommand(LCD_ENTRYMODESET | _displaymode);

    setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);
}

void write(unsigned char chr) {
    sendData(chr);
}

void print(unsigned char *chr) {
    while (*chr)
        write(*chr++);
}
