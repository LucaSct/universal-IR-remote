#pragma once

#include <Arduino.h>
#include <vector>

#include <driver/adc.h>

//USB:

//up:		2815
//down:		1812
//right:	0
//ok:		2470
//left:		3060

//Battery:

//up:		2920
//down:		1890
//right:	0
//ok:		2560
//left:		3165
class CButtonManager
{
	public:
		enum ButtonActions
		{
			BUTTON_DOWN_EVENT,
			BUTTON_UP_EVENT,
			BUTTON_CLICK_EVENT,
			BUTTON_HOLD_EVENT
		};

		typedef void(CButtonManager::*buttonEvent)(ButtonActions _buttonAction);

		struct SButtonEventObject
		{
			SButtonEventObject(uint _id, uint _pushedADCValue, uint _maxClickTime, uint _toleranceADCValue, uint _holdEventStartTime, uint _holdEventInterval, buttonEvent _event)
			{
				id = _id;
				pushedADCValue = _pushedADCValue;
				toleranceADCValue = _toleranceADCValue;
				maxClickTime = _maxClickTime;
				event = _event;
				holdEventStartTime = _holdEventStartTime;
				holdEventInterval = _holdEventInterval;

				lastState = BUTTON_UP_EVENT;

				lastPushedEventTime = 0;
				lastHoldEventTime = 0;
			}

			uint id,
				 pushedADCValue,
				 toleranceADCValue,
				 maxClickTime,
				holdEventStartTime,
				holdEventInterval;

			ButtonActions lastState;

			uint32_t lastPushedEventTime,
					 lastHoldEventTime;

			buttonEvent event;
		};

		CButtonManager();
		virtual ~CButtonManager();

		virtual void init(adc1_channel_t _adcChannel);
		virtual void loop();

		bool addButtonWatch(SButtonEventObject * _buttonEventObject);
		void removeButtonWatch(uint _id);

	protected:
		std::vector<SButtonEventObject *> buttonEventList;

	private:
		int lastLoopUpdate;

		adc1_channel_t adcChannel;

		bool isValueInRange(int _testValue, int _compareValue, int _range);
};

