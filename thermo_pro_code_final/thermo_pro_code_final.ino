// Include necessary libraries
#include <Wire.h>        // I2C communication
#include <OneWire.h>     // 1-Wire communication for temperature sensor
#include <DallasTemperature.h>
#include "I2Cdev.h"
#include "MPU6050.h"

// Define temperature sensor pin
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// MPU6050 (IMU) setup
MPU6050 accelgyro;  // Create an MPU6050 object
int16_t ax, ay, az, gx, gy, gz; // Variables to store accelerometer & gyroscope data

// Define button and LED pins
#define ONOFF 2      // Power button
#define LOCK 3       // Lock button
#define UNIT 4       // Unit toggle button (Celsius/Fahrenheit)
#define LIGHT 6      // Light toggle button
#define LED 9        // LED indicator

// System state variables
volatile bool onoffFlag = true;  // System ON/OFF flag
volatile bool lockFlag = true;   // Temperature lock flag
bool unitToggle = false;         // false = Fahrenheit, true = Celsius
bool lightToggle = false;        // false = LED OFF, true = LED ON
float lockedTempF, lockedTempC;  // Stores locked temperature readings

// IMU movement detection threshold (adjust if needed)
const int ACCEL_THRESHOLD = 10000;  

void setup() {
    Serial.begin(9600);  // Initialize serial communication at 9600 baud
    
    // Set up button and LED pins
    pinMode(ONOFF, INPUT_PULLUP);
    pinMode(LOCK, INPUT_PULLUP);
    pinMode(UNIT, INPUT_PULLUP);
    pinMode(LIGHT, INPUT_PULLUP);
    pinMode(LED, OUTPUT);

    // Attach interrupts for ON/OFF and LOCK buttons
    attachInterrupt(digitalPinToInterrupt(ONOFF), isr_onoff, RISING);
    attachInterrupt(digitalPinToInterrupt(LOCK), isr_lock, RISING);

    // Initialize temperature sensor
    sensors.begin();

    // Initialize MPU6050 (IMU)
    Wire.begin();
    accelgyro.initialize();

    // Check MPU6050 connection and print status
    if (accelgyro.testConnection()) {
        Serial.println("MPU6050 connected.");
    } else {
        Serial.println("MPU6050 connection failed. Check wiring!");
    }

    Serial.println("System Ready. Type 'help' for commands.");
}

void loop() {
    if (onoffFlag) {  // Run only if the system is ON
        // Read temperature sensor values
        sensors.requestTemperatures();
        float tempF = sensors.getTempFByIndex(0);
        float tempC = (tempF - 32) * 5.0 / 9.0;

        // If lock is enabled, store the locked temperature
        if (lockFlag) {
            lockedTempF = tempF;
            lockedTempC = tempC;
        }

        // Handle button actions for light and unit toggling
        handleButtons();

        // Read accelerometer and gyroscope data from MPU6050
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        processIMUData();  // Determine orientation & detect movement

        // Display data 
        updateSerialMonitor(tempF, tempC);

        // Check for user commands via Serial Monitor
        handleUserCommands();
    }
}

// process IMU data
void processIMUData() {
    Serial.print("Accel (X, Y, Z): ");
    Serial.print(ax); Serial.print(", ");
    Serial.print(ay); Serial.print(", ");
    Serial.println(az);

    Serial.print("Gyro (X, Y, Z): ");
    Serial.print(gx); Serial.print(", ");
    Serial.print(gy); Serial.print(", ");
    Serial.println(gz);

    // Determine device orientation based on accelerometer data
    if (az > 0) {
        Serial.println("Device is upright.");
    } else {
        Serial.println("Device is upside down.");
    }

    // Basic motion detection based on acceleration values
    if (abs(ax) > ACCEL_THRESHOLD || abs(ay) > ACCEL_THRESHOLD || abs(az) > ACCEL_THRESHOLD) {
        Serial.println("⚠️ Movement detected! ⚠️");
    }
}

// update the serial monitor
void updateSerialMonitor(float tempF, float tempC) {
    Serial.println("\n--- Temperature Probe ---");
    
    Serial.print("Temperature: ");
    Serial.print(lockFlag ? (unitToggle ? lockedTempC : lockedTempF) : (unitToggle ? tempC : tempF));
    Serial.println(unitToggle ? " °C" : " °F");

    Serial.print("Unit: ");
    Serial.println(unitToggle ? "Celsius" : "Fahrenheit");

    Serial.print("Light: ");
    Serial.println(lightToggle ? "ON" : "OFF");

    Serial.print("Lock: ");
    Serial.println(lockFlag ? "Locked" : "Unlocked");

    Serial.println("Type 'toggle unit', 'toggle lock', or 'toggle light' to interact.");
}

// Function to handle Serial Monitor commands
void handleUserCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n'); // Read user input
        command.trim(); // Remove leading/trailing spaces

        if (command == "toggle unit") {
            unitToggle = !unitToggle;
            Serial.println("Unit toggled.");
        } else if (command == "toggle lock") {
            lockFlag = !lockFlag;
            Serial.println("Lock toggled.");
        } else if (command == "toggle light") {
            lightToggle = !lightToggle;
            digitalWrite(LED, lightToggle ? HIGH : LOW);
            Serial.println("Light toggled.");
        } else if (command == "help") {
            Serial.println("\nAvailable Commands:");
            Serial.println("- 'toggle unit'  : Switch between Celsius and Fahrenheit.");
            Serial.println("- 'toggle lock'  : Lock/unlock temperature reading.");
            Serial.println("- 'toggle light' : Turn LED light ON/OFF.");
        } else {
            Serial.println("Unknown command. Type 'help' for a list of commands.");
        }
    }
}

// Interrupt service routine for the ON/OFF button
void isr_onoff() {
    if (digitalRead(ONOFF) == HIGH) {  
        onoffFlag = !onoffFlag;  // Toggle system ON/OFF
        Serial.println(onoffFlag ? "System ON" : "System OFF");
    }
}

// Interrupt service routine for the LOCK button
void isr_lock() {
    if (digitalRead(LOCK) == HIGH) {  
        lockFlag = !lockFlag;  // Toggle lock state
        Serial.println("Lock status changed.");
    }
}

// Function to handle button interactions with debouncing
void handleButtons() {
    static unsigned long lastUnitPress = 0;
    static unsigned long lastLightPress = 0;
    const unsigned long debounceDelay = 200; 

    // Handle LIGHT button press
    if (digitalRead(LIGHT) == LOW) {
        if (millis() - lastLightPress > debounceDelay) {
            lastLightPress = millis();
            lightToggle = !lightToggle;
            digitalWrite(LED, lightToggle ? HIGH : LOW);
        }
    }

    // Handle UNIT button press
    if (digitalRead(UNIT) == LOW) {
        if (millis() - lastUnitPress > debounceDelay) {
            lastUnitPress = millis();
            unitToggle = !unitToggle;
        }
    }
}
