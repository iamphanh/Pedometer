# Design and Implementation of a Pedometer 

## Member
* Nguyen Thi Phuong Anh - 20020629 - UET/VNU
* Nguyen Thi Thu Hai - 20020656 - UET/VNU
  
## Requirements

### Function
When a user moves the device, it counts their steps and shows the total on the LCD screen (using STM32F103C8T6).

### Input
The MPU6056 accelerometer is used by the system as an input to count the steps.
The active pedometer can be switched between the active and stop states using SW1. If SW1 is depressed when the counter starts to run, the counter switches to the active state, and vice versa. The counter keeps counting from the number of steps shown on the LCD as it moves from the stop state to the active state.

SW2 to clear (reset) the counter result to zero.

### Output

* The system has two status outputs: 
  * when the counter is active, a green LED blinks at 1 Hz; when the system is idle, the LED turns off. The red LED turns on when the system is not functioning and goes off when it is.
  * Display the number of steps on the LCD.
* The length of time the green LED blinks is determined by a timer in the counter.
