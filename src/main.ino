#define SHIFTR_CLOCK         (4)
#define SHIFTR_LATCH         (3)
#define SHIFTR_DATA          (2)
#define SHIFTR_CLEAR         (1)
#define SHIFTR_OUTPUT_ENABLE (0)

const char encodedSevenSegmentNumbers[10] = {
/* 0 */ 63,
/* 1 */ 6,
/* 2 */ 91,
/* 3 */ 79,
/* 4 */ 102,
/* 5 */ 109,
/* 6 */ 124,
/* 7 */ 7,
/* 8 */ 255,
/* 9 */ 103,
};

// function declarations
void sevenSegmentShowNumber( const char );
void clearPreviousNumber( void );
void updateSevenSegment( void );

// runtime
void setup(void) {
    // setting pin modes
	pinMode(SHIFTR_CLOCK,         OUTPUT);
	pinMode(SHIFTR_LATCH,         OUTPUT);
	pinMode(SHIFTR_DATA,          OUTPUT);
	pinMode(SHIFTR_CLEAR,         OUTPUT);
	pinMode(SHIFTR_OUTPUT_ENABLE, OUTPUT);

    // handling Active Low pins
    digitalWrite(SHIFTR_CLEAR, HIGH);  
}

void loop(void) {
    for ( char i = 0; i < 10; i++ ) {
        sevenSegmentShowNumber( i );
	   delay(500);
    }
}

// function definitions
void sevenSegmentShowNumber( const char number ) {
    const char encodedNumber = encodedSevenSegmentNumbers[ number ];
    clearPreviousNumber(); // from Seven Segment
    shiftOut(SHIFTR_DATA, SHIFTR_CLOCK, MSBFIRST, encodedNumber);
    updateSevenSegment();
}

void clearPreviousNumber( void ) {
   digitalWrite(SHIFTR_CLEAR, LOW);
   updateSevenSegment();
   digitalWrite(SHIFTR_CLEAR, HIGH);
}

void updateSevenSegment() {
    digitalWrite(SHIFTR_LATCH, HIGH);
    digitalWrite(SHIFTR_LATCH, LOW);
}
