/*
16-bit Timer example

Light the LED on pin 13 for 1/4 second (250,000 microseconds), 
every 2 seconds (2,000,000 microseconds). Since timer 1 is a 16-bit timer
it can count as high as 65,536 (2 to the power of 16). 
The timer resolution allows you to select how long each of those 
timer ticks take, up to 64 microseconds. 

With a resolution of 64 microseconds, we can count as high as 
65,536 * 64 = 4,194,304 microseconds, or 4.194304 seconds. The available
timer resolutions and associated maximum time are listed in AVRTimer1Lib.h:

	Resolution			Maximum period
	----------			------------------
	sixteenthMicro		4,096 microseconds
	halfMicro			32,768 microseconds
	fourMicros			262,144 microseconds
	sixteenMicros		1,048,576 microseconds
	sixtyFourMicros		4,194,304 microseconds

The circuit:
* no wiring required.  We're using the pre-wired LED on pin 13

Created 25 March, 2017
By Ron Smith
*/

#include <AVRTimer1Lib.h>

//declare a timer, with a timing resolution of 64 microseconds per timer tick
Timer myTimer(Timer::Resolution::sixtyFourMicros);

// define our timings
const unsigned long WAIT_TIME	= 2000000;		//2 million microseconds = 2 seconds
const unsigned long	ON_TIME		= 250000;		//250,000 microseconds = 1/4 second

//this function will be called when WAIT_TIME has elapsed
void turnOnLED(void)
{
	digitalWrite(LED_BUILTIN, HIGH);
}

//this function will be called when WAIT_TIME + ON_TIME has elapsed
void turnOffLED(void)
{
	digitalWrite(LED_BUILTIN, LOW);

	// re-start the timer at zero
	myTimer.start();
}

void setup()
{
	// configure the LED pin as output
	pinMode(LED_BUILTIN, OUTPUT);

	// initiailize the timer
	myTimer.begin();

	// set 'register A' to the period of time to wait
	myTimer.setPeriod(Timer::Register::compareA, WAIT_TIME);
	// Tell the timer to call turnOnLED when the timer has counted to the value in register A
	// Passing 'true' as the last parameter activates the interrupt
	myTimer.attachInterrupt(Timer::Register::compareA, turnOnLED, true);

	// set 'register B' to the time to turn the LED off
	myTimer.setPeriod(Timer::Register::compareB, WAIT_TIME + ON_TIME);
	// Tell the timer to call turnOffLED when the timer counts to the value in register B
	myTimer.attachInterrupt(Timer::Register::compareB, turnOffLED, true);

	// go!
	myTimer.start();
}

void loop()
{

  /* nothing needed here */

}
