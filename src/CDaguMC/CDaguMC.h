/*!
	\file CDaguMC.h
	\author Mike Hankey
	\date 11/23/2013 12:01:53 PM
	
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

#ifndef CDAGUMC_H_
#define CDAGUMC_H_

typedef enum
{
	e_state_idle,
	e_state_running
} ChannelState;

typedef enum
{
	e_dir_fwd,
	e_dir_rev
} ChannelDirection;

typedef struct
{
	volatile ChannelState s_state;
	volatile ChannelDirection s_dir;
	volatile uint16_t s_speed;
	
	volatile uint16_t s_top;
	volatile uint8_t* sp_ocr_sm;
	volatile uint16_t* sp_ocr_lg;
	
	volatile uint8_t* sp_adc_port;
	volatile uint8_t* sp_dir_port;
	volatile uint8_t* sp_pwm_port;
	
	volatile uint8_t m_adc_pin;
	volatile uint8_t  s_dir_pin;
	volatile uint8_t  s_pwm_pin;
	
	volatile uint8_t* sp_tccra_port;
	volatile uint8_t  s_com_value;
	volatile uint8_t* sp_tccrb_port;
	volatile uint16_t s_cs_value;
} ChannelInfo;

class CDaguMC
{
	private:
		ChannelInfo ms_channel_info[4];
		
	public:
		CDaguMC();
		
		uint16_t ReadADC(uint8_t chan);
		
	protected:
		//Accessors
		void SetState(uint8_t chan, ChannelState state);
		ChannelState GetState(uint8_t chan);
		
		void SetDirection(uint8_t chan, ChannelDirection dir);
		ChannelDirection GetDirection(uint8_t chan);
		
		uint16_t GetSpeed(uint8_t chan);
		
		void SetSpeed(uint8_t chan, uint8_t duty);		//As a percentage/duty
		void SetSpeed(uint8_t chan, uint16_t speed);	//Raw value
		
	private:
		void InitTimer1();
		void InitTimer2();
		void InitADC();
		
		void ConfigureChan_8bit(uint8_t chan,
			volatile uint8_t* ocr_addr,
			volatile uint8_t* adc_port, uint8_t adc_pin,
			volatile uint8_t* dir_port, uint8_t dir_pin,
			volatile uint8_t* pwm_port, uint8_t pwm_pin,
			volatile uint8_t* tccrb_port, uint8_t cs_value);
		
		void ConfigureChan_16bit(uint8_t chan,
			volatile uint16_t* ocr_addr,
			volatile uint8_t* adc_port, uint8_t adc_pin,
			volatile uint8_t* dir_port, uint8_t dir_pin,
			volatile uint8_t* pwm_port, uint8_t pwm_pin,
			volatile uint8_t* tccrb_port, uint8_t cs_value);
		
		void ConfigureChannel(uint8_t chan, 
			volatile uint8_t* adc_port, uint8_t adc_pin,
			volatile uint8_t* dir_port, uint8_t dir_pin,
			volatile uint8_t* pwm_port, uint8_t pwm_pin,
			volatile uint8_t* tccrb_port, uint8_t cs_value);
};

#endif /* CDAGUMC_H_ */