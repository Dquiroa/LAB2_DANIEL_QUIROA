// Pin Definitions
#define BUTTON1_PIN 2  // Button 1 connected to interrupt pin 2
#define BUTTON2_PIN 3  // Button 2 connected to interrupt pin 3
#define RED_LED_PIN 4  // Red LED (Button 1 pressed more)
#define GREEN_LED_PIN 5 // Green LED (Button 2 pressed more)
#define BLUE_LED_PIN 6  // Blue LED (Counts are equal)

// Use smaller data types for counters to save SRAM
volatile uint8_t button1Count = 0; // 1 byte (instead of int)
volatile uint8_t button2Count = 0; // 1 byte (instead of int)

void setup() {
  // Configure button pins as input with internal pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  // Configure LED pins as outputs
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  // Initialize LEDs (start with blue, indicating counts are equal)
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, HIGH);

  // Attach interrupts to the button pins
  attachInterrupt(digitalPinToInterrupt(BUTTON1_PIN), button1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), button2ISR, FALLING);

  // Start serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Calculate the difference between button presses
  int8_t difference = button1Count - button2Count;

  // LED Control Logic
  if (difference > 0) {
    // Button 1 pressed more
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, LOW);
  } else if (difference == 0) {
    // Equal counts
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, HIGH);
  } else {
    // Button 2 pressed more
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(BLUE_LED_PIN, LOW);
  }

  // Output counts to the Serial Monitor for debugging
  Serial.print("Button 1 Count: ");
  Serial.print(button1Count);
  Serial.print(" | Button 2 Count: ");
  Serial.print(button2Count);
  Serial.print(" | Difference: ");
  Serial.println(difference);

  // Small delay for better serial readability
  delay(200);
}

// ISR for Button 1
void button1ISR() {
  button1Count++;
}

// ISR for Button 2
void button2ISR() {
  button2Count++;
}