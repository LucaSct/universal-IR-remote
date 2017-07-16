#include "CEventManager.h"



CEventManager::CEventManager() : CBaseEventInterface(nullptr)
{
}


CEventManager::~CEventManager()
{
}

void CEventManager::loop()
{
	for (int i = 0; i < instanceList.size(); i++)
		instanceList[i]->loop();
}

void CEventManager::addInstance(CBaseEventInterface * _instance)
{
	instanceList.push_back(_instance);
}

void CEventManager::removeInstance(CBaseEventInterface * _instance)
{
	for (int i = 0; i < instanceList.size(); i++)
	{
		if (instanceList[i] == _instance)
		{
			instanceList.erase(instanceList.begin() + i);

			break;
		}
	}
}

bool CEventManager::registerEvent(String _eventName, eventObject _event)
{
	for (int i = 0; i < instanceList.size(); i++)
		instanceList[i]->registerEvent(_eventName, _event);

	return true;
}

void CEventManager::unregisterEvent(String _eventName)
{
	for (int i = 0; i < instanceList.size(); i++)
		instanceList[i]->unregisterEvent(_eventName);
}
