/*
 * @author: Oleksiy Nehlyadyuk
 * @mail  : savolla@protonmail.com
 */

// Arduino pins
#define SHIFTR_OUTPUT_ENABLE     (0)
#define SHIFTR_CLEAR             (1)
#define SHIFTR_DATA              (2)
#define SHIFTR_LATCH             (3)
#define SHIFTR_CLOCK             (4)
#define SHIFTR_DATA2             (5)
#define SHIFTR_CLOCK2            (6)
#define POT_BUTTON               (7)
#define LED_RED                  (8)
#define LED_GREEN                (9)
#define LED_YELLOW               (10)
#define BUZZER                   (11)
#define POTENTIOMETER            (A0)

#define POT_ADJUSTMENT_THRESHOLD (25)

unsigned char DEFAULT_WORK_TIME = 25;// minutes
unsigned char DEFAULT_REST_TIME = 5; // minutes
unsigned char DEFAULT_ROUNDS    = 4; 

unsigned int initialPotValue;     

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
// Abstraction level 3
unsigned char adjustWorkTime( void );

// Abstraction level 2
void displayMinute( const char );

// Abstraction level 1
void sevenSegmentShowNumber( const char, const char, const char );
void clearPreviousNumber( void );
void updateSevenSegment( void );

// runtime
void setup(void) {
  // setting pin modes

  pinMode(SHIFTR_CLOCK,         OUTPUT);
  pinMode(SHIFTR_CLOCK2,        OUTPUT);
  pinMode(SHIFTR_DATA,          OUTPUT);
  pinMode(SHIFTR_DATA2,         OUTPUT);

  pinMode(SHIFTR_LATCH,         OUTPUT);
  pinMode(SHIFTR_CLEAR,         OUTPUT);
  pinMode(SHIFTR_OUTPUT_ENABLE, OUTPUT);


  // handling Active Low pins
  digitalWrite(SHIFTR_CLEAR, HIGH);

  pinMode(A0, INPUT);
  pinMode(POT_BUTTON, INPUT);
  pinMode(8, OUTPUT); 
}

void loop(void) {
	initialPotValue = analogRead( POTENTIOMETER );
	while ( ! digitalRead( POT_BUTTON ) ) {
		if ( isPotAdjusted() ) { 
			DEFAULT_WORK_TIME = adjustWorkTime();
		}
		else {
			displayMinute( DEFAULT_WORK_TIME );
		}
	}		
}

// function definitions
void displayMinute( const char minute ) {
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

unsigned char adjustWorkTime( void ) {
  int potValue;
  char workTime;
  while ( ! digitalRead(POT_BUTTON) ) {
        potValue = analogRead( POTENTIOMETER );
        workTime = potValue/20;
        displayMinute( workTime );
  }
  return workTime;
}

bool isPotAdjusted( void ) {
	return ( analogRead( POTENTIOMETER ) >
	         (initialPotValue + POT_ADJUSTMENT_THRESHOLD) || 
	         analogRead( POTENTIOMETER ) <
	         (initialPotValue - POT_ADJUSTMENT_THRESHOLD) );
}
