#pragma once

//#define SAMPLE_DELAY 10 //us
#define SIGNAL_TIMEOUT 30000 //us

#define MAX_PULSE_INDEX 50

#include <driver/ledc.h>
#include <Arduino.h>

#include "IRStatus.h"

class CIRControl
{
	public:
		bool init(gpio_num_t _irSendGPIO, gpio_num_t _irReceiveGPIO, ledc_channel_t _timerChannel, ledc_timer_t _timerSelect);


		void sendSignal();
		IRStatus getSignal(uint _timeout); //timeout in milliseconds

		IRStatus setIRSignalValue(uint16_t _highValue, uint16_t _lowValue, uint8_t _pulseIndex);
		IRStatus getIRSignalValue(uint16_t * _highValue, uint16_t * _lowValue, uint8_t _pulseIndex);

	private:
		uint16_t irSignal[MAX_PULSE_INDEX][2];

		gpio_num_t irReceiveGPIO;
		
		ledc_channel_t timerChannel;

};

