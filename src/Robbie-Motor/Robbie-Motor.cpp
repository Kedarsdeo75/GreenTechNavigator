/*!
	\file Robbie_Motor.cpp
	\author Mike
	\date 11/19/2013 12:55:47 PM
	
    \copyright
	Copyright (c) 2013 Mike <mikeh32217@yahoo.com>

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

*/
#if !defined(F_CPU)
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "_cplusplus.h"
#include "CDaguHAL.h"
#include "Robbie-Motor.h"

// ============================== Variable declaration ==============================
CDaguHAL* pdagu = nullptr;

volatile uint8_t msg_buf[5] = { 0, 0, 0, 0, 0 };
volatile uint8_t out_buf = 0x55;

volatile uint8_t msg_current = 0;
volatile uint8_t msg_bit_cnt = 0;
volatile uint8_t msg_cursor = 0;
volatile bool spi_flag = false;

// =============================== Forward declarations ==============================
void Initialize();
void InitSPI();

//====================================== ISRs ========================================

/* 
	TODO This needs some work, fixed message length of 4 bytes.
	
	The Main uC shall be able to request infomation;
		- Speed
		- Direction
		- Angle
		- State
*/
ISR(INT0_vect)
{
	msg_current <<= 1;
	if (PIND & _BV(PD5))
		msg_current |= 1;
	
	if (out_buf & (7 - msg_bit_cnt))
		PORTD |= _BV(PD6);
	
	if (++msg_bit_cnt > 7)
	{
		msg_buf[msg_cursor++] = msg_current;
		msg_bit_cnt = 0;
		
		if (((msg_cursor == 1) && (msg_current & 0x80)) || (msg_cursor > 3))
			spi_flag = true;
	}
}

//====================================== main ========================================

/**
 * \brief Main routine
 * 
 * \param None
 * 
 * \return int 
 */
int main(void)
{
	volatile uint16_t adc_result = 0;
	volatile bool stop = false;
	
	Initialize();
	
//	out_buf = MOTOR_CMD | 0x05;
	
    while(1)
    {
		//TODO Read ADC value for each channel here in main loop to check for stalled motors
		for (uint8_t i = 0; i < 4; i++)
		{
			adc_result = pdagu->ReadADC(i);
			if (adc_result > 70)
			{
				stop = true;
				break;
			}
		}
		
		if (!stop)
			pdagu->Move(e_dir_fwd, 20);
		else
		{
			pdagu->Stop();
			_delay_ms(500);
			pdagu->Move(e_dir_rev, 20);
			_delay_ms(500);
			pdagu->Stop();
			pdagu->Turn(20);
			_delay_ms(750);
			pdagu->Stop();
			_delay_ms(500);
			stop = false;
		}

        //if (spi_flag)
		//{			
			//switch(msg_buf[0] & 0xf0)
			//{
				//case MI_CMD_GO:
					////nokia.Clear();
					////for (uint8_t i = 0; i < 4; i++)
					////{
						////nokia.GoToXY(0, i);
						////nokia.DrawInteger(msg_buf[i], 16);
					////}
					////nokia.Display();
					//break;
				//case MI_CMD_STOP:
					////nokia.Clear(0, 0, 40);
					////nokia.DrawInteger(msg_buf[0], 16);
					////nokia.Display(0, 0, 40);
					////_delay_ms(100);
					//PORTD |= _BV(PD7);
					//_delay_us(5);
					//PORTD &= ~_BV(PD7);
					//break;
			//}	
			//
			//memset((void*)msg_buf, 0, 4);
			//msg_bit_cnt = 0;
			//msg_cursor = 0;
			//spi_flag = false;
		//}
			//
		//_delay_ms(50);
    }
}

/**
 * \brief Main Initialize routine
 * 
 * \return void
 */
void Initialize()
{
	InitSPI();
	
	pdagu = new CDaguHAL();
	
	sei();
}

/**
 * \brief Initialization routine for SPI communications
 * 
 * \return void
 */
void InitSPI()
{	
	//Master interrupt and MISO line to the main uC
	DDRD |= _BV(PD6) | _BV(PD4);
	
	//Rising edge for incoming SPI clock 
	EICRA |= _BV(ISC00) | _BV(ISC01);
	EIMSK |= _BV(INT0);	
}
