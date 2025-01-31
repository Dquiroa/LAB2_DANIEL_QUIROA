// Definitions 
#define GREENL 4  //pin for green led
#define REDL 5     //pin for the red led
#define BUTTON 2     //pin for the button

#define TIMER1_1HZ 62499 //256 prescaler 1hz -1
#define TIMER2_10HZ 249  // 256 prescaler 10hz - 1

// ISR counters at respective frequencies
static volatile uint32_t gCounter1 = 0;   //counter for the first timer
static volatile uint32_t gCounter10 = 0;   //counter for the second timer

// Flags for button press and main task
static volatile int buttonFlag = 0;   //flag for the button
static volatile int mainFlag = 0;     //flag for the main task

void setup() {
    // I/O setup
    Serial.begin(9600);   //initialize serial communication
    pinMode(GREENL, OUTPUT);  //setting the green as a output
    pinMode(REDL, OUTPUT);    //setting the red as a output
    pinMode(BUTTON, INPUT);   //setting the button as a input

    noInterrupts();  // Disable interrupts during setup

    // TIMER 0 setup (8-bit) - Rollover at 250Hz, scales to 10Hz in ISR
    TCCR0A = 0;
    TCCR0B = 0;
    OCR0A = TIMER2_10HZ; // Setting the timer to rollover
    TCCR0B |= (1 << CS02);  // Prescaling the clock by 256 (62500 Hz)
    TCCR0A |= (1 << WGM01); // Setting the timer to CTC mode

    // TIMER 1 setup (16-bit) - Rollover at 1Hz
    TCCR1A = 0;  //clear timer0 control register A
    TCCR1B = 0;   //clear timer0 control register B
    OCR1A = TIMER1_1HZ; // Sets timer to rollover at 62500Hz
    TCCR1B |= (1 << WGM12); // Sets timer to CTC mode
    TCCR1B |= (1 << CS12);  // Prescales clock

    // Enable interrupts for Timer 0 and Timer 1
    TIMSK0 |= (1 << OCIE0A);  //enable timer0 compare interrupt 
    TIMSK1 |= (1 << OCIE1A);  // enable timer1 compare interrupt

    interrupts();  // Enable interrupts
}

void loop() {
    //when the button is pressed the red light will turn on
    if (buttonFlag) {
        digitalWrite(REDL, HIGH);
    } else if (digitalRead(REDL)) {
        digitalWrite(REDL, LOW);
    }
    //if the main flag is set every 3 seconds send a message 
    if (mainFlag) {
        Serial.println("Kevin's Knee!!");
        mainFlag = 0;
    }
}
//ISR for timer0 for the Red
ISR(TIMER0_COMPA_vect) {
    gCounter250++;
    
    if (gCounter250 % 25 == 0) {
        if (digitalRead(BUTTON)) {
            buttonFlag = 1;
        } else {
            buttonFlag = 0;
        }
    }
}

//ISR for timer 1 for the green
ISR(TIMER1_COMPA_vect) {
    gCounter1++;

    if (gCounter1 % 2 == 1) {
        digitalWrite(GREENL, HIGH);
    } else {
        digitalWrite(GREENL, LOW);
    }

    if (gCounter1 % 3 == 0) {
        mainFlag = 1;
    }
}
