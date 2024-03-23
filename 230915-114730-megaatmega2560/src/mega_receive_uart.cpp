#include <Arduino.h>
static const uint8_t analog_pins[] = {A0,A2,A4,A6,A8,A10,A12,A1,A3,A5,A7,A9,A11,A13};
static uint8_t output_pins[] = {0,0,0,0};
//static int output_pins[4] = {};
//#define P1 2
//#define P2 3
uint8_t In_1[4][7] = {{13,11,9,7,5,3,14}, {A0,A2,A4,A6,A8,A10,A12}, {16,18,20,22,24,27,31}, {36,34,30,35,39,43,47}};
uint8_t In_2[4][7] = {{12,10,8,6,4,2,15}, {A1,A3,A5,A7,A9,A11,A13}, {17,19,21,23,26,25,29}, {38,32,28,33,37,41,45}};

int rounding(int value){
  int out = value; 
  if(value<0){
    out = 0;
  }
  return out;
}

void control(int v,int time){
  int relay = abs(v)/6;
  int Switch = (relay + abs(v)) % 7;

  output_pins[0] = In_1[relay][Switch];
  output_pins[1] = In_2[relay][Switch];
  output_pins[2] = In_1[relay][Switch+1];
  output_pins[3] = In_2[relay][Switch+1];
  /*Serial.print(v);
  Serial.print(" ");
  Serial.println(time);*/
  digitalWrite(output_pins[0],rounding(v));
  digitalWrite(output_pins[1],rounding(-1*v));
  digitalWrite(output_pins[2],rounding(-1*v));
  digitalWrite(output_pins[3],rounding(v));
  delay(time);
  digitalWrite(output_pins[0],HIGH);
  digitalWrite(output_pins[1],HIGH);
  digitalWrite(output_pins[2],HIGH);
  digitalWrite(output_pins[3],HIGH);
}

void reset(){
  for(int i=0;i<29;i++){
    control (-i,0);
  }
}

int max_out_A_pin = 1;
int max_out_D_pin = 6;

void setup() {
Serial1.begin(9600); 
Serial.begin(9600);
 for (int i = 0; i <= max_out_A_pin; i++) {
 // pinMode(analog_pins[i], INPUT); 
  }
  for(int i = 0; i<3; i++){
    for(int j =0; j<6; i++){
      pinMode(In_1[i][j],OUTPUT);
      digitalWrite(In_1[i][j],HIGH);
      pinMode(In_2[i][j],OUTPUT);
      digitalWrite(In_2[i][j],HIGH);      
    }
  }
}

char pinc[4];
char timec[5];
void loop() {
  //read input provided by esp32
  String message; 
  int divider; 
    if (Serial.available()) { // Check if there's incoming serial data
      message = Serial.readString();
      if (message.length() > 1) { // check if the message is not empty
        for(unsigned int i = 0; i < message.length(); i++){
          if(message.charAt(i) == ' '){
            divider = i;
            break;
          }
        }
        for (int i = 0; i < divider; i++) {
          pinc[i] = message.charAt(i);
       }
        for (unsigned int i = divider+1; i < message.length() ; i++) {
          timec[i-divider-1] = message.charAt(i);
       }
        int Switch = atoi(pinc);
        int time = atoi(timec);
        Serial.println(message);
        if(Switch == 123){
          reset();
        }
        else{
          Serial.println(Switch);
          Serial.println(time);
          control(Switch,time);
        }
      }
    }
}