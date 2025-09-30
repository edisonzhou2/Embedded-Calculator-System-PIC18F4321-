# Embedded Calculator System (PIC18F4321)

A simple 4-state embedded calculator built using the **PIC18F4321 microcontroller**.  
This project performs **addition**, **subtraction**, and **multiplication** using two analog inputs from potentiometers and displays the results on a **16×2 LCD**.  
The user selects the operation using **two DIP switches** and the program updates the LCD accordingly.

---

## 🔧 Features

- **Analog Input via ADC:** Reads two potentiometer values from AN0 and AN1.  
- **Signed Conversion:** Converts 10-bit ADC readings (0–1023) into signed integers (−512 to +511).  
- **Four Operation States Controlled by DIP Switches:**
  - `00` → Display off  
  - `01` → Addition `(A) + (B)`  
  - `10` → Subtraction `(A) - (B)`  
  - `11` → Multiplication `(A) * (B)`  
- **Real-Time LCD Output:** Shows formatted operands and result.  
- **No Interrupts Required:** Operates on a simple polling loop for stable performance.

---

## 🧩 Pin Configuration

| Function | PIC18F4321 Pin/Port | Description |
|-----------|--------------------|--------------|
| **LCD Data Lines** | **PORTB (RB0–RB7)** | 8-bit data bus for LCD |
| **LCD Control Lines** | **PORTD** | Used by the `LiquidCrystal` driver for RS, E, and RW |
| **DIP Switch 1 (LSB)** | **RD3** | Selects operation bit 0 |
| **DIP Switch 2 (MSB)** | **RD4** | Selects operation bit 1 |
| **Potentiometer A** | **RA0 / AN0** | Left operand (ADC channel 0) |
| **Potentiometer B** | **RA1 / AN1** | Right operand (ADC channel 1) |

> 🧠 **Note:**  
> - The LCD is initialized using `pin_setup((volatile PORTx_t*)&PORTB, (volatile PORTx_t*)&PORTD)`  
> - Data pins are on **PORTB**, and control pins (RS/E/RW) are on **PORTD**.

---

## ⚙️ Hardware Components

| Component | Description |
|------------|--------------|
| **Microcontroller** | PIC18F4321 |
| **LCD Display** | 16×2 HD44780-compatible |
| **Potentiometers (x2)** | 10 kΩ for analog input |
| **DIP Switches (x2)** | Select calculator state (00–11) |
| **Power Supply** | 5V regulated DC |
| **Breadboard + Wires** | For circuit assembly |

---

## 🕒 Clock, ADC, and Timing

- **Oscillator:** Internal **1 MHz** (`#pragma config OSC = INTIO2`)  
- **ADC Configuration:**
  - `ADCON1 = 0x0D` → AN0 and AN1 enabled  
  - `ADCON2 = 0xA9` → Right-justified, Fosc/8 conversion  
  - `ADCON0` dynamically switches channels  
- **Delay:** Software delay of ~100 ms between display updates  
- **Math Precision:** Integer-only arithmetic; multiplication uses `long` (32-bit) to handle large results.

---

## 🧠 Program Overview

### Main Control Flow
```c
while (1) {
    state = get_switch_state();        // Read DIP switches
    left_value  = convert_to_signed(adc_read(0));  // AN0
    right_value = convert_to_signed(adc_read(1));  // AN1

    switch (state) {
        case 0: noDisplay(); break;    // Turn LCD off
        case 1: result = left + right; // Addition
        case 2: result = left - right; // Subtraction
        case 3: result = left * right; // Multiplication
    }

    // Update LCD with formatted text
    sprintf(display_buffer, "(%d)+( %d)=%ld", left_value, right_value, result);
    print((unsigned char*)display_buffer);

    delay_ms(100);
}
