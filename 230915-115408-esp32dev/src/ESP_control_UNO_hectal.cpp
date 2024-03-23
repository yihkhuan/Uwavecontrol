#include <Arduino.h>
#include <uwaveControl.h>


//WiFi credentials
const String ssid = "coldfire"; // Replace with your Wi-Fi network name
const String wifiPassword = "1234567890"; // Replace with your Wi-Fi network password

//timing
unsigned long lastActivityTime = 0;
const unsigned long inactiveTimeout = 10000;
//Global variables
unsigned int keyCount=0;


page Page = _main;

void setup() { 
  uwaveControl::init(ssid, wifiPassword, Page);
  //timing of inactivity
  // lastActivityTime = millis();
}  

void loop() {
  server.handleClient();
  //calculate how long the user is inactive for
  unsigned long currentTime = millis();
  long inactiveTime = currentTime - lastActivityTime;   

  // //enter pages to allow user control of the module
  //   //if a key is entered update arduino that the user is active
    if(Page.pageName == "mainPage" || Page.pageName =="screenPage"){
      switch(inactiveTime >= inactiveTimeout){
        case 1:
            Page = _screenSv;
            lcd.clear();
            lcd.noDisplay();
            lcd.noBacklight();
            break;
        case 0:
            Page = _main;
            lcd.clear();
            lcd.display();
            lcd.backlight();
            break;      
      } 
    }
    if(Page.pageName != "screenPage"){
    uwaveControl::loadPage(Page, input);
;
    char key = keypad.getKey();
    if(key != NO_KEY){
      //enter screensaver mode if user is inactive for a period longer than the timeout value
      lastActivityTime = millis();
      if(key == 'D'){
        for(int i = 0; i < sizeof(destination)/60/4 ; i++){
          if(Page.pageName == destination[i][0].pageName){
            *(Page.output) = input;
            (*(Page.output)).trim();

            input = empty;
            keyCount = 0;
            Page.fn();
            Page = destination[i][1];
            lcd.clear();
            break;
          }
        }
      }
      else if(key == 'C'){
        for(int i = 0; i < sizeof(destination)/60/4 ; i++){
          if(Page.pageName == destination[i][0].pageName){
            
            *(Page.output) = input;
            (*(Page.output)).trim();
            input = empty;
            keyCount = 0;
            Page.fn();
            Page = destination[i][2];
            lcd.clear();
            break;
          }
        }
      }
      else if(key == 'B'){
        for(int i = 0; i < sizeof(destination)/60/4 ; i++){
          if(Page.pageName == destination[i][0].pageName){
            keyCount = 0;
            Page = destination[i][3];
            lcd.clear();
            break;
          }
        }
      }
      else if(key == '*'){
        for(int i = 0; i < 16; i++){
        lcd.scrollDisplayLeft();

        }
        delay(3000);
      }
      else if(key == '#'){
        for(int i = 0; i < 16; i++){
        lcd.scrollDisplayRight();
        }
        delay(3000);
      }
      else{
        uwaveControl::typing(key,Page.keyEnd,keyCount,input);
      }
    }  
}
    if(Page.pageName == "screenPage"){
      char key = keypad.getKey();
      if(key != NO_KEY){
        //enter screensaver mode if user is inactive for a period longer than the timeout value
        lastActivityTime = millis();
      }
    }

}