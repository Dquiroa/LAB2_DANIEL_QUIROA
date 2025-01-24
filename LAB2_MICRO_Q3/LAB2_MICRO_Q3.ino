// Pin definitions
const int REDL = 9;
const int GREENL = 10;
const int BLUEL = 11;
const int button1 = 7;
const int button2 = 8;

// Arrays
int arr1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int arr2[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
int result[10];

// State Variables
volatile int operation = 0;         // Selected operation (0 = Add, 1 = Subtract, 2 = Multiply)
volatile bool executeOperation = false;  // Flag to trigger execution
unsigned long lastDebounceTime = 0; // Unified debounce timer
const unsigned long debounceDelay = 200; // Debounce delay (ms)

void setup() {
  // Initialize LED pins as output
  pinMode(REDL, OUTPUT);
  pinMode(GREENL, OUTPUT);
  pinMode(BLUEL, OUTPUT);

  // Initialize button pins as input with pull-up resistors
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  // Attach interrupts for buttons
  attachInterrupt(digitalPinToInterrupt(button1), changeOperation, FALLING);
  attachInterrupt(digitalPinToInterrupt(button2), setExecuteOperation, FALLING);

  // Initialize Serial Monitor
  Serial.begin(9600);
  Serial.println("Press button 1 to select an operation, and button 2 to execute.");
}

void loop() {
  // Execute the operation if the flag is set
  if (executeOperation) {
    executeOperation = false; // Reset the flag
    performOperation();
  }
}

// Interrupt Service Routine (ISR) for changing operation
void changeOperation() {
  if (millis() - lastDebounceTime > debounceDelay) {
    lastDebounceTime = millis();
    operation = (operation + 1) % 3; // Cycle through 0, 1, 2
    updateLEDs(); // Indicate the selected operation
  }
}

// Interrupt Service Routine (ISR) to set the execute flag
void setExecuteOperation() {
  if (millis() - lastDebounceTime > debounceDelay) {
    lastDebounceTime = millis();
    executeOperation = true;
  }
}

// Perform the selected operation on arrays
void performOperation() {
  Serial.print("Performing ");
  if (operation == 0) Serial.println("Addition:");
  else if (operation == 1) Serial.println("Subtraction:");
  else if (operation == 2) Serial.println("Multiplication:");

  // Perform the calculation
  for (int i = 0; i < 10; i++) {
    if (operation == 0) result[i] = arr1[i] + arr2[i];
    else if (operation == 1) result[i] = arr1[i] - arr2[i];
    else if (operation == 2) result[i] = arr1[i] * arr2[i];
  }

  // Print results to Serial Monitor
  for (int i = 0; i < 10; i++) {
    Serial.print("result[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(result[i]);
  }

  // Blink LEDs to indicate completion
  blinkLEDs();
}

// Update LEDs to reflect the selected operation
void updateLEDs() {
  digitalWrite(REDL, operation == 0);   // Red for addition
  digitalWrite(GREENL, operation == 1); // Green for subtraction
  digitalWrite(BLUEL, operation == 2);  // Blue for multiplication
}

// Blink all LEDs to signal operation completion
void blinkLEDs() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(REDL, HIGH);
    digitalWrite(GREENL, HIGH);
    digitalWrite(BLUEL, HIGH);
    delay(200);
    digitalWrite(REDL, LOW);
    digitalWrite(GREENL, LOW);
    digitalWrite(BLUEL, LOW);
    delay(200);
  }
}