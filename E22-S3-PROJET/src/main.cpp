#include <Arduino.h>
#include <math.h>

#define H_STATUS 1 //1 à l'endroit, -1 à l'envers

int pinV1I_MOT = A0;
int pinV2I_MOT = A1;
int pinV1I_GEN = A2;
int pinV2I_GEN = A3;
int pinVe_MOT = A4;
int pinVs_MOT = A5;
int pinVe_GEN = A6;
int pinVs_GEN = A7; 
int pinVtemp_MOT = A8;
int pinVtemp_GEN = A9;
int pinPOT = A10;
int pinH_POS = 2;
int pinH_NEG = 3;

float DC_MOT;
float DC_GEN;

float calcul_Volt(int pin);
float calcul_Temp(int pin);
float calcul_I(int pin1, int pin2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  /*----------------------------------------PWM MOTEUR----------------------------------------*/

  // RTM_TimerCalc 1.31,  RuntimeMicro.com
  // Timer-3 16-bit, Mode-14 Fast, Top=ICR
  // 100,000 Hz Frequency, Clock is 16 MHz

  TCCR3B = 0x18; // 0001 1000, Disable Timer 
  TCCR3A = 0xA2; // 1010 0010

  ICR3 = 160-1;
  OCR3A = (int) (ICR3 * 0.25);
  OCR3B = (int) (ICR3 * 0.50);
  TCNT3=0x0;

  pinMode(5, OUTPUT); // OC3a
  //pinMode(2, OUTPUT); // OC3b

  TCCR3B |= 1; // Prescale=1, Enable Timer 
  
  /*----------------------------------------PWM GÉNÉRATRICE----------------------------------------*/

  // RTM_TimerCalc 1.31,  RuntimeMicro.com
  // Timer-4 16-bit, Mode-14 Fast, Top=ICR
  // 50,000 Hz Frequency, Clock is 16 MHz

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
  pinMode(pinVs_MOT, INPUT);
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
  DC_MOT=0.00625*round(analogRead(pinPOT)/6.4);
  OCR3A = (int) (ICR3 * DC_MOT);

  float ve_mot=calcul_Volt(pinVe_MOT);
  float vs_mot=calcul_Volt(pinVs_MOT);
  float i_mot=calcul_I(pinV1I_MOT, pinV2I_MOT);
  float rt_mot=calcul_Temp(pinVtemp_MOT);

  float ve_gen=calcul_Volt(pinVe_GEN);
  float vs_gen=calcul_Volt(pinVs_GEN);
  float i_gen=calcul_I(pinV1I_GEN, pinV2I_GEN);
  float rt_gen=calcul_Temp(pinVtemp_GEN);

  delay(100);

  if(ve_mot>=26.0||vs_mot>=26.0||ve_gen>=26.0||vs_gen>=26.0||i_mot>=2.0||i_gen>=2.0||rt_mot<=33000.0||rt_gen<=33000.0){
    delay(1000);
    ve_mot=calcul_Volt(pinVe_MOT);
    vs_mot=calcul_Volt(pinVs_MOT);
    i_mot=calcul_I(pinV1I_MOT, pinV2I_MOT);
    rt_mot=calcul_Temp(pinVtemp_MOT);

    ve_gen=calcul_Volt(pinVe_GEN);
    vs_gen=calcul_Volt(pinVs_GEN);
    i_gen=calcul_I(pinV1I_GEN, pinV2I_GEN);
    rt_gen=calcul_Temp(pinVtemp_GEN);

    if(ve_mot>=26.0||vs_mot>=26.0||ve_gen>=26.0||vs_gen>=26.0||i_mot>=2.0||i_gen>=2.0||rt_mot<=33000.0||rt_gen<=33000.0){
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      Serial.println("ALERT");
    }  
  }

  Serial.println("\r");
  Serial.println(i_mot);


  /*DC_GEN=0.003125*round(analogRead(pinPOT)/3.2);
  OCR4A = (int) (ICR4 * DC_GEN);*/
}

float calcul_Volt(int pin){
  float v=analogRead(pin)*5.4*5.0/1023;
  return v;
}

float calcul_Temp(int pin){
  float v=analogRead(pin)*5.0/1023;
  float r=500000.0/v-100000.0;
  return r;
}

float calcul_I(int pin1, int pin2){
  float v1=calcul_Volt(pin1);
  float v2=calcul_Volt(pin2);
  float i=-(v2-v1)/0.2;
  return i;
}