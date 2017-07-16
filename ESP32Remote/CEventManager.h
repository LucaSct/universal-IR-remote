#pragma once

#include <vector>
#include <WiFi.h>

#include "CBaseEventInterface.h"

class CEventManager : public CBaseEventInterface
{
	public:
		CEventManager();
		virtual ~CEventManager();

		//virtual void init();
		void loop() override;

		void addInstance(CBaseEventInterface * _instance);
		void removeInstance(CBaseEventInterface * _instance);

		bool registerEvent(String _eventName, eventObject _event) override;
		void unregisterEvent(String _eventName) override;

	protected:
		std::vector<CBaseEventInterface *> instanceList;
};

