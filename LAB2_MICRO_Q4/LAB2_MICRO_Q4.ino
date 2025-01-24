#include <avr/pgmspace.h>

// Store array `x` in Flash memory to save SRAM
const int8_t x[10] PROGMEM = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // Array stored in Flash
volatile int8_t y[10]; // Array in SRAM for results (8-bit integers save space)

const int buttonPin = 2; // Button connected to pin 2

// Interrupt Service Routine (ISR) to calculate array `y`
void calculateY() {
  noInterrupts(); // Temporarily disable interrupts for atomic access

  // Compute y[0] separately
  y[0] = 2 * pgm_read_byte(&x[0]);

  // Compute remaining y[n] values
  for (int n = 1; n < 10; n++) {
    y[n] = (pgm_read_byte(&x[n]) << 1) - pgm_read_byte(&x[n - 1]); // Use bit-shifting for multiplication
  }

  interrupts(); // Re-enable interrupts
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Configure button pin with pull-up resistor
  attachInterrupt(digitalPinToInterrupt(buttonPin), calculateY, FALLING); // Attach ISR to button

  Serial.begin(9600); // Start serial communication for debugging
}

void loop() {
  // Print array `y` to the Serial Monitor
  Serial.println("Array y:");
  for (int n = 0; n < 10; n++) {
    Serial.print(y[n]);
    Serial.print(" ");
  }
  Serial.println();

  delay(1000); // Delay to reduce printing frequency
}