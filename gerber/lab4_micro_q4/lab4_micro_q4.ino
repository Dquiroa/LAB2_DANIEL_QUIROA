// Definitions
#define TEMP_PIN A0 // temp sensor
#define PWM_PIN 5 // fan speed
#define BUTTON_1 2 // on button
#define BUTTON_2 3   // changes sensitvity

// temp average
constexpr float Ave = 0.1;

// global
volatile bool fanOn = false; // is the fan on or off
volatile int sens = 1; // sensitivity setting

// stores temp reading
float temp = 25.0;           

// interrupt function
void toggleFan() { 
    fanOn = !fanOn; // inverts fan state
}

void changeSens() { 
    sens = (sens % 3) + 1; // Cycles between 1, 2, and 3
}

void setup() {
    // Set pin modes
    pinMode(PWM_PIN, OUTPUT); //sets the pwm pin
    pinMode(BUTTON_1, INPUT_PULLUP);   // resistor for button 1
    pinMode(BUTTON_2, INPUT_PULLUP); // resistor for button 2

    //atttaching interrupts to buttons
    attachInterrupt(digitalPinToInterrupt(BUTTON_1), toggleFan, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2), changeSens, FALLING);

    // starting the serial monitor
    Serial.begin(9600);
}

void loop() {
    // Static variable
    static unsigned long lastTime = 0;

    //has 0.1s has passed?
    if (millis() - lastTime >= 100) {  
        lastTime = millis(); // update time

        // Readding the sensor
        float rawTemp = (analogRead(TEMP_PIN) * 5.0 / 1023.0) * 100;  

        temp += Ave * (rawTemp - temp);  

        //telling the surrent states to the serial monitor
        Serial.print("Temp: "); 
        Serial.print(temp);
    }

    int pwmValue = fanOn ? constrain((temp - 20) * 10 / sens, 50, 255) : 0;

    analogWrite(PWM_PIN, pwmValue);

    // Print the calculated value to the monitor
    Serial.print(" | PWM: "); 
    Serial.println(pwmValue);
}