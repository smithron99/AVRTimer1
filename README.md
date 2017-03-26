# AVRTimer1
A 16-bit timer for the ATMega AVR platform

The timer class allows usage of the 16-bit ATMega Timer1 in "Normal mode".
That is to say the counter counts up from 0 to 65536 then overflows back to 0
The class allows for both compare registers A and B to be set, and for interrupts
to fired for up to three events:
- counter match to register A
- counter match to register B
- overflow (counter reaches 65536)

Prescale factors can configure the interval between timer ticks from the 
native 16MHz clock speed, which ticks every 1/16 of a microsecond, down to
15.625 KHz, which ticks every 1,024 microseconds.  These scaling factors are configured 
by the Resolution enum value which is passed to the constructor.  Since the timer 
always counts up to 65536, as the resolution becomes more coarse, the total time 
spanned by those ticks increases, per the table below:

|Resolution			|  Maximum period |
|----------			|  ------------------|
|sixteenthMicro	|	    4,096 microseconds|
|halfMicro			|       32,768 microseconds|
|fourMicros			|    262,144 microseconds|
|sixteenMicros	|	  1,048,576 microseconds|
|sixtyFourMicros|		4,194,304 microseconds|

NOTE: This class is specifically configured to work with a 16MHz clock, and does not attempt to adapt to other clock speeds
