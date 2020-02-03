 #include "mbed.h"
 #include "PinDetect.h"
 #include "platform/mbed_thread.h"
 
/********** Defines **********/
#define OS_SIGNAL_WAKE_UP 0x01

/********** Function Prototypes **********/
void notification_thread();
void alertPressCallback();
void alertReleaseCallback();

void keyPressed();
void keyReleased();
void keyPressedHeld();
void keyReleasedHeld();

/********** Global Declarations **********/
PinDetect  alertButton( PC_8 );
InterruptIn alertButtonInterrupt(PC_8, PullUp);

DigitalOut pressNotification( PC_7 );
DigitalOut pressHoldNotification( PC_6 );
DigitalOut releaseHoldNotification( PB_12 );

// Cannot use PA_4 due to Temperature Sensor Output of 800mv
// which is affecting the buzzer
AnalogOut  aout(PA_4);

LowPowerTimer lpt;
bool isAlertPressed  = false;
bool isAlertReleased = false;

/********** Threads **********/
Thread thread1 (osPriorityNormal, OS_STACK_SIZE, NULL, "notificationThread");
 
int main() 
{
    // Attach Callback Functions to Alert Button Interrupt
    alertButtonInterrupt.rise(alertPressCallback);
    alertButtonInterrupt.fall(alertReleaseCallback);

    // Attach Debouncing Mechanism to Alert Button
    alertButton.mode( PullDown );
    alertButton.attach_asserted( &keyPressed );
    alertButton.attach_deasserted( &keyReleased );
    alertButton.attach_asserted_held( &keyPressedHeld );
    alertButton.attach_deasserted_held( &keyReleasedHeld );

    // Sample at 20ms
    alertButton.setSampleFrequency();

    // Start the thread
    thread1.start(notification_thread);

    while( 1 ) {
        printf("Main Thread going to sleep... forever!\n");
        ThisThread::sleep_for(osWaitForever);
    }
}

 void keyPressed() 
 {
      pressNotification = 1;
      pressHoldNotification = 0;
      releaseHoldNotification = 0;
 }
 
 void keyReleased()
 {
      pressNotification = 0;
      pressHoldNotification = 0;
      releaseHoldNotification = 0;
 }
 
 void keyPressedHeld()
 {
      pressHoldNotification = 1;
 }
 
 void keyReleasedHeld()
 {
      // Disable and Use for Buzzer
      releaseHoldNotification = 1;
 }

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
    pressNotification = 1;
}

void alertReleaseCallback()
{
    // Record the Alert Release Event and Stop the Timer
    isAlertReleased = true;
    lpt.stop();

    // If conditions are met, wakeup the notification thread
    if (isAlertPressed && isAlertReleased && lpt.read_ms()>=2000)
        osSignalSet(thread1.get_id(), OS_SIGNAL_WAKE_UP);

    // Turn Off LED notification
    pressNotification = 0;

    // Reset Everything
    isAlertPressed = false;
    isAlertReleased = false;
    lpt.reset();
}