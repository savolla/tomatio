#define SHIFTR_CLOCK         (4)
#define SHIFTR_LATCH         (3)
#define SHIFTR_DATA          (2)
#define SHIFTR_CLEAR         (1)
#define SHIFTR_OUTPUT_ENABLE (0)

const char encodedSevenSegmentNumbers[10] = {
/* 0 */ 64,
/* 1 */ 121,
/* 2 */ 36,
/* 3 */ 48,
/* 4 */ 25,
/* 5 */ 18,
/* 6 */ 3,
/* 7 */ 120,
/* 8 */ 128,
/* 9 */ 24
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
        delay(1000);
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
