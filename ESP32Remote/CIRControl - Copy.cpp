#include "CIRControl.h"


bool CIRControl::init(gpio_num_t _irSendGPIO, gpio_num_t _irReceiveGPIO, ledc_channel_t _timerChannel, ledc_timer_t _timerSelect)
{

	pinMode(_irSendGPIO, OUTPUT);
	pinMode(_irReceiveGPIO, INPUT);

	irReceiveGPIO = _irReceiveGPIO;
	timerChannel = _timerChannel;

	ledc_channel_config_t ledcChannelConfig;

	ledcChannelConfig.gpio_num = _irSendGPIO;
	ledcChannelConfig.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledcChannelConfig.channel = _timerChannel;
	ledcChannelConfig.intr_type = LEDC_INTR_DISABLE;
	ledcChannelConfig.timer_sel = _timerSelect;
	ledcChannelConfig.duty = 0;

	ledc_timer_config_t ledcTimerConfig;

	ledcTimerConfig.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledcTimerConfig.bit_num = LEDC_TIMER_11_BIT;
	ledcTimerConfig.timer_num = _timerSelect;
	ledcTimerConfig.freq_hz = 38000;

	if (ledc_channel_config(&ledcChannelConfig) != ESP_OK)
		return false;

	if (ledc_timer_config(&ledcTimerConfig) != ESP_OK)
		return false;

	return true;
}

void CIRControl::sendSignal()
{
	int i = 0;

	while (true)
	{
		if (irSignal[i][0] == 0)
			break;

		ledc_set_duty(LEDC_HIGH_SPEED_MODE, timerChannel, 1024);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, timerChannel);

		ets_delay_us(irSignal[i][0] * SAMPLE_DELAY);

		if (irSignal[i][1] == 0)
			break;

		ledc_stop(LEDC_HIGH_SPEED_MODE, timerChannel, 0);

		ets_delay_us(irSignal[i][1] * SAMPLE_DELAY);

		i++;
	}

	ledc_stop(LEDC_HIGH_SPEED_MODE, timerChannel, 0);
}

bool CIRControl::getSignal(uint _timeout)
{
	uint16_t irPulseCounter;

	uint8_t irCounter;

	int currentTime = millis();

	uint32_t startMicros,
			 endMicros;

	bool bSampling;

	__retry:

	irCounter = 0;

	bSampling = true;

	while (true)
	{
		if (_timeout != 0 && _timeout <= millis() - currentTime)
			return false;

		if (!gpio_get_level(irReceiveGPIO))
		{
			while (bSampling)
			{
				irPulseCounter = 0;

				startMicros = micros();

				while (!gpio_get_level(irReceiveGPIO))
				{
					irPulseCounter++;

					ets_delay_us(SAMPLE_DELAY);

					if (SAMPLE_DELAY * irPulseCounter > SIGNAL_TIMEOUT)
					{
						irPulseCounter = 0;

						bSampling = false;

						break;
					}
				}

				if (irCounter == MAX_PULSE_INDEX)
					return false;

				endMicros = micros();

				if (irPulseCounter != 0)
					irSignal[irCounter][0] = irPulseCounter + (endMicros - startMicros - irPulseCounter * SAMPLE_DELAY) / SAMPLE_DELAY + 1;
				else
					irSignal[irCounter][0] = 0;

				irPulseCounter = 0;

				startMicros = micros();

				while (gpio_get_level(GPIO_NUM_25) && bSampling)
				{
					irPulseCounter++;

					ets_delay_us(SAMPLE_DELAY);

					if (SAMPLE_DELAY * irPulseCounter > SIGNAL_TIMEOUT)
					{
						irPulseCounter = 0;

						bSampling = false;

						break;
					}
				}

				endMicros = micros();

				if (irPulseCounter != 0)
					irSignal[irCounter][1] = irPulseCounter + (endMicros - startMicros - irPulseCounter * SAMPLE_DELAY) / SAMPLE_DELAY + 1;
				else
					irSignal[irCounter][1] = 0;

				irCounter++;
			}

			for (int i = 0; i < irCounter; i++) //check whether IR singal is valid
			{
				Serial.print("Pulse " + String(i) + ":");
				Serial.println("Low " + String(irSignal[i][0] * SAMPLE_DELAY) + ";" + "High " + irSignal[i][1] * SAMPLE_DELAY);


				if (irSignal[i][0] * SAMPLE_DELAY < 250 || irSignal[i][1] * SAMPLE_DELAY < 250) //pulse below 250us is unrealistic
				{
					if (i == irCounter - 1 && irSignal[i][0] * SAMPLE_DELAY >= 250)
						break;

					goto __retry;
				}
			}

			break;
		}
	}

	return true;
}

bool CIRControl::setIRSignalValue(uint16_t _highValue, uint16_t _lowValue, uint8_t _pulseIndex)
{
	if (_pulseIndex >= MAX_PULSE_INDEX)
		return false;

	irSignal[_pulseIndex][0] = _lowValue / SAMPLE_DELAY;
	irSignal[_pulseIndex][1] = _highValue / SAMPLE_DELAY;

	return true;
}

bool CIRControl::getIRSignalValue(uint16_t *_highValue, uint16_t * _lowValue, uint8_t _pulseIndex)
{
	if (_pulseIndex >= MAX_PULSE_INDEX)
		return false;

	*_lowValue = irSignal[_pulseIndex][0] * SAMPLE_DELAY;
	*_highValue = irSignal[_pulseIndex][1] * SAMPLE_DELAY;

	return true;
}
