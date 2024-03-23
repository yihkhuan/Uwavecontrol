#ifndef uwaveControl_h
#define uwaveControl_h

#include "Arduino.h"
#include "uwaveControl.h"
#include "LiquidCrystal_I2C.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WebServer.h"
#include "Preferences.h"
#include <ArduinoJson.h>
#include "Keypad.h"
#include "functional"

extern WebServer server;
extern LiquidCrystal_I2C lcd;
extern Keypad keypad;
// extern String numbers[];

extern struct page _main;
extern struct page _screenSv;
extern struct page destination[10][4];

extern String Switch; 
extern String t;
extern String Command; 
extern String password;
extern String Password;
extern String Select;
extern String input;
extern String empty;

struct page{
    page(String pageName, String firstRowDisplay, String secondRowDisplay,int initX, int initY, int keyEnd, String* input1, String* input2, String* output, 
    void (*fn)());
    String pageName;
    String firstRowDisplay;
    String secondRowDisplay;
    int initX;
    int initY;
    int keyEnd;
    String* input1;
    String* input2;
    String* output;
    void (*fn)();
};
 

class uwaveControl
{
    public:
        static void init(String ssid,String wifiPassword, page &Page);
        //keypad functions
        static void typing(char key, int keyEnd, unsigned int &keyCount, String &var);
        static void loadPage(page &Page,String input);



        static void comm();
        static void read();
        static void nullFn();
        static void save();
        static void passwordSwt();
        static void passwordArd();
        static void openCheck();
        static void closeCheck();

        //wifi functions



    private:
        //keypad functions
        static void backspace(int initPosX,int initPosY, int &keyCount);

        //wifi functions
        static void stringyfy(String list[], String &jsonString);
        static void testConnection();
        static void close();
        static void open();
        static void reset();
        static void check();
        static void list();        
        static void ardReset();
};

#endif