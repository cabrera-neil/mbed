/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "platform/mbed_thread.h"

// Blinking rate in milliseconds
#define INTERVAL 500


int main()
{
    AnalogOut dac(PA_4);
    AnalogIn adc(PA_0);

    while (true) {
        dac = 0.25f;
        printf("Analog Reading: %f\n", adc.read());
        ThisThread::sleep_for(INTERVAL);
    }
}
