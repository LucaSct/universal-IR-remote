#include "CButtonManager.h"



CButtonManager::CButtonManager()
{
}


CButtonManager::~CButtonManager()
{
	while (buttonEventList.size() > 0)
	{
		delete buttonEventList[0];

		buttonEventList.erase(buttonEventList.begin());
	}
}

void CButtonManager::init(adc1_channel_t _adcChannel)
{
	adcChannel = _adcChannel;

	adc1_config_channel_atten(_adcChannel, ADC_ATTEN_11db);
}

void CButtonManager::loop()
{
	if (millis() - lastLoopUpdate <= 10)
		return;

	for (int i = 0; i < buttonEventList.size(); i++)
	{
		if (isValueInRange(adc1_get_voltage(adcChannel), buttonEventList[i]->pushedADCValue, buttonEventList[i]->toleranceADCValue))
		{
			if (buttonEventList[i]->lastState == BUTTON_UP_EVENT)
			{
				(this->*buttonEventList[i]->event)(BUTTON_DOWN_EVENT);

				lastLoopUpdate = millis();
				buttonEventList[i]->lastPushedEventTime = millis();
			}
			else if (millis() - buttonEventList[i]->lastPushedEventTime >= buttonEventList[i]->holdEventStartTime && millis() - buttonEventList[i]->lastHoldEventTime >= buttonEventList[i]->holdEventInterval)
			{
				(this->*buttonEventList[i]->event)(BUTTON_HOLD_EVENT);

				buttonEventList[i]->lastHoldEventTime = millis();
			}

			buttonEventList[i]->lastState = BUTTON_DOWN_EVENT;
		}
		else
		{
			if (buttonEventList[i]->lastState == BUTTON_DOWN_EVENT)
			{
				if (millis() - buttonEventList[i]->lastPushedEventTime <= buttonEventList[i]->maxClickTime)
				{
					Serial.println("click event");

					(this->*buttonEventList[i]->event)(BUTTON_CLICK_EVENT);
				}
				else
					(this->*buttonEventList[i]->event)(BUTTON_UP_EVENT);

				lastLoopUpdate = millis();
			}

			buttonEventList[i]->lastState = BUTTON_UP_EVENT;
		}
	}

	lastLoopUpdate = millis();
}

bool CButtonManager::addButtonWatch(SButtonEventObject * _buttonEventObject)
{
	for (int i = 0; i < buttonEventList.size(); i++)
	{
		if (buttonEventList[i]->id == _buttonEventObject->id)
			return false;
	}

	buttonEventList.push_back(_buttonEventObject);

	return true;
}

void CButtonManager::removeButtonWatch(uint _id)
{
	for (int i = 0; i < buttonEventList.size(); i++)
	{
		if (buttonEventList[i]->id == _id)
		{
			delete buttonEventList[i];

			buttonEventList.erase(buttonEventList.begin() + i);

			break;
		}
	}
}

bool CButtonManager::isValueInRange(int _testValue, int _compareValue, int _range)
{
	if (_testValue < _compareValue - _range || _testValue > _compareValue + _range)
		return false;

	return true;
}
