/*!
	\file CDaguMC.cpp
	\author Mike Hankey
	\date 11/23/2013 12:01:06 PM
	
    \copyright
	Copyright (c) 2013 Mike Hankey <mikeh32217@yahoo.com>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

	\note This class is set up for the ATMega328P device using TImer1 and Timer2
	to control the Dagu 4 Channel Motor Controller board.
*/

#include <avr/io.h>
#include "CDaguMC.h"

/**
 * \brief Constructor
 * 
 * \return 
 */
CDaguMC::CDaguMC()
{
	//NOTE If anything changes the changes need to be reflected here!
	ConfigureChan_16bit(0, &OCR1A, &PORTC, PC0, &PORTB, PB4, &PORTB, PB1, &TCCR1B, _BV(CS11));
	ConfigureChan_16bit(1, &OCR1B, &PORTC, PC1, &PORTB, PB5, &PORTB, PB2, &TCCR1B, _BV(CS11));
	ConfigureChan_8bit(2, &OCR2A, &PORTC, PC2, &PORTB, PB6, &PORTB, PB3, &TCCR2B, _BV(CS21));
	ConfigureChan_8bit(3, &OCR2B, &PORTC, PC3, &PORTB, PB7, &PORTD, PD3, &TCCR2B, _BV(CS21));

	InitTimer1();
	InitTimer2();
	InitADC();
}

/**
 * \brief Read ADC value on selected chan
 * 
 * \param chan
 * 
 * \return uint8_t
 */
uint16_t CDaguMC::ReadADC(uint8_t chan)
{
	//Select appropriate channel to read
	ADMUX = (ADMUX & 0xf8) | (chan & 0x07);
	
	//Start conversion
	ADCSRA |= _BV(ADSC);
	while(ADCSRA & _BV(ADSC)) {}
	
	return ADC;
}

/**
 * \brief Accessor to obtain State
 * 
 * \param chan The channel state required
 * 
 * \return ChannelState The state
 */
ChannelState CDaguMC::GetState(uint8_t chan)
{
	return ms_channel_info[chan].s_state;
}

/**
 * \brief Accessor to obtain the direction
 * 
 * \param chan The channel required
 * 
 * \return ChannelDirection The channels direction
 */
ChannelDirection CDaguMC::GetDirection(uint8_t chan)
{
	return ms_channel_info[chan].s_dir;
}

uint16_t CDaguMC::GetSpeed(uint8_t chan)
{
	return ms_channel_info[chan].s_speed;
}

/**
 * \brief Sets a channels state
 * 
 * \param chan The channel
 * \param state The new state
 * 
 * \return void
 */
void CDaguMC::SetState(uint8_t chan, ChannelState state)
{
	ms_channel_info[chan].s_state = state;
	
	if (state == e_state_idle)
	{
		*ms_channel_info[chan].sp_tccrb_port &= ~ms_channel_info[chan].s_cs_value;
		*ms_channel_info[chan].sp_tccra_port &= ~ms_channel_info[chan].s_com_value;
		*ms_channel_info[chan].sp_pwm_port &= ~_BV(ms_channel_info[chan].s_pwm_pin);
	}
	else
	{
		*ms_channel_info[chan].sp_tccra_port |= ms_channel_info[chan].s_com_value;
		*ms_channel_info[chan].sp_tccrb_port |= ms_channel_info[chan].s_cs_value;
	}
}

/**
 * \brief Sets a channels direction
 * 
 * \param chan The channel
 * \param dir The new direction
 * 
 * \return void
 */
void CDaguMC::SetDirection(uint8_t chan, ChannelDirection dir)
{
	ms_channel_info[chan].s_dir = dir;
	
	if (dir == e_dir_fwd)
		*ms_channel_info[chan].sp_dir_port |= _BV(ms_channel_info[chan].s_dir_pin);
	else
		*ms_channel_info[chan].sp_dir_port &= ~_BV(ms_channel_info[chan].s_dir_pin);	
}

/**
 * \brief Sets a channels speed
 * 
 * \param chan The channel
 * \param speed The new speed
 * 
 * \return void
 */
void CDaguMC::SetSpeed(uint8_t chan, uint16_t speed)
{
	ms_channel_info[chan].s_speed = ((100.0 - (double)speed) / 100.0) * ms_channel_info[chan].s_top;
	
	*ms_channel_info[chan].sp_ocr_lg = ms_channel_info[chan].s_speed;
}

/**
 * \brief Set the speed relative to duty cycle
 * 
 * \param chan The channel
 * \param duty The duty cycle (0 - 100%)
 * 
 * \return void
 */
void CDaguMC::SetSpeed(uint8_t chan, uint8_t duty)
{
	ms_channel_info[chan].s_speed =  ((100.0 -(double)duty) / 100.0) * ms_channel_info[chan].s_top;
	
	*ms_channel_info[chan].sp_ocr_sm = ms_channel_info[chan].s_speed & 0x00ff;
}

/**
 * \brief Configure the 8bit Timer2
 * 
 * \param chan The channel associated with this timer channel
 * \param ocr_addr Pointer to the OCRnX register
 * \param dir_port Pointer to the direction port
 * \param dir_pin Pin used for direction
 * \param pwm_port Pointer to the PWM port
 * \param pwm_pin Pin used for PWM
 * \param tccrb_port Pointer to the TCCRB port
 * \param cs_value Value to use for prescale
 * 
 * \return void
 */
void CDaguMC::ConfigureChan_8bit(uint8_t chan,
	volatile uint8_t* ocr_addr,
	volatile uint8_t* adc_port, uint8_t adc_pin,
	volatile uint8_t* dir_port, uint8_t dir_pin,
	volatile uint8_t* pwm_port, uint8_t pwm_pin,
	volatile uint8_t* tccrb_port, uint8_t cs_value)
{
	ms_channel_info[chan].sp_ocr_sm = ocr_addr;
	ConfigureChannel(chan, adc_port, adc_pin, dir_port, dir_pin, pwm_port, pwm_pin, tccrb_port, cs_value);	
}

/**
 * \brief Configure the 16bit Timer1
 * 
 * \param chan The channel associated with this timer channel
 * \param ocr_addr Pointer to the OCRnX register
 * \param dir_port Pointer to the direction port
 * \param dir_pin Pin used for direction
 * \param pwm_port Pointer to the PWM port
 * \param pwm_pin Pin used for PWM
 * \param tccrb_port Pointer to the TCCRB port
 * \param cs_value Value to use for prescale
 * 
 * \return void
 */
void CDaguMC::ConfigureChan_16bit(uint8_t chan,
	volatile uint16_t* ocr_addr,
	volatile uint8_t* adc_port, uint8_t adc_pin,
	volatile uint8_t* dir_port, uint8_t dir_pin,
	volatile uint8_t* pwm_port, uint8_t pwm_pin,
volatile uint8_t* tccrb_port, uint8_t cs_value)
{
	ms_channel_info[chan].sp_ocr_lg = ocr_addr;
	ConfigureChannel(chan, adc_port, adc_pin, dir_port, dir_pin, pwm_port, pwm_pin, tccrb_port, cs_value);
}

/**
 * \brief Generic method to configure Timer 
 * 
 * \param chan The channel associated with this timer channel
 * \param dir_port Pointer to the direction port
 * \param dir_pin Pin used for direction
 * \param pwm_port Pointer to the PWM port
 * \param pwm_pin Pin used for PWM
 * \param tccrb_port Pointer to the TCCRB port
 * \param cs_value Value to use for prescale
 * 
 * \return void
 */
void CDaguMC::ConfigureChannel(uint8_t chan,
		volatile uint8_t* adc_port, uint8_t adc_pin,
		volatile uint8_t* dir_port, uint8_t dir_pin,
		volatile uint8_t* pwm_port, uint8_t pwm_pin,
		volatile uint8_t* tccrb_port, uint8_t cs_value)
{
	//NOTE Since both timers have a top value of 255 this is OK.
	//The reason they both have the same value is because I have
	//them running at same frequency and that is how it worked out.
	ms_channel_info[chan].s_top = 255;
	
	ms_channel_info[chan].sp_dir_port = dir_port;
	ms_channel_info[chan].s_dir_pin = dir_pin;
	ms_channel_info[chan].sp_pwm_port = pwm_port;
	ms_channel_info[chan].s_pwm_pin = pwm_pin;
	
	//NOTE Since all COM bits are the same for A/B channels 4-7 we can use any of them
	ms_channel_info[chan].sp_tccra_port = tccrb_port - 1;
	ms_channel_info[chan].s_com_value = _BV(COM1A0) | _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1);
	
	ms_channel_info[chan].sp_tccrb_port = tccrb_port;
	ms_channel_info[chan].s_cs_value = cs_value;
	
	volatile uint8_t* ddr = dir_port - 1;
	*ddr |= _BV(dir_pin);
	
	ddr = pwm_port - 1;
	*ddr |= _BV(pwm_pin);
}

/**
 * \brief Initialize Timer1
 * 
 * \return void
 */
void CDaguMC::InitTimer1()
{
	//NOTE Prescale will be 8, _BV(CS11)
	
	DDRB |= _BV(ms_channel_info[0].s_pwm_pin) | _BV(ms_channel_info[1].s_pwm_pin);
	
	//Set on up counting, clear on down counting
	TCCR1A |= _BV(COM1A0) | _BV(COM1A1);
	TCCR1A |= _BV(COM1B0) | _BV(COM1B1);
	//Phase and Frequency Correct mode.
	TCCR1B |= _BV(WGM13);
	//~4kHz
	ICR1 = 255;
	
	SetSpeed(0, (uint16_t) 50);
	SetSpeed(1, (uint16_t) 50);
}

/**
 * \brief Initialize Timer2
 * 
 * \return void
 */
void CDaguMC::InitTimer2()
{
	//NOTE Prescale will be 8, _BV(CS22)
	
	DDRB |= _BV(ms_channel_info[2].s_pwm_pin); 
	DDRD |= _BV(ms_channel_info[1].s_pwm_pin);
	
	TCCR2A |= _BV(WGM00) | _BV(COM2A0) | _BV(COM2A1);
	TCCR2A |= _BV(COM2B0) | _BV(COM2B1);

	SetSpeed(2, (uint8_t) 50);
	SetSpeed(3, (uint8_t) 50);
}

/**
 * \brief Initialize ADC unit
 * 
 * \return void
 */
void CDaguMC::InitADC()
{
	//ADC Prescale 128 @ 16MHz produced a frequency of 125K sample/sec.
	ADCSRA |= _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);
	//Enable ADC unit
	ADCSRA |= _BV(ADEN);
	//External reference
	ADMUX = _BV(REFS0);
	
	//Disable digital input on ADC pins
	DIDR0 |= _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D) | _BV(ADC3D);
}
