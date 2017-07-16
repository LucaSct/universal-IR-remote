/*
 Name:		ESP32Remote.ino
 Created:	5/30/2017 8:12:38 PM
 Author:	Luca
*/

//#include <vector>
//
//#include "FS.h"
//#include "SD.h"
//#include "SPI.h"
//
//#include "CIRControl.h"
//#include "CWifiManager.h"
//

#include "CMainApplication.h"

CMainApplication mainApp;

// the setup function runs once when you press reset or power the board
void setup()
{
	mainApp.init();
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	mainApp.loop();
}


