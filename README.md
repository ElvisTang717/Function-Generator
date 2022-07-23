# Function-Generator

# Project Introduction
This project aims to design a function generator by integrating a DAC(MCP 4921 E/P), a 4 X 4 keyboard, and the MCU STM32L476. The function generator could generate 4 different function waveforms, which include sine wave, triangle wave, sawtooth wave, and square wave. The Function Generator could display five different frequencies: 100 Hz, 200Hz, 300Hz, 400Hz, 500Hz for each waveform, and could also adjust the duty cycle of the square wave from 10% through 90% with an interval of 10%. <br /> 
All the functions of the function generator are controlled by the 4 X 4 keypad. While pressing 6, 7, 8, and 9 it will generate sine waves, triangle waves, sawtooth waves, and square waves. Pressing 1, 2, 3, 4, and 5 will adjust the frequency of the wave from 100 Hz to 500Hz. Pressing * and # will decrease and increase the duty cycle of the square wave and pressing 0 will reset the duty cycle to the default duty cycle.<br />
The waveform is generated through outputting different voltages(0V ~ 3V) on the oscilloscope by digital to analog converter(DAC).<br />

# System Specification
It can be seen from the below specifications of the LCD module that the size of the LCD monitor we used for the lockbox is sufficient, and the color contrast is very good. It can not only display the instructions clearly but also have enough space to key in the pins. The power requirement is 3.3V, which is basically very low, so we can use a very small power supply to drive the LCD. <br />

| Wave Form | Sin wave, Triangle Wave, Sawtooth Wave, Square Wave|
| :-----------:  | :-----------: |
| Frequency  Range| 100Hz, 200Hz, 300Hz, 400Hz, 500Hz |
| Duty Cycle |From 10% to 90%(only for square wave)|
| Max Resolution | 349040 samples/seconds |
| Power Supply | 3.3V |
| DAC| MCP 4921 E/P |
| High Volts(output) |3V|
| Low Volts(output) | 0V |
| Control Panel | Adafruit 4 X 4 Keypad |

# Schematic
<p align="center">
  <img src="Media/Schematic.jpg" alt="" width="70%"/>
</p>

# Software Architecture
* Calculations - Interrupt Period & Max Resolution:<br />
There are two while loops in the software architecture, I label them as while loop 1 and while loop 2. The purpose of both while loops is to continuously detect the status of the keypad(pressed or released) and repeatedly check each condition.<br /> 
The software architecture can mainly separate into two parts, one is the lock state, and another is the unlock state. The region within the red box shown in the picture below is the sector that shows how the pins are being entered while in the lock state. The region within the blue sector is the sector that compares the pins we key in and the default keys, if two sets of pins align with each other then the lockbox system will be unlocked.<br />
After unlocking the lockbox, the software architecture comes to the second while loop. The green sector within this while loop offers the function of resetting the default pins and relocking the system after resetting the pins.<br />
The details and further information can be shown in the following figures, which include the flowcharts of the lockbox system and the functions that have been utilized while designing the software architecture of this system.<br />
* main():
<p align="center">
  <img src="Media/main.jpg" alt="" width="70%"/>
</p>

* CheckPress():
<p align="center">
  <img src="Media/CheckPress.jpg" alt="" width="70%"/>
</p>

* Key_in():
<p align="center">
  <img src="Media/Key_in.jpg" alt="" width="70%"/>
</p>

* CompareArrays():
<p align="center">
  <img src="Media/CompareArray.jpg" alt="" width="70%"/>
</p>
