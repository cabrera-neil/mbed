// /* mbed Microcontroller Library
//  * Copyright (c) 2019 ARM Limited
//  * SPDX-License-Identifier: Apache-2.0
//  */

// #include "mbed.h"
// #include "platform/mbed_thread.h"


// // Blinking rate in milliseconds
// #define BLINKING_RATE_MS                                                    5000

// // Test the DAC
// AnalogOut buzz(PA_4);

// int main()
// {
//     // Initialise the digital pin LED1 as an output
//     DigitalOut led(LED1);

//     while (true) {
//         led = !led;
//         buzz.write(0.50);
//         thread_sleep_for(BLINKING_RATE_MS);
//         buzz.write(0.00);
//     }
// }


#include "mbed.h"
 
// Initialize a pins to perform analog and digital output fucntions
AnalogOut  aout(PA_4);
DigitalOut dout(LED1);
 
int main(void)
{
    while (1) {
        // change the voltage on the digital output pin by 0.1 * VCC
        //  and print what the measured voltage should be (assuming VCC = 3.3v)
        for (float i = 0.0f; i < 1.0f; i += 0.1f) {
            //aout = i;
            aout = 0;
            printf("aout = %1.2f volts\n", aout.read() * 3.3f);
            // turn on the led if the voltage is greater than 0.5f * VCC
            dout = (aout > 0.5f) ? 1 : 0;
            wait(0.5f);
        }
    }
}