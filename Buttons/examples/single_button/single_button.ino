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
| This sketch demonstrates the use of a button.                               |
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
| button:                                                                   |
|   * COM to GND                                                               |
|   * B1 to pin A0                                                             |
|                                                                              |
| Notes:                                                                       |
|   * see "Keypad Pinouts.PNG" and "Keypads Connections.PNG" for help          |
\******************************************************************************/
#include <Buttons.h>

#define LED_PIN						(13)

Buttons button(getButtonBitmask);

void setup() {
	Serial.begin(115200);			// initialize serial to 115200 baud
	
	pinMode(LED_PIN, OUTPUT);		// LED is connected between pin and VCC via 1K resistor.
	PORTC |= 0x01; DDRC &= 0xFE;	// pin A0 INPUT_PULLUP
	
	button.setEventsFuncs(
		onShortPress,				// function that will be called once when button is down for more then short_press_millis.
		onShortRelease,				// function that will be called once the button released after less then long_press_millis.
		onLongPress,				// function that will be called once when button is down for more then long_press_millis.
		onLongRelease,				// function that will be called once the button released after more then long_press_millis.
		onLongPulse					// function that will be called once when button is down and next_pulse_millis passed.
	);

	Serial.print(F("setup completed!\n\n"));
}

void loop() {
	// update the buttons states and call events if needed, max 50ms between calls.
	button.update();
	// led shows the state.
	digitalWrite(LED_PIN, button.isDown());

	delay(5);
}

bitmask_t getButtonBitmask() {
	return ~PINC & 0x01; // pin A0
}

void onShortPress() {
	Serial.print(F("press begin:\n"));
	Serial.print(F("millis up time = "));
	Serial.print(button.getUpMillis(), DEC);
	Serial.print(F("\n"));
}

void onShortRelease() {
	Serial.print(F("short press completed:\n"));
	Serial.print(F("millis = "));
	Serial.print(button.getDownMillis(), DEC);
	Serial.print(F("\n\n"));
}

void onLongPress() {
	Serial.print(F("long press begin:\n"));
	Serial.print(F("millis = "));
	Serial.print(button.getDownMillis(), DEC);
	Serial.print(F("\n"));
}

void onLongRelease() {
	Serial.print(F("\nlong press completed:\n"));
	Serial.print(F("millis = "));
	Serial.print(button.getDownMillis(), DEC);
	Serial.print(F(", pulses = "));
	Serial.print(button.getPulseCount(), DEC);
	Serial.print(F("\n\n"));
}

void onLongPulse() {
	uint32_t down_millis = button.getDownMillis();
	
	if (down_millis < 4000)
		button.setNextPulseMillis(1000);
	else if (down_millis < 7000)
		button.setNextPulseMillis(400);
	else
		button.setNextPulseMillis(100);
	
	Serial.print('.');
}
