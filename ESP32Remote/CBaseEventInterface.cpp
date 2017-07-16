#include "CBaseEventInterface.h"



CBaseEventInterface::CBaseEventInterface(CBaseEventInterface * _basePointer)
{
	basePointer = _basePointer;
}


CBaseEventInterface::~CBaseEventInterface()
{
	while (eventList.size() > 0)
	{
		delete eventList[0];

		eventList.erase(eventList.begin());
	}
}

bool CBaseEventInterface::registerEvent(String _eventName, eventObject _event)
{
	for (int i = 0; i < eventList.size(); i++)
	{
		if (eventList[i]->_eventName == _eventName)
			return false;
	}

	SEventObject * event = new SEventObject;

	event->_eventName = _eventName;
	event->_event = _event;

	eventList.push_back(event);

	return true;
}

void CBaseEventInterface::unregisterEvent(String _eventName)
{
	for (int i = 0; i < eventList.size(); i++)
	{
		if (eventList[i]->_eventName == _eventName)
		{
			delete eventList[i];

			eventList.erase(eventList.begin() + i);

			break;
		}
	}
}

String CBaseEventInterface::raiseEvent(String _eventName, String _eventData)
{
	for (int i = 0; i < eventList.size(); i++)
	{
		if (eventList[i]->_eventName == _eventName && eventList[i]->_event)
			return (basePointer->*eventList[i]->_event)(_eventData);	
	}

	return IRStatusToString(IR_CMD_NOT_RECOGNIZED);
}
