/*!
	\file CDaguHAL.h
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

#ifndef __CDAGUHAL_H__
#define __CDAGUHAL_H__

#include "CDaguMC.h"

class CDaguHAL : public CDaguMC
{
	private:
		
	//functions
	public:
		CDaguHAL();
	
		void Move(ChannelDirection dir, uint8_t speed);
		void Turn(uint16_t angle);
		void Stop();
		
		//Linear methods
		void SetSpeed(uint8_t speed);
		void SetDirection(ChannelDirection dir);
		void SetState(ChannelState state);
		
		//Differential methods
		void SetSpeed(uint8_t left, uint8_t rt);
		void SetDirection(ChannelDirection left, ChannelDirection rt);
		void SetState(ChannelState left, ChannelState rt);

}; //CDaguHAL

#endif //__CDAGUHAL_H__
