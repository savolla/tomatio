/*
 * @author: Oleksiy Nehlyadyuk
 * @mail  : savolla@protonmail.com
 * @time: Sat Apr 24 01:09:51 2021
 */

// Shift Register pins (SN74HC595N)
#define SHIFTR_CLEAR                         (1)
#define SHIFTR_DATA                          (2)
#define SHIFTR_LATCH                         (3)
#define SHIFTR_CLOCK                         (4)
#define SHIFTR_DATA2                         (5)
#define SHIFTR_CLOCK2                        (6)

// Led pins
#define LED_RED                              (8)
#define LED_GREEN                            (9)
#define LED_YELLOW                           (10)

// Potentiomenter pins
#define POTENTIOMETER                        (A0)
#define POT_BUTTON                           (7)

#define BUZZER                               (11)

// variable macros
#define POT_ADJUSTMENT_THRESHOLD             (25)  // analog value
#define NOTIFICATION_NOTE                    (440) // C4 (middle C)
#define NOTIFICATION_LENGTH                  (100) // ms (set short since Buzzers sound terrible)
#define DELAY_BETWEEN_NOTIFICATIONS          (200) // ms
#define TIME_INTEVAL_BETWEEN_BUTTON_PRESSES  (200) // ms
#define SECOND                               (1000)// ms

enum NOTIFICATION {
    REST = 1,
    POMODORO_START,
    POMODORO_STOP
};

// Default Pomodoro values
unsigned char WORK_TIME = 25;// minutes
unsigned char REST_TIME = 5; // minutes
unsigned char ROUNDS    = 4;

// temporary placeholders
unsigned int initialPotValue;
unsigned char rounds;

// Values for "Common Cathode" 7-Segment Display
const unsigned char encodedSevenSegmentNumbers[10] = {
//        abcdefgP (P = Point)
/* 0 */ 0b11111100,
/* 1 */ 0b01100000,
/* 2 */ 0b11011010,
/* 3 */ 0b11110010,
/* 4 */ 0b01100110,
/* 5 */ 0b10110110,
/* 6 */ 0b00111110,
/* 7 */ 0b11100000,
/* 8 */ 0b11111110,
/* 9 */ 0b11100110
};

// function declarations
// Abstraction level 6
void pomodoroSession( void );

// Abstraction level 5
void startTimer( const unsigned char );

// Abstraction level 4
void setPomodoroValue( unsigned char* );
void showRemaining( unsigned char );

// Abstraction level 3
unsigned char adjust( unsigned char *);
void turnOffDisplay( void );

// Abstraction level 2
void ledTurnOn( unsigned char );
void displayNumber( unsigned char );
void clearPreviousNumber( void );

// Abstraction level 1
bool isPressed( unsigned char button );
bool isPotAdjusted( void );
void turnOffAllLeds( void );
void notify( NOTIFICATION );
void updateSevenSegment( void );
void sevenSegmentShowNumber( const char,
                             const char,
                             const char );

// runtime
void setup(void) {

  // Shift Register pins
  pinMode( SHIFTR_CLOCK,         OUTPUT );
  pinMode( SHIFTR_CLOCK2,        OUTPUT );
  pinMode( SHIFTR_DATA,          OUTPUT );
  pinMode( SHIFTR_DATA2,         OUTPUT );
  pinMode( SHIFTR_LATCH,         OUTPUT );
  pinMode( SHIFTR_CLEAR,         OUTPUT );
  digitalWrite( SHIFTR_CLEAR,      HIGH ); // Active Low one

  // Led pins
  pinMode( LED_RED,              OUTPUT );
  pinMode( LED_YELLOW,           OUTPUT );
  pinMode( LED_GREEN,            OUTPUT );

  // Potentiometer pins
  pinMode( POTENTIOMETER,         INPUT );
  pinMode( POT_BUTTON,            INPUT );
}

// Main Loop
void loop(void) {

  // setting Work Time
  ledTurnOn( LED_RED );
  setPomodoroValue( &WORK_TIME );

  // setting Rest Time
	ledTurnOn( LED_GREEN );
	setPomodoroValue( &REST_TIME );

  // setting Rounds
	ledTurnOn( LED_YELLOW );
	setPomodoroValue( &ROUNDS );

  if ( ROUNDS < 1 ) {
    ROUNDS = 1;
  }
	rounds = ROUNDS;

	do {
		rounds--;
		pomodoroSession();
	} while ( rounds );

	notify( POMODORO_STOP );
}

// function definitions
void pomodoroSession( void ) {
	notify( POMODORO_START );
	startTimer( WORK_TIME );
	notify( REST );
	startTimer( REST_TIME );
}

void setPomodoroValue( unsigned char *number ) {
	initialPotValue = analogRead( POTENTIOMETER );
	while ( ! isPressed( POT_BUTTON ) ) {
		if ( isPotAdjusted() ) {
			*number = adjust( number );
		}
		else {
			displayNumber( *number );
		}
	}
	delay( TIME_INTEVAL_BETWEEN_BUTTON_PRESSES );
}

void displayNumber( unsigned char minute ) {
    const char onesDigit = minute%10;
    const char tensDigit = minute/10;
    clearPreviousNumber(); // from Seven Segment
    sevenSegmentShowNumber( onesDigit, SHIFTR_DATA2, SHIFTR_CLOCK2 );
    sevenSegmentShowNumber( tensDigit, SHIFTR_DATA, SHIFTR_CLOCK );
    updateSevenSegment();
}

void sevenSegmentShowNumber( const char number,
                             const char serialDataPin,
                             const char serialClockPin ) {
    const char encodedNumber = encodedSevenSegmentNumbers[number];
    shiftOut(serialDataPin, serialClockPin, LSBFIRST, encodedNumber);
}

void clearPreviousNumber( void ) {
   digitalWrite(SHIFTR_CLEAR, LOW);
   updateSevenSegment();
   digitalWrite(SHIFTR_CLEAR, HIGH);
}

void updateSevenSegment( void ) {
    digitalWrite(SHIFTR_LATCH, HIGH);
    digitalWrite(SHIFTR_LATCH, LOW);
}

unsigned char adjust( unsigned char *number ) {
  int potValue;
  while ( ! isPressed(POT_BUTTON) ) {
        potValue = analogRead( POTENTIOMETER );
        *number = potValue/20;
        displayNumber( *number );
  }
  return *number;
}

bool isPotAdjusted( void ) {
	return ( analogRead( POTENTIOMETER ) >
	         (initialPotValue + POT_ADJUSTMENT_THRESHOLD) ||
	         analogRead( POTENTIOMETER ) <
	         (initialPotValue - POT_ADJUSTMENT_THRESHOLD) );
}

void ledTurnOn( unsigned char led ) {
	turnOffAllLeds();
	digitalWrite( led, HIGH );
}

void turnOffAllLeds( void ) {
	digitalWrite( LED_RED, LOW );
	digitalWrite( LED_YELLOW, LOW );
	digitalWrite( LED_GREEN, LOW );
}

void turnOffDisplay( void ) {
	clearPreviousNumber();
	updateSevenSegment();
}

void notify( NOTIFICATION mode ) {
	for ( char i = mode; i != 0; i--) {
		tone( BUZZER, NOTIFICATION_NOTE );
		delay( NOTIFICATION_LENGTH );
		noTone( BUZZER );
		delay( DELAY_BETWEEN_NOTIFICATIONS );
	}
}

void startTimer( unsigned char value ) {
	// turn off for power management
	turnOffDisplay();
	turnOffAllLeds();

	unsigned int _value = value * 60;

	while ( _value ) {
		if ( isPressed( POT_BUTTON )) {
			showRemaining( _value / 60 );
		}
		delay( SECOND );
		_value--;
	}
}

void showRemaining( unsigned char value ) {
	displayNumber( value );
	delay( SECOND );
	turnOffDisplay();
	value = value - SECOND;
}

bool isPressed( unsigned char button ) {
  return digitalRead( button );
}
