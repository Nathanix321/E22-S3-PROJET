#include <Arduino.h>
#include <math.h>

#define H_STATUS 1 //1 à l'endroit, -1 à l'envers

int pinV1I_MOT = A0;
int pinV2I_MOT = A1;
int pinV1I_GEN = A2;
int pinV2I_GEN = A3;
int pinVe_MOT = A4;
int pinVs_mot = A5;
int pinVe_GEN = A6;
int pinVs_GEN = A7; 
int pinVtemp_MOT = A8;
int pinVtemp_GEN = A9;
int pinPOT = A10;
int pinH_POS = 2;
int pinH_NEG = 3;

float DC_MOT;
float DC_GEN;

float calcul_Temp(int pin);
float calcul_I(int pin1, int pin2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*----------------------------------------PWM MOTEUR----------------------------------------*/

  // RTM_TimerCalc 1.31,  RuntimeMicro.com
  // Timer-3 16-bit, Mode-14 Fast, Top=ICR
  // 500,000 Hz Frequency, Clock is 16 MHz
  
  DC_MOT = 0.05;

  TCCR3B = 0x18; // 0001 1000, Disable Timer 
  TCCR3A = 0xA2; // 1010 0010

  ICR3 = 320-1;
  OCR3A = (int) (ICR3 * 0.25);
  OCR3B = (int) (ICR3 * 0.50);
  TCNT3=0x0;

  pinMode(5, OUTPUT); // OC3a
  pinMode(2, OUTPUT); // OC3b

  TCCR3B |= 1; // Prescale=1, Enable Timer 

 

  
  /*----------------------------------------PWM GÉNÉRATRICE----------------------------------------*/

  // RTM_TimerCalc 1.31,  RuntimeMicro.com
  // Timer-4 16-bit, Mode-14 Fast, Top=ICR
  // 100,000 Hz Frequency, Clock is 16 MHz

  DC_GEN = 0.15;

  TCCR4B = 0x18; // 0001 1000, Disable Timer 
  TCCR4A = 0xA2; // 1010 0010

  ICR4 = 320-1;
  OCR4A = (int) (ICR4 * 0.25);
  OCR4B = (int) (ICR4 * 0.50);
  TCNT4=0x0;

  pinMode(6, OUTPUT); // OC4a
  //pinMode(7, OUTPUT); // OC4b

  TCCR4B |= 1; // Prescale=1, Enable Timer 
 

  /*----------------------------------------SETUP PINS----------------------------------------*/
  pinMode(pinV1I_MOT, INPUT);
  pinMode(pinV2I_MOT, INPUT);
  pinMode(pinV1I_GEN, INPUT);
  pinMode(pinV2I_GEN, INPUT);
  pinMode(pinVe_MOT, INPUT);
  pinMode(pinVs_mot, INPUT);
  pinMode(pinVe_GEN, INPUT);
  pinMode(pinVs_GEN, INPUT);
  pinMode(pinVtemp_MOT, INPUT);
  pinMode(pinVtemp_GEN, INPUT);
  pinMode(pinH_POS, OUTPUT);
  pinMode(pinH_NEG, OUTPUT);

  if(H_STATUS==1){
    digitalWrite(pinH_POS, HIGH);
    digitalWrite(pinH_NEG, LOW);
  }
  else if(H_STATUS==-1){
    digitalWrite(pinH_POS, LOW);
    digitalWrite(pinH_NEG, HIGH);
  }
}

void loop(){
  /*DC_MOT=0.003125*round(analogRead(pinPOT)/3.2);
  OCR3A = (int) (ICR3 * DC_MOT);*/

  DC_GEN=0.003125*round(analogRead(pinPOT)/3.2);
  OCR4A = (int) (ICR4 * DC_GEN);
  Serial.print(DC_GEN);
}

float calcul_Temp(int pin){
  float v=(5/1023)*analogRead(pin);
  float r=(-v*100000)/(v-5);
  float t=pow((((log(r/100000))/4303)+1/25), -1);
  return t;
}

float calcul_I(int pin1, int pin2){
  float v1=5.4*(5/1023)*analogRead(pin1);
  float v2=5.4*(5/1023)*analogRead(pin2);
  float i=(v2-v1)/0.2;
  return i;
}