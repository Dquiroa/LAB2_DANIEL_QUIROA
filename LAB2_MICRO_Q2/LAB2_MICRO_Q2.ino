#include <Arduino.h>

// Pin definitions for LEDs and button
#define RED_LED_PIN 7
#define GREEN_LED_PIN 8
#define BLUE_LED_PIN 9
#define BUTTON_PIN 2

// State variables
volatile bool buttonPressed = false;
volatile bool sequenceRunning = false;
unsigned long lastMillis = 0;
const unsigned long interval = 500; // Delay for LED sequence

// Interrupt service routine for button press
void handleButtonPress() {
    buttonPressed = true;
}

// Function to control LEDs in the sequence
void runLEDSequence() {
    static uint8_t step = 0;
    uint8_t ledState = 0;

    // Generate LED state for each step
    switch (step) {
        case 0: ledState = 0b100; break; // R
        case 1: ledState = 0b110; break; // RG
        case 2: ledState = 0b111; break; // RGB
        case 3: ledState = 0b011; break; // GB
        case 4: ledState = 0b001; break; // B
        case 5: ledState = 0b101; break; // RB
    }

    // Set LED pins based on bit states
    bitWrite(PORTD, RED_LED_PIN - 2, (ledState >> 2) & 0x01);
    bitWrite(PORTD, GREEN_LED_PIN - 2, (ledState >> 1) & 0x01);
    bitWrite(PORTD, BLUE_LED_PIN - 2, ledState & 0x01);

    // Move to the next step
    step = (step + 1) % 6;
}

void setup() {
    // Initialize LEDs and button pin
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Attach interrupt to the button
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);
}

void loop() {
    // Handle button press in the main loop (since interrupt toggles only the flag)
    if (buttonPressed) {
        buttonPressed = false;
        sequenceRunning = !sequenceRunning;
        if (!sequenceRunning) {
            // If sequence is turned off, ensure LEDs are off
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, LOW);
            digitalWrite(BLUE_LED_PIN, LOW);
        }
    }

    // Run the LED sequence at a fixed interval if enabled
    if (sequenceRunning && (millis() - lastMillis >= interval)) {
        lastMillis = millis();
        runLEDSequence();
    }
}