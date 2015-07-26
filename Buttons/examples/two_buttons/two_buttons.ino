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
| This sketch demonstrates the use of 2 buttons.                               |
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
| 2 buttons:                                                                   |
|   * COM to GND                                                               |
|   * B1 to pin A0                                                             |
|   * B2 to pin A1                                                             |
|                                                                              |
| Notes:                                                                       |
|   * see "Keypad Pinouts.PNG" and "Keypads Connections.PNG" for help          |
\******************************************************************************/
#include <Buttons.h>

#define LED_PIN						(13)

Buttons buttons_2;

void setup() {
	Serial.begin(115200);			// initialize serial to 115200 baud
	
	pinMode(LED_PIN, OUTPUT);		// LED is connected between pin and VCC via 1K resistor.
	
	uint8_t buttons_pins[] = {A0, A1};
	
	buttons_2.begin(
		buttons_pins,				// buttons_pins
		sizeof(buttons_pins)		// buttons_count
	);
	buttons_2.setEventsFuncs(
		onShortPress2,				// function that will be called once when button is down for more then short_press_millis.
		onShortRelease2,			// function that will be called once the button released after less then long_press_millis.
		onLongPress2,				// function that will be called once when button is down for more then long_press_millis.
		onLongRelease2,				// function that will be called once the button released after more then long_press_millis.
		onLongPulse2				// function that will be called once when button is down and next_pulse_millis passed.
	);

	Serial.print(F("setup completed!\n\n"));
}

void loop() {
	// update the buttons states and call events if needed, max 50ms between calls.
	buttons_2.update();
	// led shows the state.
	digitalWrite(LED_PIN, buttons_2.isDown());

	delay(5);
}

void onShortPress2() {
	Serial.print(F("press begin:\n"));
	Serial.print(F("buttons = 0x"));
	Serial.print(buttons_2.getButtonsBitmask(), HEX);
	Serial.print(F(", millis up time = "));
	Serial.print(buttons_2.getUpMillis(), DEC);
	Serial.print(F("\n"));
}

void onShortRelease2() {
	Serial.print(F("short press completed:\n"));
	Serial.print(F("millis = "));
	Serial.print(buttons_2.getDownMillis(), DEC);
	Serial.print(F("\n\n"));
}

void onLongPress2() {
	Serial.print(F("long press begin:\n"));
	Serial.print(F("millis = "));
	Serial.print(buttons_2.getDownMillis(), DEC);
	Serial.print(F("\n"));
}

void onLongRelease2() {
	Serial.print(F("\nlong press completed:\n"));
	Serial.print(F("millis = "));
	Serial.print(buttons_2.getDownMillis(), DEC);
	Serial.print(F(", pulses = "));
	Serial.print(buttons_2.getPulseCount(), DEC);
	Serial.print(F("\n\n"));
}

void onLongPulse2() {
	uint32_t down_millis = buttons_2.getDownMillis();
	
	if (down_millis < 4000)
		buttons_2.setNextPulseMillis(1000);
	else if (down_millis < 7000)
		buttons_2.setNextPulseMillis(400);
	else
		buttons_2.setNextPulseMillis(100);
	
	Serial.print('.');
}
