/*#define bitsize 12

float Vref = 3.6;
float Vin = 3.3;
float thresh;
float quantized = 0;
int count;
int bitval;
int bits[bitsize];

int main (void){
  Vref /= 2;
  thresh = Vref;

  for(count = 0; count<bitsize; count++){
    Vref /= 2;
    if(Vin >= thresh){
      bitval = 1;
      thresh += Vref;}
    else {
      bitval = 0;
      thresh -= Vref;}
    bits[count] = bitval;
    quantized += 2*Vref*bitval;}
  while(1);
}
*/
#include <stdio.h>

#define BITSIZE 14   // 14-bit ADC resolution
#define VREF 3.0     // Reference voltage (3V)

// Function to simulate a 14-bit SAR ADC conversion
int SAR_ADC(float Vin) {
    int digital_output = 0;   // Stores the final digital output (14-bit)
    float threshold = VREF / 2; // Initial threshold (midpoint)

    // Perform the SAR (Successive Approximation) ADC conversion
    for (int i = BITSIZE - 1; i >= 0; i--) {
        if (Vin >= threshold) {  
            digital_output |= (1 << i);  // Set bit if Vin is above threshold
            threshold += VREF / (1 << (i + 1)); // Increase threshold
        } else {
            threshold -= VREF / (1 << (i + 1)); // Decrease threshold
        }
    }
    return digital_output; // Return final 14-bit digital result
}

// Function to test the SAR ADC with predefined voltage values
void test_ADC() {
    float test_voltages[] = {0.0, 0.42, 0.83, 1.65, 2.752, VREF}; // Added 0V and VREF for edge cases
    int num_tests = sizeof(test_voltages) / sizeof(test_voltages[0]);

    printf("---------------------------------------------------------\n");
    printf("|   Vin (V)   | Dout |  Q (V)  |\n");
    printf("---------------------------------------------------------\n");

    for (int i = 0; i < num_tests; i++) {
        int digital_value = SAR_ADC(test_voltages[i]); // Convert Vin to 14-bit digital
        float quantized_value = (digital_value * VREF) / ((1 << BITSIZE) - 1); // Convert back to analog

        // Print formatted results
        printf("|  %7.3f V  |  %6d       |   %7.5f V  |\n",
               test_voltages[i], digital_value, quantized_value);
    }
    printf("---------------------------------------------------------\n");
}

// Main function
int main() {
    test_ADC();  // Run ADC test cases
    return 0;    // Exit successfully
}
