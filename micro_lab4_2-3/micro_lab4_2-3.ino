/*
Q2 – You are responsible to design a solar charge controller code using your embedded platform with the
specifications below:
a) The system has a battery, a solar panel, a microcontroller, and a controllable switching element
(such as a power multiplexer or analog switch IC).
b) If the voltage output of the solar panel is greater than 2.5V, then the microcontroller power is
supplied from the solar panel.
c) If the voltage level of the solar panel is lower than 2.5V, then the battery will supply power to the
microcontroller.
d) If the voltage level of the battery drops below the voltage of the solar panel, then the switching
element connects the battery to the solar panel.
e) Provide a block diagram of this system.
f) For software purposes, since we don’t have a access to a controllable switching element, use the
OLED display to indicate how the power is routed in the system.
Q3 – The solar charge controller specifications in Q2 have a major issue, the system never has the chance
to charge the battery fully.
a) Improve the code from Q2 to allow charging the battery fully before switching the solar panels
back to the microcontroller.
[Note: if for some reason your solution in Q2 addresses this issue, disregard this question]
*/
#include <Wire.h> // I2C communication
#include <Adafruit_SSD1306.h> // OLED display

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Initialize OLED display with width, height, and I2C interface
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Define analog pins for voltage readings
#define SOLAR_PIN A0   // Analog pin to read solar panel voltage
#define BATTERY_PIN A1 // Analog pin to read battery voltage
#define SWITCH_PIN 2   // Digital pin for switching element (simulated)

const float thresholdVoltage = 2.5; // Voltage threshold for switching logic
bool isCharging = false; // Variable to track charging status

void setup() {
    pinMode(SWITCH_PIN, OUTPUT); // Set switching pin as output
    digitalWrite(SWITCH_PIN, LOW); // Initialize switching pin to LOW
    Serial.begin(9600); // Start serial communication for debugging

    // Initialize OLED display
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
}

void loop() {
    // Read and convert analog voltage from solar panel and battery
    float solarVoltage = analogRead(SOLAR_PIN) * 0.009775; // Convert ADC value to voltage
    float batteryVoltage = analogRead(BATTERY_PIN) * 0.009775; // Convert ADC value to voltage

    // Print values to serial monitor for debugging
    Serial.print("Solar: "); Serial.print(solarVoltage); Serial.print("V, ");
    Serial.print("Battery: "); Serial.print(batteryVoltage); Serial.println("V");

    display.clearDisplay(); //Clear screen
    display.setTextSize(1); //text size
    display.setTextColor(WHITE); //text color
    display.setCursor(0, 0); // cursor position
    
    // Determine power source and charging status
    if (solarVoltage > thresholdVoltage) {
        if (isCharging || batteryVoltage < solarVoltage) {
            isCharging = true; // charging mode
            digitalWrite(SWITCH_PIN, HIGH); // battery charging
            display.println("Charging from Solar"); // read message
        } else {
            digitalWrite(SWITCH_PIN, LOW); // Switch power
            display.println("MCU on Solar"); // display message
        }
    } else {
        isCharging = false; // Stop charging
        digitalWrite(SWITCH_PIN, LOW); // Switch power to battery
        display.println("MCU on Battery"); // Display message
    }

    display.display(); // Update OLED screen
    delay(1000); // Wait 1 second before next update
}
