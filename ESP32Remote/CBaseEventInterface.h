#pragma once

#include <vector>
#include <WiFi.h>

#include "IRStatus.h"

// CMainApplication;

class CBaseEventInterface
{
	public:
		typedef String(CBaseEventInterface::*eventObject)(String _data);

		struct SEventObject
		{
			String _eventName;

			eventObject _event;
		};

		CBaseEventInterface(CBaseEventInterface * _basePointer);
		virtual ~CBaseEventInterface();

		virtual void loop() = 0;

		virtual bool registerEvent(String _eventName, eventObject _event);
		virtual	void unregisterEvent(String _eventName);

		String raiseEvent(String _eventName, String _eventData);

	protected:
		std::vector<SEventObject *> eventList;

		CBaseEventInterface * basePointer;
};

