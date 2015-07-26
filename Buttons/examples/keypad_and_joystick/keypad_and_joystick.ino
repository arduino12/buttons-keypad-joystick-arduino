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
| This sketch demonstrates the use of 3x4 keypad and 2-axes analog joystick.   |
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
| 3x4 keypad:                                                                  |
|   * COL3 to pin 5                                                            |
|   * COL2 to pin 6                                                            |
|   * COL1 to pin 7                                                            |
|   * ROW4 to pin 8                                                            |
|   * ROW3 to pin 9                                                            |
|   * ROW2 to pin 10                                                           |
|   * ROW1 to pin 11                                                           |
|                                                                              |
| 2-axis analog joystick:                                                      |
|   * GND to GND                                                               |
|   * VCC to VCC                                                               |
|   * X to pin A0                                                              |
|   * Y to pin A1                                                              |
|   * BUTTON to pin A2                                                         |
|                                                                              |
| Notes:                                                                       |
|   * see "Keypad Pinouts.PNG" and "Keypads Connections.PNG" for help          |
\******************************************************************************/
#include <Buttons.h>

#define LED_PIN						(13)

// create keymap, note that "NONE" value is at index=0, see Buttons.h...
char * keymap_3x4[] = {"NONE", 
	"#",	"9",	"6",	"3",
	"0",	"8",	"5",	"2",
	"*",	"7",	"4",	"1",
};

char * joystick_directions[] = {
	"MIDDLE", "LEFT", "LEFT UP", "UP", "RIGHT UP",
	"RIGHT", "RIGHT DOWN", "DOWN", "LEFT DOWN",
};

Keypad keypad_3x4;
Joystick joystick_2;

void setup() {
	Serial.begin(115200);			// initialize serial to 115200 baud
	
	pinMode(LED_PIN, OUTPUT);		// LED is connected between pin and VCC via 1K resistor.
	
	uint8_t row_pins[]		= {5, 6, 7};
	uint8_t column_pins[]	= {8, 9, 10, 11};
	
	keypad_3x4.begin(
		row_pins,					// row pins
		column_pins,				// column pins
		sizeof(row_pins),			// rows count
		sizeof(column_pins)			// columns count
	);
	keypad_3x4.setPressMillis(
		20,							// short_press_millis - the minimal amount of milliseconds for a "short press".
		1000,						// long_press_millis - the minimal amount of milliseconds for a "long press".
		800							// pulse_press_millis - the amount of milliseconds between "long pulse".
	);
	keypad_3x4.setEventsFuncs(
		onShortPress3x4,			// function that will be called once when button is down for more then short_press_millis.
		onShortRelease3x4,			// function that will be called once the button released after less then long_press_millis.
		onLongPress3x4,				// function that will be called once when button is down for more then long_press_millis.
		onLongRelease3x4,			// function that will be called once the button released after more then long_press_millis.
		onLongPulse3x4				// function that will be called once when button is down and next_pulse_millis passed.
	);
	
	joystick_2.begin(
		A0,							// x axis pin - must be casted to pin_t because of overloading.
		A1,							// y axis pin - must be casted to pin_t because of overloading.
		A2,							// button pin - exists in most 2-axes analog joystick, connected to GND.
		900,						// left limit - bigger/smaller joystick axis readings will be considered LEFT.
		100,						// right limit - bigger/smaller joystick axis readings will be considered RIGHT.
		100,						// up limit - bigger/smaller joystick axis readings will be considered UP.
		900							// down limit - bigger/smaller joystick axis readings will be considered DOWN.
	);
	joystick_2.setPressMillis(
		30,							// short_press_millis - the minimal amount of milliseconds for a "short press".
		500,						// long_press_millis - the minimal amount of milliseconds for a "long press".
		100							// pulse_press_millis - the amount of milliseconds between "long pulse".
	);
	joystick_2.setShortPressFunc(
		onShortPressJoystick_2		// function that will be called once when button is down for more then short_press_millis.
	);
	
	Serial.print(F("setup completed!\n\n"));
}

void loop() {
	// update the joystick and keypad states and call events if needed, max 50ms between calls.
	joystick_2.update();
	keypad_3x4.update();
	
	// led shows the state.
	digitalWrite(LED_PIN, keypad_3x4.isDown() || joystick_2.isButtonDown());

	delay(5);
}

void onShortPressJoystick_2() {
	Serial.print(F("joystick direction:\n"));
	Serial.print(joystick_directions[joystick_2.getDirection()]);
	Serial.print(F("\n\n"));
}

void onShortPress3x4() {
	Serial.print(F("press begin:\n"));
	Serial.print(F("buttons = 0x"));
	Serial.print(keypad_3x4.getButtonsBitmask(), HEX);
	Serial.print(F(", key = "));
	Serial.print(keymap_3x4[keypad_3x4.getButtonIndex()]);
	Serial.print(F(", millis up time = "));
	Serial.print(keypad_3x4.getUpMillis(), DEC);
	Serial.print(F("\n"));
}

void onShortRelease3x4() {
	Serial.print(F("short press completed:\n"));
	Serial.print(F("millis = "));
	Serial.print(keypad_3x4.getDownMillis(), DEC);
	Serial.print(F("\n\n"));
}

void onLongPress3x4() {
	Serial.print(F("long press begin:\n"));
	Serial.print(F("millis = "));
	Serial.print(keypad_3x4.getDownMillis(), DEC);
	Serial.print(F("\n"));
}

void onLongRelease3x4() {
	Serial.print(F("\nlong press completed:\n"));
	Serial.print(F("millis = "));
	Serial.print(keypad_3x4.getDownMillis(), DEC);
	Serial.print(F(", pulses = "));
	Serial.print(keypad_3x4.getPulseCount(), DEC);
	Serial.print(F("\n\n"));
}

void onLongPulse3x4() {
	uint32_t down_millis = keypad_3x4.getDownMillis();
	
	if (down_millis < 4000)
		keypad_3x4.setNextPulseMillis(1000);
	else if (down_millis < 7000)
		keypad_3x4.setNextPulseMillis(400);
	else
		keypad_3x4.setNextPulseMillis(100);
	
	Serial.print('.');
}
