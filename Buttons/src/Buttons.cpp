#include <Buttons.h>

Buttons::Buttons() { }

Buttons::~Buttons() {
	end();
}

void Buttons::end() {
	if (NULL != _buttons_pins)	free(_buttons_pins);
	_buttons_pins				= NULL;
	_getButtonsBitmaskFunc		= NULL;
}

Buttons::Buttons(
	bitmask_t	(* getButtonsBitmaskFunc)()
) {
	Buttons::begin(
		getButtonsBitmaskFunc
	);
}

Buttons::Buttons(
	uint8_t *	buttons_pins,
	uint8_t		buttons_count
) {
	Buttons::begin(
		buttons_pins,
		buttons_count
	);
}

void Buttons::begin(
	bitmask_t	(* getButtonsBitmaskFunc)()
) {
	_getButtonsBitmaskFunc = getButtonsBitmaskFunc;
	_buttonsUp();
}

void Buttons::begin(
	uint8_t *	buttons_pins,
	uint8_t		buttons_count
) {
	Buttons::begin((bitmask_t (*)())&Buttons::_getButtonsBitmaskFromPins);
	_buttons_count = buttons_count;
	_buttons_pins = _allocAndCopy(buttons_pins, _buttons_count);
	for (uint8_t button = 0; button < _buttons_count; button++)	// pull all buttons pins to logic "1"
		pinMode(_buttons_pins[button], INPUT_PULLUP);
}

uint8_t * Buttons::_allocAndCopy(uint8_t * src, uint8_t size) {
	uint8_t * dst = (uint8_t *)malloc(size);
	memcpy(dst, src, size);
	return dst;
}

bitmask_t Buttons::_getButtonsBitmaskFromPins() {
	bitmask_t keypad_bitmask = 0;
	for (uint8_t button = 0; button < _buttons_count; button++)
		if (!digitalRead(_buttons_pins[button]))				// did a button shortened with logic "0"?
			keypad_bitmask |= (bitmask_t)1 << button;
	return keypad_bitmask;
}

void Buttons::setShortPressMillis(
	uint32_t	short_press_millis
) {
	_short_press_millis		= short_press_millis;
}

void Buttons::setLongPressMillis(
	uint32_t	long_press_millis
) {
	_long_press_millis		= long_press_millis;
}

void Buttons::setPulsePressMillis(
	uint32_t	pulse_press_millis
) {
	_pulse_press_millis		= pulse_press_millis;
}

void Buttons::setPressMillis(
	uint32_t	short_press_millis,
	uint32_t	long_press_millis,
	uint32_t	pulse_press_millis
) {
	_short_press_millis		= short_press_millis;
	_long_press_millis		= long_press_millis;
	_pulse_press_millis		= pulse_press_millis;
}

void Buttons::setShortPressFunc(
	event_t		onShortPressFunc
) {
	_onShortPressFunc	= _setEventFunc(onShortPressFunc);
}

void Buttons::setShortReleaseFunc(
	event_t		onShortReleaseFunc
) {
	_onShortReleaseFunc	= _setEventFunc(onShortReleaseFunc);
}
		
void Buttons::setLongPressFunc(
	event_t		onLongPressFunc
) {
	_onLongPressFunc	= _setEventFunc(onLongPressFunc);
}

void Buttons::setLongReleaseFunc(
	event_t		onLongReleaseFunc
) {
	_onLongReleaseFunc	= _setEventFunc(onLongReleaseFunc);
}

void Buttons::setLongPulseFunc(
	event_t		onLongPulseFunc
) {
	_onLongPulseFunc	= _setEventFunc(onLongPulseFunc);
}

void Buttons::setEventsFuncs(
	event_t		onShortPressFunc,
	event_t		onShortReleaseFunc,
	event_t		onLongPressFunc,
	event_t		onLongReleaseFunc,
	event_t		onLongPulseFunc
) {
	_onShortPressFunc	= _setEventFunc(onShortPressFunc);
	_onShortReleaseFunc	= _setEventFunc(onShortReleaseFunc);
	_onLongPressFunc	= _setEventFunc(onLongPressFunc);
	_onLongReleaseFunc	= _setEventFunc(onLongReleaseFunc);
	_onLongPulseFunc	= _setEventFunc(onLongPulseFunc);
}

void Buttons::setNextPulseMillis(
	uint32_t	next_pulse_millis
) {
	_next_pulse_millis = _down_millis + next_pulse_millis;
}

void Buttons::update() {
	update(millis());
}

void Buttons::update(uint32_t cur_millis) {
	if (NULL == _getButtonsBitmaskFunc) return;
	uint32_t delta_millis = cur_millis - _old_millis;
	bitmask_t buttons_bitmask = _getButtonsBitmaskFunc();

	if (_buttons_bitmask)	_down_millis += delta_millis;
	else					_up_millis += delta_millis;
	
	if		(isReleased()) {
		_buttonsUp();
	}
	else if	(_buttons_state == BUTTONS_STATE_LONG_PULSE) {
		_buttons_state = BUTTONS_STATE_LONG_PRESS;
	}
	
	if		(_buttons_state == BUTTONS_STATE_UP) {
		if (buttons_bitmask) {
			_down_millis = 0;
			_pulse_count = 0;
			_buttons_bitmask = buttons_bitmask;
			_buttons_state = BUTTONS_STATE_BOUNCE;
		}
	}
	else if	(_buttons_state == BUTTONS_STATE_BOUNCE) {
		if (_buttons_bitmask != buttons_bitmask) {
			_buttonsUp();
		}
		else if (_down_millis >= _short_press_millis) {
			_buttons_state = BUTTONS_STATE_SHORT_PRESS;
			_onShortPressFunc();
		}
	}
	else if	(_buttons_state == BUTTONS_STATE_SHORT_PRESS) {
		if (0 == buttons_bitmask) {
			_buttons_state = BUTTONS_STATE_SHORT_RELEASE;
			_onShortReleaseFunc();
		}
		else if (_down_millis >= _long_press_millis) {
			setNextPulseMillis(_pulse_press_millis);
			_buttons_state = BUTTONS_STATE_LONG_PRESS;
			_onLongPressFunc();
		}
	}
	else if	(_buttons_state == BUTTONS_STATE_LONG_PRESS) {
		if (0 == buttons_bitmask) {
			_buttons_state = BUTTONS_STATE_LONG_RELEASE;
			_onLongReleaseFunc();
		}
		else if (_down_millis >= _next_pulse_millis) {
			setNextPulseMillis(_pulse_press_millis);
			_pulse_count++;
			_buttons_state = BUTTONS_STATE_LONG_PULSE;
			_onLongPulseFunc();
		}
	}
	
	_old_millis = cur_millis;
}

void Buttons::_buttonsUp() {
	_buttons_bitmask = 0;
	_up_millis = 0;
	_down_millis = 0;
	_pulse_count = 0;
	_buttons_state = BUTTONS_STATE_UP;
}

event_t Buttons::_setEventFunc(event_t func) {
	return (func ? func : _nothing);
}

uint32_t Buttons::getPulseCount() {
	return	_pulse_count;
}

bitmask_t Buttons::getButtonsBitmask() {
	return _buttons_bitmask;
}

uint8_t Buttons::getButtonIndex() {
	bitmask_t buttons_bitmask = getButtonsBitmask();
	uint8_t button_index = 0;
	while (buttons_bitmask) {
		button_index++;
		buttons_bitmask >>= 1;
	}
	return button_index;
}

uint32_t Buttons::getUpMillis() {
	return	_up_millis;
}

uint32_t Buttons::getDownMillis() {
	return	_down_millis;
}

buttons_state_t Buttons::getState() {
	return	_buttons_state;
}

bool Buttons::isUp() {
	return	!isDown();
}

bool Buttons::isDown() {
	return	isPressed() || (_buttons_state == BUTTONS_STATE_BOUNCE);
}

bool Buttons::isPressed() {
	return	(_buttons_state == BUTTONS_STATE_SHORT_PRESS) ||
			(_buttons_state == BUTTONS_STATE_LONG_PRESS) ||
			(_buttons_state == BUTTONS_STATE_LONG_PULSE);
}

bool Buttons::isReleased() {
	return	(_buttons_state == BUTTONS_STATE_SHORT_RELEASE) ||
			(_buttons_state == BUTTONS_STATE_LONG_RELEASE);
}

bool Buttons::isPulsed() {
	return	(_buttons_state == BUTTONS_STATE_LONG_PULSE);
}



Keypad::Keypad() : Buttons() { }

Keypad::~Keypad() {
	end();
}

void Keypad::end() {
	if (NULL != _row_pins)		free(_row_pins);
	if (NULL != _column_pins)	free(_column_pins);
	_row_pins			= NULL;
	_column_pins		= NULL;
	Buttons::end();
}

Keypad::Keypad(
	void		(* setScanFunc)(uint8_t scan),
	bitmask_t	(* getDataBitmaskFunc)(),
	uint8_t		scans_count,
	uint8_t		data_bits_count
) : Buttons((bitmask_t (*)())&Keypad::_getKeypadBitmaskFromScans) {
	_setFromScans(
		setScanFunc,
		getDataBitmaskFunc,
		scans_count,
		data_bits_count
	);
}

Keypad::Keypad(
	uint8_t *	row_pins,
	uint8_t *	column_pins,
	uint8_t		rows_count,
	uint8_t		columns_count
) : Buttons((bitmask_t (*)())&Keypad::_getKeypadBitmaskFromPins) {
	_setFromPins(
		row_pins,
		column_pins,
		rows_count,
		columns_count
	);
}

void Keypad::begin(
	void		(* setScanFunc)(uint8_t scan),
	bitmask_t	(* getDataBitmaskFunc)(),
	uint8_t		scans_count,
	uint8_t		data_bits_count
) {
	Buttons::begin((bitmask_t (*)())&Keypad::_getKeypadBitmaskFromScans);
	_setFromScans(
		setScanFunc,
		getDataBitmaskFunc,
		scans_count,
		data_bits_count
	);
}

void Keypad::begin(
	uint8_t *	row_pins,
	uint8_t *	column_pins,
	uint8_t		rows_count,
	uint8_t		columns_count
) {
	Buttons::begin((bitmask_t (*)())&Keypad::_getKeypadBitmaskFromPins);
	_setFromPins(
		row_pins,
		column_pins,
		rows_count,
		columns_count
	);
}

void Keypad::_setFromScans(
	void		(* setScanFunc)(uint8_t scan),
	bitmask_t	(* getDataBitmaskFunc)(),
	uint8_t		scans_count,
	uint8_t		data_bits_count
) {
	_setScanFunc = setScanFunc;
	_getDataBitmaskFunc = getDataBitmaskFunc;
	_scans_count = scans_count;
	_data_bits_count = data_bits_count;
}

void Keypad::_setFromPins(
	uint8_t *	row_pins,
	uint8_t *	column_pins,
	uint8_t		rows_count,
	uint8_t		columns_count
) {
	_rows_count = rows_count;
	_columns_count = columns_count;
	_row_pins = Buttons::_allocAndCopy(row_pins, _rows_count);
	_column_pins = Buttons::_allocAndCopy(column_pins, _columns_count);
	for (uint8_t row = 0; row < _rows_count; row++)					// set all row pins to logic "Z"
		pinMode(_row_pins[row], INPUT);
	for (uint8_t column = 0; column < _columns_count; column++)		// pull all columns pins to logic "1"
		pinMode(_column_pins[column], INPUT_PULLUP);
}

bitmask_t Keypad::_getKeypadBitmaskFromScans() {
	bitmask_t keypad_bitmask = 0;
	for (uint8_t scan = 0; scan < _scans_count; scan++) {
		_setScanFunc(scan);
		keypad_bitmask <<= _data_bits_count;
		keypad_bitmask |= _getDataBitmaskFunc();
	}
	return keypad_bitmask;
}

bitmask_t Keypad::_getKeypadBitmaskFromPins() {
	bitmask_t keypad_bitmask = 0;
	for (uint8_t row = 0; row < _rows_count; row++) {
		pinMode(_row_pins[(row ? row : _rows_count) - 1], INPUT);	// set old row pin to logic "Z"
		pinMode(_row_pins[row], OUTPUT);							// set current row pin to logic "0"
		for (uint8_t column = 0; column < _columns_count; column++)
			if (!digitalRead(_column_pins[column]))					// did a column shortened with logic "0"?
				keypad_bitmask |= (bitmask_t)1 << (row * _columns_count + column);
	}
	return keypad_bitmask;
}



Joystick::Joystick() : Buttons() { }

Joystick::~Joystick() {
	Buttons::end();
}

Joystick::Joystick(
	uint8_t		x_axis_pin,
	uint16_t	left_limit,
	uint16_t	right_limit
) : Buttons((bitmask_t (*)())&Joystick::_getJoystickBitmask1Pin) {
	_setFromPins(
		x_axis_pin,
		left_limit,
		right_limit
	);
}

Joystick::Joystick(
	uint8_t		x_axis_pin,
	uint8_t		y_axis_pin,
	uint16_t	left_limit,
	uint16_t	right_limit,
	uint16_t	up_limit,
	uint16_t	down_limit
) : Buttons((bitmask_t (*)())&Joystick::_getJoystickBitmask2Pins) {
	_setFromPins(
		x_axis_pin,
		y_axis_pin,
		left_limit,
		right_limit,
		up_limit,
		down_limit
	);
}

Joystick::Joystick(
	uint8_t		x_axis_pin,
	uint8_t		y_axis_pin,
	uint8_t		button_pin,
	uint16_t	left_limit,
	uint16_t	right_limit,
	uint16_t	up_limit,
	uint16_t	down_limit
) : Buttons((bitmask_t (*)())&Joystick::_getJoystickBitmask3Pins) {
	_setFromPins(
		x_axis_pin,
		y_axis_pin,
		button_pin,
		left_limit,
		right_limit,
		up_limit,
		down_limit
	);
}

void Joystick::begin(
	uint8_t		x_axis_pin,
	uint16_t	left_limit,
	uint16_t	right_limit
) {
	Buttons::begin((bitmask_t (*)())&Joystick::_getJoystickBitmask1Pin);
	_setFromPins(
		x_axis_pin,
		left_limit,
		right_limit
	);
}

void Joystick::begin(
	uint8_t		x_axis_pin,
	uint8_t		y_axis_pin,
	uint16_t	left_limit,
	uint16_t	right_limit,
	uint16_t	up_limit,
	uint16_t	down_limit
) {
	Buttons::begin((bitmask_t (*)())&Joystick::_getJoystickBitmask2Pins);
	_setFromPins(
		x_axis_pin,
		y_axis_pin,
		left_limit,
		right_limit,
		up_limit,
		down_limit
	);
}

void Joystick::begin(
	uint8_t		x_axis_pin,
	uint8_t		y_axis_pin,
	uint8_t		button_pin,
	uint16_t	left_limit,
	uint16_t	right_limit,
	uint16_t	up_limit,
	uint16_t	down_limit
) {
	Buttons::begin((bitmask_t (*)())&Joystick::_getJoystickBitmask3Pins);
	_setFromPins(
		x_axis_pin,
		y_axis_pin,
		button_pin,
		left_limit,
		right_limit,
		up_limit,
		down_limit
	);
}

void Joystick::_setFromPins(
	uint8_t		x_axis_pin,
	uint8_t		y_axis_pin,
	uint8_t		button_pin,
	uint16_t	left_limit,
	uint16_t	right_limit,
	uint16_t	up_limit,
	uint16_t	down_limit
) {
	_button_pin = button_pin;
	pinMode(_button_pin, INPUT_PULLUP);
	_setFromPins(
		x_axis_pin,
		y_axis_pin,
		left_limit,
		right_limit,
		up_limit,
		down_limit
	);
}

void Joystick::_setFromPins(
	uint8_t		x_axis_pin,
	uint8_t		y_axis_pin,
	uint16_t	left_limit,
	uint16_t	right_limit,
	uint16_t	up_limit,
	uint16_t	down_limit
) {
	_y_axis_pin = y_axis_pin;
	_up_limit = up_limit;
	_down_limit = down_limit;
	pinMode(_y_axis_pin, INPUT);
	_setFromPins(
		x_axis_pin,
		left_limit,
		right_limit
	);
}

void Joystick::_setFromPins(
	uint8_t		x_axis_pin,
	uint16_t	left_limit,
	uint16_t	right_limit
) {
	_x_axis_pin = x_axis_pin;
	_left_limit = left_limit;
	_right_limit = right_limit;
	pinMode(_x_axis_pin, INPUT);
}

bitmask_t Joystick::_getJoystickBitmask1Pin() {
	bitmask_t joystick_bitmask = 0;
	uint16_t axis_value = analogRead(_x_axis_pin);
	if (_left_limit < _right_limit) {
		if (axis_value < _left_limit)		joystick_bitmask = JOYSTICK_MASK_LEFT;
		else if (axis_value > _right_limit)	joystick_bitmask = JOYSTICK_MASK_RIGHT;
	}
	else {
		if (axis_value > _left_limit)		joystick_bitmask = JOYSTICK_MASK_LEFT;
		else if (axis_value < _right_limit)	joystick_bitmask = JOYSTICK_MASK_RIGHT;
	}
	return joystick_bitmask;
}

bitmask_t Joystick::_getJoystickBitmask2Pins() {
	bitmask_t joystick_bitmask = _getJoystickBitmask1Pin();
	uint16_t axis_value = analogRead(_y_axis_pin);
	if (_up_limit < _down_limit) {
		if (axis_value < _up_limit)			joystick_bitmask |= JOYSTICK_MASK_UP;
		else if (axis_value > _down_limit)	joystick_bitmask |= JOYSTICK_MASK_DOWN;
	}
	else {
		if (axis_value > _up_limit)			joystick_bitmask |= JOYSTICK_MASK_UP;
		else if (axis_value < _down_limit)	joystick_bitmask |= JOYSTICK_MASK_DOWN;
	}
	return joystick_bitmask;
}

bitmask_t Joystick::_getJoystickBitmask3Pins() {
	return _getJoystickBitmask2Pins() | (digitalRead(_button_pin) ? JOYSTICK_MASK_NONE : JOYSTICK_MASK_BUTTON);
}

bool Joystick::isButtonDown() {
	return (Buttons::getButtonsBitmask() & JOYSTICK_MASK_BUTTON) != 0;
}

joystick_direction_t Joystick::getDirection() {
	uint8_t direction_bitmask = Buttons::getButtonsBitmask() & ~JOYSTICK_MASK_BUTTON;
	if (direction_bitmask == JOYSTICK_MASK_LEFT)		return JOYSTICK_DIRECTION_LEFT;
	if (direction_bitmask == JOYSTICK_MASK_LEFT_UP)		return JOYSTICK_DIRECTION_LEFT_UP;
	if (direction_bitmask == JOYSTICK_MASK_UP)			return JOYSTICK_DIRECTION_UP;
	if (direction_bitmask == JOYSTICK_MASK_RIGHT_UP)	return JOYSTICK_DIRECTION_RIGHT_UP;
	if (direction_bitmask == JOYSTICK_MASK_RIGHT)		return JOYSTICK_DIRECTION_RIGHT;
	if (direction_bitmask == JOYSTICK_MASK_RIGHT_DOWN)	return JOYSTICK_DIRECTION_RIGHT_DOWN;
	if (direction_bitmask == JOYSTICK_MASK_DOWN)		return JOYSTICK_DIRECTION_DOWN;
	if (direction_bitmask == JOYSTICK_MASK_LEFT_DOWN)	return JOYSTICK_DIRECTION_LEFT_DOWN;
	return JOYSTICK_DIRECTION_MIDDLE;
}
