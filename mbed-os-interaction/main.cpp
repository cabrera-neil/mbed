#include "mbed.h"
//#include "cmsis_os.h"
#include "platform/mbed_thread.h"

/* Defines */
#define OS_SIGNAL_WAKE_UP 0x01

/* Function Prototypes */
//void notification_thread(void const *args);
void notification_thread();
void alertPressCallback();
void alertReleaseCallback();

/* Global Declarations */
InterruptIn alert(PC_8, PullUp);

// Use both LED and DAC for Notification
DigitalOut notification(PC_7);
AnalogOut  aout(PA_4);

LowPowerTimer lpt;
bool isAlertPressed  = false;
bool isAlertReleased = false;

/* Threads and Thread IDs */
//osThreadDef(notification_thread, osPriorityNormal, OS_STACK_SIZE);
//osThreadId thread1Id = osThreadCreate(osThread(notification_thread), NULL);
Thread thread1 (osPriorityNormal, OS_STACK_SIZE, NULL, "notificationThread");

int main (void)
{
    aout = 0.0f;

    // Bind the ISR Handlers to the Panic Button
    alert.rise(alertPressCallback);
    alert.fall(alertReleaseCallback);

    // Start the thread
    thread1.start(notification_thread);
    
    while (true) {
        printf("Main Thread going to sleep... forever!\n");
        ThisThread::sleep_for(osWaitForever);
    }
}

//void notification_thread(void const *args) {
void notification_thread() {
    while (true) {
        // Signal flags that are reported as event are automatically cleared.
        printf("Going to sleep and wait for interrupt...\n");
        osSignalWait(0x1, osWaitForever);

        printf("Waking Up\n");
        //core_util_critical_section_enter();

            // Do some important stuffs here...
            aout = 0.25f;
            ThisThread::sleep_for(1000);
            aout = 0.00f;

        //core_util_critical_section_exit();
        printf("Finished doing some important stuffs...\n");
    }
}

void alertPressCallback()
{
    // Record the Alert Press Event and Start the Timer
    isAlertPressed = true;
    lpt.start();

    // Notify the User via LED
    // Next time it Buzzer sound will be included
    notification = 1;
}

void alertReleaseCallback()
{
    // Record the Alert Release Event and Stop the Timer
    isAlertReleased = true;
    lpt.stop();

    // If conditions are met, wakeup the notification thread
    if (isAlertPressed && isAlertReleased && lpt.read_ms()>=2000)
        osSignalSet(thread1.get_id(), OS_SIGNAL_WAKE_UP);
        //osSignalSet(thread1Id, 0x1);

    // Turn Off LED notification
    // Next time it Buzzer sound will be included
    notification = 0;

    // Reset Everything
    isAlertPressed = false;
    isAlertReleased = false;
    lpt.reset();
}