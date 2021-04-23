/*
 * @author: Oleksiy Nehlyadyuk
 * @mail  : savolla@protonmail.com
 * @time: Fri Apr 23 20:09:50 2021
 */

// Arduino pins
#define SHIFTR_OUTPUT_ENABLE                 (0)
#define SHIFTR_CLEAR                         (1)
#define SHIFTR_DATA                          (2)
#define SHIFTR_LATCH                         (3)
#define SHIFTR_CLOCK                         (4)
#define SHIFTR_DATA2                         (5)
#define SHIFTR_CLOCK2                        (6)

#define POT_BUTTON                           (7)

#define LED_RED                              (8)
#define LED_GREEN                            (9)
#define LED_YELLOW                           (10)

#define BUZZER                               (11)
#define POTENTIOMETER                        (A0)

// variable macros
#define POT_ADJUSTMENT_THRESHOLD             (25)  // analog value
#define NOTIFICATION_NOTE                    (440) // C4 (middle C)
#define NOTIFICATION_LENGTH                  (100) // ms (since Buzzers sound terrible)
#define DELAY_BETWEEN_NOTIFICATIONS          (200) // ms
#define TIME_INTEVAL_BETWEEN_BUTTON_PRESSES  (200) // ms
#define SECOND                               (1000)// ms

enum NOTIFICATION {
    REST = 1,
    POMODORO_START,
    POMODORO_STOP
};

// Pomodoro related GLOBAL variables
unsigned char WORK_TIME = 25;// minutes
unsigned char REST_TIME = 5; // minutes
unsigned char ROUNDS    = 4;

unsigned int initialPotValue;
unsigned char rounds;

// Values for "Common Cathode" 7-Segment Display
const unsigned char encodedSevenSegmentNumbers[10] = {
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
// Abstraction level 4
void pomodoroSession( void );
void setPomodoroValue( unsigned char* );

// Abstraction level 3
unsigned char adjustWorkTime( void );
void ledTurnOn( unsigned char );
void turnOffAllLeds( void );
void notify( NOTIFICATION );
void startTimer( const unsigned char );
void showRemaining( unsigned char );
unsigned char adjust( unsigned char *);

// Abstraction level 2
void displayNumber( unsigned char );
void turnOffDisplay( void );

// Abstraction level 1
void clearPreviousNumber( void );
void updateSevenSegment( void );
void sevenSegmentShowNumber( const char,
                             const char,
                             const char );

// TODO: delete OUTPUT_ENABLE and free pin 0
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
  pinMode( SHIFTR_OUTPUT_ENABLE, OUTPUT );

  // Led pins
  pinMode( LED_RED,              OUTPUT );
  pinMode( LED_YELLOW,           OUTPUT );
  pinMode( LED_GREEN,            OUTPUT );

  // Potentiometer pins
  pinMode( POTENTIOMETER,         INPUT );
  pinMode( POT_BUTTON,            INPUT );
}

// TODO: don't let user to set 0 rounds. set minimum value to 1
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
	while ( ! digitalRead( POT_BUTTON ) ) {
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
  while ( ! digitalRead(POT_BUTTON) ) {
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

// TODO: write isPressed() function for buttons

void startTimer( unsigned char value ) {
	// turn off for power management
	turnOffDisplay();
	turnOffAllLeds();

	unsigned int _value = value * 60;

	while ( _value ) {
		if ( digitalRead( POT_BUTTON )) {
			showRemaining( _value / 60 );
		}
		delay( 1000 );
		_value--;
	}
}

void showRemaining( unsigned char value ) {
	displayNumber( value );
	delay( 1000 );
	turnOffDisplay();
	value = value - 1000;
}
