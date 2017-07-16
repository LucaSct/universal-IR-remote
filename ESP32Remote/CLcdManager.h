#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <driver/adc.h>

#include "CBaseEventInterface.h"
#include "CButtonManager.h"
#include "CLcdMenu.h"

#define LCD_INTERFACE_ACTIVATE_PIN GPIO_NUM_36

class CLcdManager : public CBaseEventInterface, public CButtonManager
{
	public:
		CLcdManager(CBaseEventInterface * _basePointer);
		~CLcdManager();

		void init();
		void loop() override;

		

	private:
		LiquidCrystal lcd;

		CLcdMenu lcdMainMenu;

		bool bILcdnitialized;

		String lcdFunctionEvent(String _data);

		//eventObject lcdFunctionEventObject;

		void upButtonEventHandler(ButtonActions _buttonAction);
		void downButtonEventHandler(ButtonActions _buttonAction);
		void rightButtonEventHandler(ButtonActions _buttonAction);
		void okButtonEventHandler(ButtonActions _buttonAction);
		void leftButtonEventHandler(ButtonActions _buttonAction);
};

