/*********************************** LICENCE **********************************\
| Copyright (c) 2015, A.E. TEC                                                 |
| All rights reserved.                                                         |
|                                                                              |
| Redistribution and use in source and binary forms, with or without           |
| modification, are permitted provided that the following conditions are met:  |
|                                                                              |
| * Redistributions of source code must retain the above copyright notice,     |
|   this list of conditions and the following disclaimer.                      |
| * Redistributions in binary form must reproduce the above copyright notice,  |
|   this list of conditions and the following disclaimer in the documentation  |
|   and/or other materials provided with the distribution.                     |
|                                                                              |
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  |
| AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    |
| IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   |
| ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE    |
| LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR          |
| CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF         |
| SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS     |
| INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      |
| CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)      |
| ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   |
| POSSIBILITY OF SUCH DAMAGE.                                                  |
\******************************************************************************/
/************************************ USES ************************************\
| This sketch demonstrates the use of both 4x5 and 6x1 keypads.                |
|                                                                              |
| It works with my Buttons.h lib - a non-blocking smart input library,         |
| supports: single-press, multi-press, long-press, pulse-press and de-bouncing.|
|                                                                              |
| With this sketch, you can easily understand how to use my Buttons.h lib..!   |
|                                                                              |
| Simply burn it to your Arduino, and open the serial monitor with 115200 baud |
|                                                                              |
\******************************************************************************/
/*********************************** CIRCUIT **********************************\
| 4x5 keypad:                                                                  |
|   * COL1 to pin 4                                                            |
|   * COL2 to pin 5                                                            |
|   * COL3 to pin 6                                                            |
|   * COL4 to pin 7                                                            |
|   * ROW5 to pin 8                                                            |
|   * ROW4 to pin 9                                                            |
|   * ROW3 to pin 10                                                           |
|   * ROW2 to pin 11                                                           |
|   * ROW1 to pin 12                                                           |
|                                                                              |
| 6x1 keypad:                                                                  |
|   * ROW1 to GND                                                              |
|   * COL1 to pin A5                                                           |
|   * COL2 to pin A4                                                           |
|   * COL3 to pin A3                                                           |
|   * COL4 to pin A2                                                           |
|   * COL5 to pin A1                                                           |
|   * COL6 to pin A0                                                           |
|   * LED- to pin 13 via 1K resistor                                           |
|   * LED+ to pin VCC                                                          |
|                                                                              |
| Notes:                                                                       |
|   * see "Keypad Pinouts.PNG" and "Keypads Connections.PNG" for help          |
\******************************************************************************/
#include <Buttons.h>

#define LED_PIN						(13)

// create keymap, note that "NONE" value is at index=0, see Buttons.h...
char * keymap_4x5[] = {"NONE", 
	"Ent",	"Esc",	"Down",	"Up",	"*",
	"Right","9",	"6",	"3",	"#",
	"0",	"8",	"5",	"2",	"F2",
	"Left",	"7",	"4",	"1",	"F1",
};

char * keymap_6[] = {"NONE",
	"ON/OFF", "SELECT", "RIGHT", "LEFT", "MENU", "AUTO",
};

Keypad keypad_4x5;
Buttons buttons_6;

void setup() {
	Serial.begin(115200);			// initialize serial to 115200 baud
	
	pinMode(LED_PIN, OUTPUT);		// LED is connected between pin and VCC via 1K resistor.
	
	DDRD |= 0xF0; PORTD &= 0x0F;	// (4  -  7) outputs 
	DDRB &= 0xE0; PORTB |= 0x1F;	// (8  - 12) inputs (pulled up)
	DDRC &= 0xC0; PORTC |= 0x3F;	// (A0 - A5) inputs (pulled up) 
	
	keypad_4x5.begin(
		setScanFunc4x5,				// function that sets the key(s) to be scanned.
		getDataBitmaskFunc4x5,		// function that returns the key(s) state as bitmask.
		4,							// scan bits count
		5							// data bits count
	);
	keypad_4x5.setPressMillis(
		30,							// short_press_millis - the minimal amount of milliseconds for a "short press".
		1500,						// long_press_millis - the minimal amount of milliseconds for a "long press".
		1000						// pulse_millis - the amount of milliseconds between "long pulse".
	);
	keypad_4x5.setEventsFuncs(
		onShortPress4x5,			// function that will be called once when button is down for more then short_press_millis.
		onShortRelease4x5,			// function that will be called once the button released after less then long_press_millis.
		onLongPress4x5,				// function that will be called once when button is down for more then long_press_millis.
		onLongRelease4x5,			// function that will be called once the button released after more then long_press_millis.
		onLongPulse4x5				// function that will be called once when button is down and next_pulse_millis passed.
	);
	
	buttons_6.begin(
		getButtonsBitmaskFunc6		// function that returns the button(s) state as bitmask.
	);
	buttons_6.setPressMillis(
		30,
		800,
		1000
	);
	
	Serial.print(F("setup completed!\n\n"));
}

void loop() {
	// update button(s) and keys(s) state and call events if needed, max 50ms between calls.
	buttons_6.update();
	keypad_4x5.update();
	
	// led shows the button(s) state.
	digitalWrite(LED_PIN, !keypad_4x5.isDown() && !buttons_6.isDown());
	
	if (buttons_6.isReleased()) {
		// we just released the button(s) (from short or long press).
		Serial.print(F("released completed:\n"));
		Serial.print(F("key = "));
		Serial.print(keymap_6[buttons_6.getButtonIndex()]);
		Serial.print(F("\nbuttons = 0x"));
		Serial.print(buttons_6.getButtonsBitmask(), HEX);
		Serial.print(F(", millis = "));
		Serial.print(buttons_6.getDownMillis(), DEC);
		Serial.print(F("\n\n"));
	}
	else if (buttons_6.isPulsed()) {
		// a pulse occurs (long press for setNextPulseMillis() milliseconds).
		Serial.print(F("0"));
	}
	else if (buttons_6.getState() == BUTTONS_STATE_SHORT_PRESS) {
		// the button(s) is down for less then long_press_millis.
		Serial.print(F("o"));
	}
	else if (buttons_6.getState() == BUTTONS_STATE_LONG_PRESS) {
		// the button(s) is down for more then long_press_millis (and a pulse doesn't occur).
		Serial.print(F("O"));
	}
	delay(5);
}

void setScanFunc4x5(uint8_t scan) {
	DDRD &= 0x0F;					// clear all scan lines (put in "Z" state)
	DDRD |= 0x10 << scan;			// set only the wanted scan line to logic "0" - output
}

bitmask_t getDataBitmaskFunc4x5() {
	return ~PINB & 0x1F;			// returns the scanned key(s) state
}

bitmask_t getButtonsBitmaskFunc6() {
	return ~PINC & 0x3F;			// returns the scanned buttons(s) state
}

void onShortPress4x5() {
	
}

void onShortRelease4x5() {
	Serial.print(F("short press completed:\n"));
	Serial.print(F("buttons = 0x"));
	Serial.print(keypad_4x5.getButtonsBitmask(), HEX);
	Serial.print(F(", key = "));
	Serial.print(keymap_4x5[keypad_4x5.getButtonIndex()]);
	Serial.print(F(", millis = "));
	Serial.print(keypad_4x5.getDownMillis(), DEC);
	Serial.print(F("\n\n"));
}

void onLongPress4x5() {
	Serial.print(F("long press begin:\n"));
	Serial.print(F("buttons = 0x"));
	Serial.print(keypad_4x5.getButtonsBitmask(), HEX);
	Serial.print(F(", key = "));
	Serial.print(keymap_4x5[keypad_4x5.getButtonIndex()]);
	Serial.print(F(", millis = "));
	Serial.print(keypad_4x5.getDownMillis(), DEC);
	Serial.print(F("\n"));
}

void onLongRelease4x5() {
	Serial.print(F("long press completed:\n"));
	Serial.print(F("millis = "));
	Serial.print(keypad_4x5.getDownMillis(), DEC);
	Serial.print(F("\n\n"));
}

void onLongPulse4x5() {
	uint32_t down_millis = keypad_4x5.getDownMillis();
	
	if (down_millis < 2000)			down_millis = 1000;
	else if (down_millis < 4000)	down_millis = 700;
	else							down_millis = 500;
	keypad_4x5.setNextPulseMillis(down_millis);
	Serial.print(F("millis = "));
	Serial.print(keypad_4x5.getDownMillis(), DEC);
	Serial.print(F("\n"));
}
