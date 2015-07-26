#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <Arduino.h>

#define DEFAULT_SHORT_PRESS_MILLIS	(50)
#define DEFAULT_LONG_PRESS_MILLIS	(1000)
#define DEFAULT_PULSE_PRESS_MILLIS	(500)

#define JOYSTICK_MASK_NONE			(0x00)
#define JOYSTICK_MASK_LEFT			(0x01)
#define JOYSTICK_MASK_RIGHT			(0x02)
#define JOYSTICK_MASK_UP			(0x04)
#define JOYSTICK_MASK_DOWN			(0x08)
#define JOYSTICK_MASK_BUTTON		(0x10)
#define JOYSTICK_MASK_LEFT_UP		(JOYSTICK_MASK_LEFT | JOYSTICK_MASK_UP)
#define JOYSTICK_MASK_LEFT_DOWN		(JOYSTICK_MASK_LEFT | JOYSTICK_MASK_DOWN)
#define JOYSTICK_MASK_RIGHT_UP		(JOYSTICK_MASK_RIGHT | JOYSTICK_MASK_UP)
#define JOYSTICK_MASK_RIGHT_DOWN	(JOYSTICK_MASK_RIGHT | JOYSTICK_MASK_DOWN)

// Change the following options by making them as comments (// before)
//typedef uint8_t bitmask_t;		// 8 buttons max
//typedef uint16_t bitmask_t;		// 16 buttons max
typedef uint32_t bitmask_t;			// 32 buttons max
//typedef uint64_t bitmask_t;		// 64 buttons max

typedef enum {
	BUTTONS_STATE_UP,
	BUTTONS_STATE_BOUNCE,
	BUTTONS_STATE_SHORT_PRESS,
	BUTTONS_STATE_LONG_PRESS,
	BUTTONS_STATE_SHORT_RELEASE,
	BUTTONS_STATE_LONG_RELEASE,
	BUTTONS_STATE_LONG_PULSE,
	BUTTONS_STATES_COUNT,
} buttons_state_t;

typedef enum {
	JOYSTICK_DIRECTION_MIDDLE = 0,
	JOYSTICK_DIRECTION_LEFT,
	JOYSTICK_DIRECTION_LEFT_UP,
	JOYSTICK_DIRECTION_UP,
	JOYSTICK_DIRECTION_RIGHT_UP,
	JOYSTICK_DIRECTION_RIGHT,
	JOYSTICK_DIRECTION_RIGHT_DOWN,
	JOYSTICK_DIRECTION_DOWN,
	JOYSTICK_DIRECTION_LEFT_DOWN,
	JOYSTICK_DIRECTIONS_COUNT,
} joystick_direction_t;

typedef void (*event_t)();

class Buttons {
	public:
		Buttons();
		~Buttons();
		void end();
		Buttons(
			bitmask_t	(* getButtonsBitmaskFunc)()
		);
		Buttons(
			uint8_t *	buttons_pins,
			uint8_t		buttons_count
		);
		void begin(
			bitmask_t	(* getButtonsBitmaskFunc)()
		);
		void begin(
			uint8_t *	buttons_pins,
			uint8_t		buttons_count
		);
		
		void setShortPressMillis(
			uint32_t	short_press_millis
		);
		
		void setLongPressMillis(
			uint32_t	long_press_millis
		);
		
		void setPulsePressMillis(
			uint32_t	pulse_press_millis
		);
		
		void setPressMillis(
			uint32_t	short_press_millis		= DEFAULT_SHORT_PRESS_MILLIS,
			uint32_t	long_press_millis		= DEFAULT_LONG_PRESS_MILLIS,
			uint32_t	pulse_press_millis		= DEFAULT_PULSE_PRESS_MILLIS
		);
		
		void setShortPressFunc(
			event_t		onShortPressFunc
		);
		
		void setShortReleaseFunc(
			event_t		onShortReleaseFunc
		);

		void setLongPressFunc(
			event_t		onLongPressFunc
		);

		void setLongReleaseFunc(
			event_t		onLongReleaseFunc
		);

		void setLongPulseFunc(
			event_t		onLongPulseFunc
		);
		
		void setEventsFuncs(
			event_t		onShortPressFunc		= NULL,
			event_t		onShortReleaseFunc 		= NULL,
			event_t		onLongPressFunc			= NULL,
			event_t		onLongReleaseFunc		= NULL,
			event_t		onLongPulseFunc			= NULL
		);
		
		void setNextPulseMillis(
			uint32_t	next_pulse_millis
		);
		
		uint32_t		getPulseCount();
		uint32_t		getUpMillis();
		uint32_t		getDownMillis();
		bitmask_t		getButtonsBitmask();
		uint8_t			getButtonIndex();
		
		buttons_state_t	getState();
		
		void			update();
		void			update(uint32_t cur_millis);
					
		bool			isUp();
		bool			isDown();
					
		bool			isPressed();
		bool			isReleased();
		bool			isPulsed();
		
	protected:
		uint8_t *		_allocAndCopy(uint8_t * src, uint8_t size);
	
	private:
		static void		_nothing() {};
		uint8_t *		_buttons_pins					= NULL;
		uint8_t			_buttons_count;
		bitmask_t		_buttons_bitmask;
		bitmask_t		_getButtonsBitmaskFromPins();
		bitmask_t		(* _getButtonsBitmaskFunc)()	= NULL;
		uint32_t		_short_press_millis				= DEFAULT_SHORT_PRESS_MILLIS;
		uint32_t		_long_press_millis				= DEFAULT_LONG_PRESS_MILLIS;
		uint32_t		_pulse_press_millis				= DEFAULT_PULSE_PRESS_MILLIS;
		event_t			_onShortPressFunc				= _nothing;
		event_t			_onShortReleaseFunc 			= _nothing;
		event_t			_onLongPressFunc				= _nothing;
		event_t			_onLongReleaseFunc				= _nothing;
		event_t			_onLongPulseFunc				= _nothing;
		event_t			_setEventFunc(event_t func);
		void			_buttonsUp();
		uint32_t		_up_millis;
		uint32_t		_down_millis;
		uint32_t		_old_millis;
		uint32_t		_next_pulse_millis;
		uint32_t		_pulse_count;
		buttons_state_t	_buttons_state;
};



class Keypad : public Buttons {
	public:
		Keypad();
		~Keypad();
		void end();
		Keypad(
			void		(* setScanFunc)(uint8_t scan),
			bitmask_t	(* getDataBitmaskFunc)(),
			uint8_t		scans_count,
			uint8_t		data_bits_count
		);
		Keypad(
			uint8_t *	row_pins,
			uint8_t *	column_pins,
			uint8_t		rows_count,
			uint8_t		columns_count
		);
		void begin(
			void		(* setScanFunc)(uint8_t scan),
			bitmask_t	(* getDataBitmaskFunc)(),
			uint8_t		scans_count,
			uint8_t		data_bits_count
		);
		void begin(
			uint8_t *	row_pins,
			uint8_t *	column_pins,
			uint8_t		rows_count,
			uint8_t		columns_count
		);
	private:
		void			(* _setScanFunc)(uint8_t scan);
		bitmask_t		(* _getDataBitmaskFunc)();
		uint8_t			_scans_count;
		uint8_t			_data_bits_count;
		uint8_t *		_row_pins				= NULL;
		uint8_t *		_column_pins			= NULL;
		uint8_t			_rows_count;
		uint8_t			_columns_count;
		
		bitmask_t		_getKeypadBitmaskFromScans();
		bitmask_t		_getKeypadBitmaskFromPins();
		
		void			_setFromScans(
							void		(* setScanFunc)(uint8_t scan),
							bitmask_t	(* getDataBitmaskFunc)(),
							uint8_t		scans_count,
							uint8_t		data_bits_count
						);
		void			_setFromPins(
							uint8_t *	row_pins,
							uint8_t *	column_pins,
							uint8_t		rows_count,
							uint8_t		columns_count
						);
};



class Joystick : public Buttons {
	public:
		Joystick();
		~Joystick();
		void end();
		Joystick(
			uint8_t		x_axis_pin,
			uint16_t	left_limit,
			uint16_t	right_limit
		);
		Joystick(
			uint8_t		x_axis_pin,
			uint8_t		y_axis_pin,
			uint16_t	left_limit,
			uint16_t	right_limit,
			uint16_t	up_limit,
			uint16_t	down_limit
		);
		Joystick(
			uint8_t		x_axis_pin,
			uint8_t		y_axis_pin,
			uint8_t		button_pin,
			uint16_t	left_limit,
			uint16_t	right_limit,
			uint16_t	up_limit,
			uint16_t	down_limit
		);
		void begin(
			uint8_t		x_axis_pin,
			uint16_t	left_limit,
			uint16_t	right_limit
		);
		void begin(
			uint8_t		x_axis_pin,
			uint8_t		y_axis_pin,
			uint16_t	left_limit,
			uint16_t	right_limit,
			uint16_t	up_limit,
			uint16_t	down_limit
		);
		void begin(
			uint8_t		x_axis_pin,
			uint8_t		y_axis_pin,
			uint8_t		button_pin,
			uint16_t	left_limit,
			uint16_t	right_limit,
			uint16_t	up_limit,
			uint16_t	down_limit
		);
		bool isButtonDown();
		joystick_direction_t getDirection();
	private:
		uint8_t			_x_axis_pin;
		uint8_t			_y_axis_pin;
		uint8_t			_button_pin;
		uint16_t		_left_limit;
		uint16_t		_right_limit;
		uint16_t		_up_limit;
		uint16_t		_down_limit;
		
		bitmask_t			_getJoystickBitmask1Pin();
		bitmask_t			_getJoystickBitmask2Pins();
		bitmask_t			_getJoystickBitmask3Pins();
		
		void			_setFromPins(
							uint8_t		x_axis_pin,
							uint8_t		y_axis_pin,
							uint8_t		button_pin,
							uint16_t	left_limit,
							uint16_t	right_limit,
							uint16_t	up_limit,
							uint16_t	down_limit
		);
		void			_setFromPins(
							uint8_t		x_axis_pin,
							uint8_t		y_axis_pin,
							uint16_t	left_limit,
							uint16_t	right_limit,
							uint16_t	up_limit,
							uint16_t	down_limit
		);
		void			_setFromPins(
							uint8_t		x_axis_pin,
							uint16_t	left_limit,
							uint16_t	right_limit
		);
};

#endif
