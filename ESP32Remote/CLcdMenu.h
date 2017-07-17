#pragma once

#include <LiquidCrystal.h>
#include <vector>

#include "CButtonManager.h"
#include "CBaseEventInterface.h"

class CLcdMenuInterface
{
	public:
		enum Buttons
		{
			BUTTON_UP,
			BUTTON_DOWN,
			BUTTON_LEFT,
			BUTTON_RIGHT,
			BUTTON_OK
		};

		String menuName;

		CLcdMenuInterface(String _menuName);
		virtual ~CLcdMenuInterface();

		virtual void loop() = 0;
		virtual void processButtonEvent(Buttons _button, CButtonManager::ButtonActions _buttonAction) = 0;

		virtual bool hasContent() = 0;

		void setLcdPointer(LiquidCrystal * _lcd);
		void setParentMenuPointer(CLcdMenuInterface * _parentMenu);
		void setActive();
		void activateMenu(CLcdMenuInterface * _menu);

		static bool printCenteredString(String _string, bool _bSelected, uint8_t _row, uint8_t _endPosition, LiquidCrystal * _lcd, uint8_t _startPosition = 0);

	protected:
		bool bUpdateRequired;

		LiquidCrystal * lcd;

		CLcdMenuInterface	* activeMenu,
							* parentMenu;

		void setUpdateRequired() { bUpdateRequired = true; }

		bool printCenteredString(String _string, bool _bSelected, uint8_t _row, uint8_t _endPosition, uint8_t _startPosition = 0);
};

class CLcdMenu : public CLcdMenuInterface
{
	public:
		CLcdMenu(String _menuName, LiquidCrystal * _lcd);
		~CLcdMenu();

		void loop() override;

		void processButtonEvent(Buttons _button, CButtonManager::ButtonActions _buttonAction) override;

		void addElement(CLcdMenuInterface * _menu);
		void removeElement(CLcdMenuInterface * _menu);

		bool hasContent() override;

	private:
		int iIndex;

		bool btopSelected;

		std::vector<CLcdMenuInterface *> menuList;

		bool incrementIndex();
		bool decrementIndex();

};

class CFunctionParameter : public CLcdMenuInterface
{
	public:
		
		enum Parameters
		{
			PARAMETER_GET_STRING,
			PARAMETER_GET_NUMBER
		};

		CFunctionParameter(String _menuName, Parameters _parameter);
		~CFunctionParameter();

		void loop() override;
		void reset();
		void processButtonEvent(Buttons _button, CButtonManager::ButtonActions _buttonAction) override;

		bool hasContent() override;		
		bool isDone();

		String getResult();

	private:
		uint iCurrentNumber;
		
		char cCurrentChar;
		
		bool bDone;

		String result;

		Parameters parameter;

		/*void getString();
		void getNumber();*/

};

class CLcdMenuFunction : public CLcdMenuInterface, public CBaseEventInterface
{
	public:
		CLcdMenuFunction(String _menuName, eventObject _event, CBaseEventInterface * _basePointer, String _command = "");
		~CLcdMenuFunction();

		void loop() override;
		void processButtonEvent(Buttons _button, CButtonManager::ButtonActions _buttonAction) override;
		void addParameter(CFunctionParameter * _parameter);
		void addParameter(String _parameter);
		void removeParameter(CFunctionParameter * _parameter);

		bool hasContent() override;

	private:
		int iIndex;
		
		String parameterResult,
			   command;

		eventObject event;

		std::vector<CFunctionParameter *> parameterList;

};