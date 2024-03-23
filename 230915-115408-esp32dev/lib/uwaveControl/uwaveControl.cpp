#include "Arduino.h"
#include "uwaveControl.h"
#include "LiquidCrystal_I2C.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WebServer.h"
#include "Preferences.h"
#include <ArduinoJson.h>
#include <functional>
#include <sstream>
#include <iomanip>

#define RXp2 16
#define TXp2 17

LiquidCrystal_I2C lcd(0x27,16,2);
WebServer server(80);

//keypad definations
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {23, 32, 33, 25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 27, 14, 13}; //connect to the column pinouts of the keypad

//Create an object of keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// String numbers[] = 
// {"00","01","02","03","04","05","06","07","08","09","10",
// "11","12","13","14","15","16","17","18","19","20",
// "21","22","23","24","25","26","27","28"
// };

String Switch = String("AA"); 
String t = String(500);
String Command = String(3);
String password = String(0000000);
String Password = String(1234567);
String Select = String(9);
String currentState = String(9);
String empty = String() + ' ' + ' '+ ' '+' '+ ' ' + ' '+ ' '+' ';
String input = empty;
page _main    = page("mainPage",    "uwaveControl", "",                   8, 1, 0, &empty,        &empty,   &Select,  uwaveControl::nullFn);
page _time    = page("timePage",    "Current(ms):", "New(ms):",           9, 1, 4, &t,            &empty,   &t,       uwaveControl::nullFn);
page _set     = page("setPage",     "set" ,"switch/time",                 0, 0, 1, &empty,        &empty,   &Select,   uwaveControl::nullFn);
page _reset   = page("resetPage",   "reset", "switch/arduino",            0, 0, 1, &empty,        &empty,   &Select,   uwaveControl::nullFn);
page _resetSwt= page("resetSwt",    "enter passord", "",                  0, 1, 7, &empty,        &empty,   &password, uwaveControl::passwordSwt);
page _resetArd= page("resetArd",    "enter passord", "",                  0, 1, 7, &empty,        &empty,   &password, uwaveControl::passwordArd);
page _switch  = page("switchPage",  "set Switch:",  "",                   0, 1, 2, &empty,        &empty,   &Switch,  uwaveControl::read);
page _open    = page("openPage",    "current state:",  "key 0, open",   0, 0, 1, &currentState, &Switch,  &Command, uwaveControl::openCheck);
page _close   = page("closePage",   "current state:",  "key 1, close",  0, 0, 1, &currentState, &Switch,  &Command, uwaveControl::closeCheck);
page _screenSv= page("screenPage",    "",       "",                         0, 0, 0, &empty,       &empty, &Select,   uwaveControl::nullFn);
page destination[10][4] = {{_main,   _set,    _reset,  _main},
                          {_set,    _switch, _time,   _main},
                          {_reset,  _resetSwt, _resetArd, _main},
                          {_resetSwt, _main, _main, _reset},
                          {_resetArd, _main, _main, _reset},
                          {_time,   _main,   _main,   _main},
                          {_switch, _open,   _close,  _main},
                          {_open,   _main,   _main ,  _switch},
                          {_close,  _main,   _main,   _switch},
                          {_screenSv,   _main,   _main,   _main}
                          };

page::page(String pageName, String firstRowDisplay, String secondRowDisplay,int initX, int initY, int keyEnd, String* input1, String* input2, String* output,
void (*fn)())
{
  page::pageName = pageName;
  page::firstRowDisplay = firstRowDisplay;
  page::secondRowDisplay = secondRowDisplay;
  page::initX = initX;
  page::initY = initY;
  page::keyEnd = keyEnd;
  page::input1 = input1;
  page::input2 = input2;
  page::output = output;
  page::fn = fn;
};


void uwaveControl::init(String ssid,String wifiPassword, page &Page)
{
  //init lcd
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.begin(16, 2);

  //Code to set ESP 32 to connect with wifi
  Serial.begin(9600);

  WiFi.begin(ssid, wifiPassword); // Connect to Wi-Fi network
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi connection
    delay(1000);
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Types of instructions to be handled
  server.on("/close", uwaveControl::close);
  server.on("/open", uwaveControl::open);
  server.on("/reset", uwaveControl::reset);
  server.on("/check", uwaveControl::check);
  server.on("/list", uwaveControl::list);
  server.on("/resetModule", uwaveControl::ardReset);
  server.on("/", uwaveControl::testConnection);

  server.begin();
  //Sending message to UNO via UART
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  Page = _main;
}
//keypad methods
void uwaveControl::typing(char key, int keyEnd, unsigned int &keyCount, String &input)
{
  if(key == 'A'){
    if(keyCount == 0){
      keyCount = keyEnd - 1;
    }
    else{
      keyCount--;
    }
    input[keyCount] = ' '; 
  }
  else{
    input[keyCount] = key;
    if(keyCount == keyEnd - 1){
      keyCount = 0;
    }
    else{
      keyCount++;
    }
  }
}

void uwaveControl::backspace(int initPosX,int initPosY, int &keyCount)
{
  if(keyCount!=0){
    lcd.setCursor(initPosX+keyCount-2, initPosY);
    lcd.print(' ');
  }
}

void uwaveControl::save()
{
  Preferences pre;
  pre.begin("states", false);
  pre.putInt(Switch.c_str(),Command.toInt()); 
  pre.end();
}

void uwaveControl::read()
{   Switch.trim();
    Preferences pre;
    pre.begin("states", true);
    int state = pre.getInt(Switch.c_str()); 
    pre.end();
    currentState = String(state);
}

void uwaveControl::comm()
{ int action = Command.toInt();
switch (action){
    default:
    {
      server.send(200, "text/plain", "instuction is not performed, check for error");
      break;
    }
    case 0:
    {
      server.send(200, "text/plain", "switch "+ Switch +" will be open");

      //communcation
      Serial.print('-');
      Serial.print(Switch); 
      Serial.print(' ');    
      Serial.println(t);
      //Serial2.printf("-%.s %.s\n",Switch,time);

      break;
    }
    case 1:
    {
      server.send(200, "text/plain", "switch "+ Switch +" will be close");


      Serial.print('+');
      Serial.print(Switch); 
      Serial.print(' ');    
      Serial.println(t); 
      //Serial2.printf(" %.s %.s\n",Switch,time);

      break;
    }
  }
}

void uwaveControl::loadPage(page &Page, String keyInput)
{
  String D_page;
  String C_page;
  for(int i = 0; i < sizeof(destination)/60/4 ; i++){
    if(Page.pageName == destination[i][0].pageName){

      D_page = destination[i][1].pageName;
      C_page = destination[i][2].pageName;
      break;
    }
  }
  lcd.setCursor(0,0);
  lcd.print(Page.firstRowDisplay + " " + *(Page.input1));
  lcd.setCursor(17,0);
  lcd.print("C - " + C_page);

  lcd.setCursor(0,1);
  lcd.print(Page.secondRowDisplay + " " + *(Page.input2));
  lcd.setCursor(17,1);
  lcd.print("D - " + D_page);

  lcd.setCursor(Page.initX,Page.initY);
  lcd.print(keyInput);
}

void uwaveControl::nullFn()
{
  //function suppose to be a dummy
}

void uwaveControl::passwordSwt()
{ Serial.println(password);
  if(password == Password){
      //Serial2.println("123 0000");
      server.send(200,"text/plain","all pins will be set to open");
      Preferences pre;
      pre.begin("states", false);
      pre.clear();
      pre.end();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("All switch");
      lcd.setCursor(0, 1);
      lcd.print("opened");
      delay(1000);
      lcd.clear();
  }
  else{
      server.send(200,"text/plain","Incorrect password");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Incorrect password");
      delay(1000);
      lcd.clear();
  }
}

void uwaveControl::passwordArd()
{
    if(password == Password){
      // Serial2.println("666 000");
      server.send(200,"text/plain","all arduino is reset");
      lcd.setCursor(0, 0);
      lcd.print("Arduino");
      lcd.setCursor(0, 1);
      lcd.print("restarting");
      delay(5000);
      ESP.restart();

  }
  else{
      server.send(200,"text/plain","Incorrect password");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Incorrect password");
      delay(1000);
      lcd.clear();
  }
}

void uwaveControl::openCheck()
{
  if (Command == String(0)){
    lcd.clear();
    uwaveControl::comm();
    uwaveControl::save();
    lcd.setCursor(0,0);
    lcd.print("Switch " + Switch);
    lcd.setCursor(0,1);
    lcd.print("is set to " + Command);
    delay(5000);
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("incorrect key");
    delay(1000);
  }
}

void uwaveControl::closeCheck()
{
  if (Command == String(1)){
    lcd.clear();
    uwaveControl::comm();
    uwaveControl::save();
    lcd.setCursor(0,0);
    lcd.print("Switch " + Switch);
    lcd.setCursor(0,1);
    lcd.print("is set to " + Command);
    delay(5000);
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("incorrect key");
    delay(1000);
  }
}
//wifi control methods
void uwaveControl::testConnection()
{
  server.send(200, "text/plain", "*Firmware* ");
}

void uwaveControl::close()
{
  Switch = server.arg("switch");
  t = server.arg("time");
  
  Command = String(1);
  uwaveControl::save();
  uwaveControl::comm();
}

void uwaveControl::open()
{
  Switch = server.arg("switch");
  t = server.arg("time");
  
  Command = String(0);
  uwaveControl::save();
  uwaveControl::comm();
}

void uwaveControl::reset()
{
  password = server.arg("password");
  uwaveControl::passwordSwt();
}

void uwaveControl::ardReset(){
  password = server.arg("password");
  uwaveControl::passwordArd();
}

void uwaveControl::check()
{
  Switch = server.arg("switch");
  uwaveControl::read();
  server.send(200,"text/plain","swtich "+ Switch +" is "+currentState);
}

void uwaveControl::stringyfy(String list[], String &jsonString)
{
  StaticJsonDocument<2560> doc;
  JsonArray array = doc.to<JsonArray>();
  for (size_t i = 1; i < 29; i++) {
    array.add(list[i]);
  }
  
  serializeJson(doc, jsonString);
}

void uwaveControl::list()
{
  String stateList[29];
  for (int i = 1; i<29; i ++){

    if (i<10) Switch = String(0) + String(i);
    else Switch = String(i);

    uwaveControl::read();
    stateList[i] = "swtich "+ Switch +" is "+currentState;
    Serial.println(stateList[i]);
  }
  
  String jsonString ;
  stringyfy(stateList,jsonString);
  Serial.println(jsonString);
  server.send(200,"application/json",jsonString);    
}



