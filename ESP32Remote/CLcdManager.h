#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <driver/adc.h>

#include "CBaseEventInterface.h"
#include "CButtonManager.h"
#include "CLcdMenu.h"

#define LCD_INTERFACE_ACTIVATE_PIN GPIO_NUM_36

class CLcdManager : public CBaseEventInterface, public CButtonManager, public LiquidCrystal
{
	public:
		CLcdManager(CBaseEventInterface * _basePointer);
		~CLcdManager();

		void init();
		void loop() override;

		void updateLoadMenu(std::vector<String> * _signalNameList);

	private:

		CLcdMenu  lcdMainMenu,
				 *loadIRSignalMenu;

		bool bILcdnitialized;

		String lcdFunctionEvent(String _data);

		void upButtonEventHandler(ButtonActions _buttonAction);
		void downButtonEventHandler(ButtonActions _buttonAction);
		void rightButtonEventHandler(ButtonActions _buttonAction);
		void okButtonEventHandler(ButtonActions _buttonAction);
		void leftButtonEventHandler(ButtonActions _buttonAction);
};

