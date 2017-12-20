// 
// AVRTimer1
// 

#include "AVRTimer1Lib.h"

//---------------------------------------------------------------------
// Default ISR if none is attached
//---------------------------------------------------------------------
void dummyISR(void) {}

//---------------------------------------------------------------------
// Initialize statics
//---------------------------------------------------------------------
Timer::isr Timer::isrCompareA = dummyISR;
Timer::isr Timer::isrCompareB = dummyISR;
Timer::isr Timer::isrOverflow = dummyISR;

// note this list is indexed by the Timer::Resolution enum
static const Timer::Scale Timer::scaler[6] =
{
	{ 0x00,						0, left, 0 },					// timer stopped
	{ _BV(CS10),				4, left, 4096 },				// factor 1 - no scaling, resolution sixteenthMicro
	{ _BV(CS11),				1, left, 32768 },				// factor 8 - resolution halfMicro
	{ _BV(CS11) | _BV(CS10),	2, right, 262144 },				// factor 64 - resolution fourMicros
	{ _BV(CS12),				4, right, 1048576 },			// factor 256 - resolution sixteenMicros
	{ _BV(CS12) | _BV(CS10),	6, right, 4194304 }				// factor 1024 - resolution sixtyFourMicros
};

//---------------------------------------------------------------------
// attachInterrupt( register, isr, enableFlag )
//---------------------------------------------------------------------
void Timer::attachInterrupt(Register type, isr i, boolean enable)
{
	switch (type)
	{
	case compareA:
		noInterrupts();
		isrCompareA = i;
		if (enable)
		{
			setbit(TIMSK1, OCIE1A);	//enable interrupt
		}
		interrupts();
		break;
	case compareB:
		noInterrupts();
		isrCompareB = i;
		if (enable)
		{
			setbit(TIMSK1, OCIE1B);	//enable interrupt
		}
		interrupts();
		break;
	case overflow:
		noInterrupts();
		isrOverflow = i;
		if (enable)
		{
			setbit(TIMSK1, TOIE1);	//enable interrupt
		}
		interrupts();
		break;
	};
}

//---------------------------------------------------------------------
// begin()
//---------------------------------------------------------------------
void Timer::begin(void)
{
	noInterrupts();
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TCNT1 = 0x0000;
	TIMSK1 = 0x00;
	interrupts();
}

//---------------------------------------------------------------------
// disableInterrupt()
//---------------------------------------------------------------------
void Timer::disableInterrupt(Register r)
{
	switch (r)
	{
	case compareA:
		clearbit(TIMSK1, OCIE1A);
		break;
	case compareB:
		clearbit(TIMSK1, OCIE1B);
		break;
	case overflow:
		clearbit(TIMSK1, TOIE1);
		break;
	};
}

//---------------------------------------------------------------------
// enableInterupt( register )
//---------------------------------------------------------------------
void Timer::enableInterrupt(Register r)
{
	switch (r)
	{
	case compareA:
		setbit(TIMSK1, OCIE1A);
		break;
	case compareB:
		setbit(TIMSK1, OCIE1B);
		break;
	case overflow:
		setbit(TIMSK1, TOIE1);
		break;
	};
}

//---------------------------------------------------------------------
// getTicks( microseconds )
// convert microseconds to ticks, given the configured resolution
//---------------------------------------------------------------------
unsigned short Timer::getTicks(unsigned long microseconds)
{
	unsigned long result;

	if (config.shift == left)
	{
		// multiply by 2 to the exponent (factor)
		result = (unsigned long)(microseconds << config.factor);
	}
	else
	{
		// divide by 2 to the exponent (factor)
		result = (unsigned long)(microseconds >> config.factor);
	}
	return (unsigned short)result;
}

//---------------------------------------------------------------------
// micros()
// Return the current count, in microseconds (range 0-maxPeriod)
// Note shift direction in config is for converting micros to ticks.
// This is the reverse direction
//---------------------------------------------------------------------
unsigned long Timer::micros()
{
	unsigned long result;
	unsigned long t = (unsigned long)ticks();

	if (config.shift == left)
	{
		// divide by 2 to the exponent (factor)
		result = t >> config.factor;
	}
	else
	{
		// multiply by 2 to the exponent (factor)
		result = t << config.factor;
	}
	return result;
}

//---------------------------------------------------------------------
// setPeriod( register, microseconds )
//---------------------------------------------------------------------
long Timer::setPeriod(Register reg, unsigned long microseconds)
{
	unsigned short t;

	if (microseconds > config.maxPeriod)
	{
		return config.maxPeriod;
	}

	t = getTicks(microseconds);

	if (reg == compareA)
	{
		noInterrupts();
		OCR1A = t;
		interrupts();
	}
	else if (reg == compareB)
	{
		noInterrupts();
		OCR1B = t;
		interrupts();
	}
	return 0;
}

//---------------------------------------------------------------------
// start()
//---------------------------------------------------------------------
void Timer::start(void)
{
	noInterrupts();
	TCCR1B = config.bits;
	TCNT1 = 0x0000;
	interrupts();
}

//---------------------------------------------------------------------
// comparator match A isr
//---------------------------------------------------------------------
ISR(TIMER1_COMPA_vect)
{
	Timer::isrCompareA();
}

//---------------------------------------------------------------------
// comparator match B isr
//---------------------------------------------------------------------
ISR(TIMER1_COMPB_vect)
{
	Timer::isrCompareB();
}

//---------------------------------------------------------------------
// overflow isr
//---------------------------------------------------------------------
ISR(TIMER1_OVF_vect)
{ 
	Timer::isrOverflow();
}
