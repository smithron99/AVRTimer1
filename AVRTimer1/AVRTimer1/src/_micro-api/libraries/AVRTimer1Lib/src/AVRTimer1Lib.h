//------------------------------------------------------------------------------------
// AVRTimer1Lib.h
// The timer class Allows usage of the 16-bit ATMega Timer1 in "Normal mode".
// That is to say the counter counts up from zero to 65536 then overflows back to 0
// The class allows for both comare registers A and B to be set, and for interrupts
// to fired for up to three events:
//	- counter match to register A
//	- counter match to register B
//	- overflow
//
// Prescale factors can configure the interval between timer ticks from the 
// native 16MHz clock speed, which ticks every 1/16 of a microsecond, down to
// 15.625 KHz, which ticks evry 1,204 microseconds.  These scaling factors are configured 
// by the Resolution enum value which is passed to the constructor.  Since the timer 
// always counts up to 65536, as the resolution becomes more coarse, the total time 
// spanned by those ticks increases, per the table below:
//
//	Resolution			Maximum period
//	----------			------------------
//	sixteenthMicro		4,096 microseconds
//	halfMicro			32,768 microseconds
//	fourMicros			262,144 microseconds
//	sixteenMicros		1,048,576 microseconds
//	sixtyFourMicros		4,194,304 microseconds
//------------------------------------------------------------------------------------

#ifndef _AVRTIMER1_h
#define _AVRTIMER1_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define setbit(reg, b) (reg) |= (1 << (b))
#define clearbit(reg, b) (reg) &= ~(1 << (b))

#define DEBUG
class Timer
{
public:
	typedef void(*isr)(void);
	static isr isrCompareA;
	static isr isrCompareB;
	static isr isrOverflow;

	// in microseconds (correlates to pre-scale factors)
	enum Resolution
	{
		none=0,
		sixteenthMicro,
		halfMicro,
		fourMicros,
		sixteenMicros,
		sixtyFourMicros
	};

	enum Register
	{
		compareA,
		compareB,
		overflow
	};

	enum Direction
	{
		left,
		right
	};

	typedef struct _Scale
	{
		byte			bits;
		int				factor;		// period will be shifted left (x2) or right (/2) this many positions to convert to ticks
		Direction		shift;		
		unsigned long	maxPeriod;	//(microseconds)
	} Scale;

	
	//---------------------------------------------------------------------
	// constructor
	// Initialize the timer with the specified resolution
	// Note the timer can only count 65536 ticks, so as
	// precision gets finer, the maximum period shrinks accordingly.
	// For example, resolution halfMicro has a maximum period of
	// 32768 microseconds, while resolution sixtyFourMicros can
	// count as high as 4,194,304 micros, in steps of 64
	// Method begin() will return the maximum period in microseconds
	// for the configured resolution
	//---------------------------------------------------------------------
	Timer(Resolution r):config(scaler[r]){}

	//---------------------------------------------------------------------
	// attachedInterrupt( register, isr, enable )
	// Configures the provided interrupt service routine 
	// to be called when the interrupt associated with 
	// 'register' is fired. If 'enable' is true, the
	// interrupt is also activated
	//---------------------------------------------------------------------
	void attachInterrupt(Register type, isr routine, boolean enable);

	//---------------------------------------------------------------------
	// begin()
	// Initializes timer registers and the timer count to zero, timer stopped.
	// Call this method first.
	//---------------------------------------------------------------------
	void begin(void);

	//---------------------------------------------------------------------
	// maxPeriod()
	// returns the maximum count, in microseconds for
	// the configured timer resolution
	//---------------------------------------------------------------------
	unsigned long maxPeriod( void )
	{
		return config.maxPeriod;
	}

	//---------------------------------------------------------------------
	// micros()
	// Return the current count, in microseconds (range 0-maxPeriod)
	//---------------------------------------------------------------------
	unsigned long micros();

	//---------------------------------------------------------------------
	// setPeriod(register, period)
	// Sets compare register A or compare register B for the
	// requested number of microseconds. If configured, 
	// interrupts will be generated when the timer count reaches these values.
	// Returns zero if successful.
	// If requested period exceeds maximum period for the given resolution, 
	// returns maximum period and does not attempt to set the register
	//--------------------------------------------------------------------- 
	long setPeriod(Register reg, long microseconds);
	
	//---------------------------------------------------------------------
	// start()
	// Zeroes the counter and starts counting
	//---------------------------------------------------------------------
	void start(void);

	//---------------------------------------------------------------------
	// stop()
	// Stops the counter
	//---------------------------------------------------------------------
	void stop(void)
	{
		TCCR1B = scaler[none].bits;
	}

	//---------------------------------------------------------------------
	// ticks()
	// Return the current count, in timer ticks (range 0-65536)
	//---------------------------------------------------------------------
	unsigned short ticks()
	{
		noInterrupts();
		unsigned short t = TCNT1;
		interrupts();
		return t;
	}

private:
	static const Scale scaler[6];
	const Scale& config;
	void enableInterrupt(Register r);
	void disableInterrupt(Register r);

	//---------------------------------------------------------------------
	// getTicks( microseconds)
	// Returns the number of ticks required to span the passed
	// number of microseconds, given the configured resolution.
	// If the passed microseconds are not evenly divisble by the 
	// resolution, the remainder will be lost. For example,
	// if resolution is set to sixtyFourMicros, and the period 
	// passed is 130 microseconds, this will return 2 ticks, 
	// spanning 2 * 64 = 128 microseconds.
	//---------------------------------------------------------------------
	unsigned short getTicks(long microseconds);
};

#endif

