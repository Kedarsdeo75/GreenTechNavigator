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

#if !defined(F_CPU)
#define  F_CPU 16000000
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "CDaguHAL.h"

CDaguHAL::CDaguHAL()
{
}

void CDaguHAL::SetSpeed(uint8_t speed)
{
	CDaguMC::SetSpeed(0, (uint16_t)speed);
	CDaguMC::SetSpeed(1, (uint16_t)speed);
	CDaguMC::SetSpeed(2, (uint8_t)speed);
	CDaguMC::SetSpeed(3, (uint8_t)speed);
}

void CDaguHAL::Stop()
{
	SetState(e_state_idle);
}

void CDaguHAL::SetState(ChannelState state)
{
	CDaguMC::SetState(0, state);
	CDaguMC::SetState(1, state);
	CDaguMC::SetState(2, state);
	CDaguMC::SetState(3, state);
}

void CDaguHAL::SetDirection(ChannelDirection dir)
{
	CDaguMC::SetDirection(0, dir);
	CDaguMC::SetDirection(1, dir);
	CDaguMC::SetDirection(2, dir);
	CDaguMC::SetDirection(3, dir);
}

void CDaguHAL::Move(ChannelDirection dir, uint8_t speed)
{
	SetState(e_state_running);
	SetSpeed(speed);
	SetDirection(dir);
}

void CDaguHAL::SetState(ChannelState left, ChannelState rt)
{
	CDaguMC::SetState(0, left);
	CDaguMC::SetState(1, rt);
	CDaguMC::SetState(2, left);
	CDaguMC::SetState(3, rt);	
}

void CDaguHAL::SetDirection(ChannelDirection left, ChannelDirection rt)
{
	CDaguMC::SetDirection(0, left);
	CDaguMC::SetDirection(1, rt);
	CDaguMC::SetDirection(2, left);
	CDaguMC::SetDirection(3, rt);
}

void CDaguHAL::SetSpeed(uint8_t left, uint8_t rt)
{
	CDaguMC::SetSpeed(0, (uint16_t)left);
	CDaguMC::SetSpeed(1, (uint16_t)rt);
	CDaguMC::SetSpeed(2, (uint8_t)left);
	CDaguMC::SetSpeed(3, (uint8_t)rt);
}

void CDaguHAL::Turn(uint16_t angle)
{
	//TODO Should be different algorithm if moving as opposed to standing still
	SetState(e_state_running);
	SetDirection(e_dir_fwd, e_dir_rev);
	SetSpeed(20);
}