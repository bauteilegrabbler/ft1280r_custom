#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <I2C.h>
#include <EE24xx.h>
#include <ClickEncoder.h>
#include "Menu.h"
#include "Si5351.h"

typedef struct DatenStruktur_t {
  uint64_t frequenz1;
  uint64_t frequenz2;
  uint64_t frequenz3;
  uint64_t frequenz4;
  uint64_t frequenz5;
  uint64_t frequenz6;
  uint64_t frequenz7;
  uint64_t frequenz8;
  uint64_t frequenz9;
} DatenStruktur;

const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//globale variablen ////////////////////////////
eAction step = eAction_STEP_1M;
eAction offset = eAction_OFFSET_000;
bool pttActive = false;
uint64_t frequency = 1270000000;
DatenStruktur daten;
EE24xx m24c08 = EE24xx(0x54, EE24x08);
#define FREQ_MIN 1240000000
#define FREQ_MAX 1300000000
//Datasheet OSC si5351 ==> Figure 8
#define OSC_ADDRESS_A0_0 0b1100000
#define OSC_ADDRESS_A0_1 0b1100001

// PINNING /////////////////////////////////////
#define encA 2
#define encB 3
#define PIN_PTT 4
#define encBtn 5
#define PIN_LED_RX_TX 7
///////////////////////////////////////////////

ClickEncoder clickEncoder(encA,encB,encBtn,4, HIGH, LOW);
void timerIsr() {clickEncoder.service();}

eATyp previousTyp = 0;
eAction previousAction = 0;
void event(eAction action, eATyp typ,char* text)
{
  if(action == eAction_VFO)
  { 
     if(typ==eATyp_DARSTELLEN_Z1)
     {
        uint16_t p3 = frequency%1000;      
        uint16_t p2 = ((uint32_t)(frequency/1000))%1000;
        uint16_t p1 = (frequency/1000000)%10000;
        sprintf(text, "%04d.%03d,%03d MHz",p1,p2,p3);
     }
     else if(typ==eATyp_DARSTELLEN_Z2)
     {
        char stepText[5];
        char offsetText[7];
        memset(stepText,0,5);
        memset(offsetText,0,7);
        switch(step)
        {
          case eAction_STEP_100k: mystrncpy(stepText,"100k",4);break;
          case eAction_STEP_50k: mystrncpy(stepText,"50k",4);break;
          case eAction_STEP_25k: mystrncpy(stepText,"25k",4);break;
          case eAction_STEP_12k5: mystrncpy(stepText,"12k5",4);break;
          case eAction_STEP_10k: mystrncpy(stepText,"10k",3);break;
          case eAction_STEP_6k25: mystrncpy(stepText,"6k25",4);break;
          case eAction_STEP_5k: mystrncpy(stepText,"5k",2);break;
          case eAction_STEP_1k: mystrncpy(stepText,"1k",2);break;
          case eAction_STEP_1M:mystrncpy(stepText,"1MHz",4);break;
          default: mystrncpy(stepText,"????",4);break;
        }
        switch(offset)
        {
          case eAction_OFFSET_000:mystrncpy(offsetText," 0 MHz",6);break;
          case eAction_OFFSET_P28:mystrncpy(offsetText,"+28MHz",6);break;
          case eAction_OFFSET_N28:mystrncpy(offsetText,"-28MHz",6);break;
          case eAction_OFFSET_P59:mystrncpy(offsetText,"+59MHz",6);break;
          
          default:mystrncpy(offsetText,"?????",5);break;
        }
        bool isFreqSet = false;
        bool isSending = pttActive;
        sprintf(text+0, "%4s %s %s %6s",stepText,isSending?"\xFF":"\xDB",isFreqSet?"*":"o",offsetText);
     }
     else//? das ist der fall (ausgewaehlt, turnup, turndown), also der button
     {  
        uint32_t stepValue = 1000;
        switch(step)
        {
          case eAction_STEP_100k: stepValue=100000;break;
          case eAction_STEP_50k: stepValue=50000;break;
          case eAction_STEP_25k: stepValue=25000;break;
          case eAction_STEP_12k5: stepValue=12500;break;
          case eAction_STEP_10k: stepValue=10000;break;
          case eAction_STEP_6k25: stepValue=6250;break;
          case eAction_STEP_5k: stepValue=5000;break;
          case eAction_STEP_1k: stepValue=1000;break;
          case eAction_STEP_1M: stepValue=1000000;break;
          default: break;
        }
        if(typ==eATyp_TURN_UP)
        {
          frequency += stepValue;
        }
        else if (typ==eATyp_TURN_DOWN)
        {
          frequency -= stepValue;
        }
        if(frequency < FREQ_MIN)frequency = FREQ_MAX-stepValue;
        if(frequency >= FREQ_MAX)frequency = FREQ_MIN;
        oszillatorAnsteuern();
     }
   
  }
  else
  {
    switch(action)
    {
      case eAction_STEP_1M:
      case eAction_STEP_100k:
      case eAction_STEP_50k:
      case eAction_STEP_25k:
      case eAction_STEP_12k5:
      case eAction_STEP_10k:
      case eAction_STEP_6k25:
      case eAction_STEP_5k:
      case eAction_STEP_1k:
        //step action
        step = action;break;
      case eAction_OFFSET_000:
      case eAction_OFFSET_P28:
      case eAction_OFFSET_N28:
      case eAction_OFFSET_P59:
        offset = action;break;
      case eAction_MEM_S1:daten.frequenz1 = frequency;datenSpeichern();break;
      case eAction_MEM_S2:daten.frequenz2 = frequency;datenSpeichern();break;
      case eAction_MEM_S3:daten.frequenz3 = frequency;datenSpeichern();break;
      case eAction_MEM_S4:daten.frequenz4 = frequency;datenSpeichern();break;
      case eAction_MEM_S5:daten.frequenz5 = frequency;datenSpeichern();break;
      case eAction_MEM_S6:daten.frequenz6 = frequency;datenSpeichern();break;
      case eAction_MEM_S7:daten.frequenz7 = frequency;datenSpeichern();break;
      case eAction_MEM_S8:daten.frequenz8 = frequency;datenSpeichern();break;
      case eAction_MEM_S9:daten.frequenz9 = frequency;datenSpeichern();break;
      
      case eAction_MEM_R1:frequency = daten.frequenz1;break;
      case eAction_MEM_R2:frequency = daten.frequenz2;break;
      case eAction_MEM_R3:frequency = daten.frequenz3;break;
      case eAction_MEM_R4:frequency = daten.frequenz4;break;
      case eAction_MEM_R5:frequency = daten.frequenz5;break;
      case eAction_MEM_R6:frequency = daten.frequenz6;break;
      case eAction_MEM_R7:frequency = daten.frequenz7;break;
      case eAction_MEM_R8:frequency = daten.frequenz8;break;
      case eAction_MEM_R9:frequency = daten.frequenz9;break;
    }
    if(action==previousAction && previousTyp == typ)return;
    previousAction = action;
    previousTyp = typ;
    Serial.print("Menu-Aktion ");
    Serial.print(action);
    Serial.print(" ");
    switch(typ)
    { 
       case eATyp_DARSTELLEN_Z1:
       case eATyp_DARSTELLEN_Z2:
          break;
       case eATyp_AUSGEWAEHLT:
          Serial.print("AUSGEWAEHLT");
          break;
       case eATyp_TURN_UP:
          Serial.print("TURN_UP");
          break;
       case eATyp_TURN_DOWN:
          Serial.print("TURN_DOWN");
          break;
    }  
    Serial.println("");
  }
}

void setup() {
  setMenuEvent(event);
  pinMode(encBtn,INPUT_PULLUP);
  pinMode(LED_BUILTIN,OUTPUT);
  //PTT
  pinMode(PIN_PTT,INPUT_PULLUP);
  I2c.begin();
  si5351aInit();
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Arduino Menu Library");Serial.flush();
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  lcd.print("Menu 0.x LCD");
  lcd.setCursor(0, 1);
  lcd.print("Jacobs");
  Timer1.initialize(100);
  Timer1.attachInterrupt(timerIsr);
  delay(2000);
  datenLaden();
}

void runButton()
{
  
  ClickEncoder::Button buttonState = clickEncoder.getButton();
  if(buttonState == ClickEncoder::Open)
  {
    int16_t value = clickEncoder.getValue();
    if(value < 0)
    {
      //gegen uhrzeiger gedreht
      menu_down();
      Serial.println("ccw");
    }
    else if(value > 0)
    {
      menu_up();
      Serial.println("cw");
    }
  }
  else if(buttonState == ClickEncoder::Clicked)
  {
    Serial.println("push");
    menu_select();
  }
}

void runPTT()
{
  pttActive = digitalRead(PIN_PTT)?false:true;//digitalRead(PIN_PTT);
  
  digitalWrite(PIN_LED_RX_TX,pttActive?LOW:HIGH);
  
}

void oszillatorAnsteuern()
{
  Serial.print("freqEingestellt= ");
  Serial.println((long)frequency);
  float targetF = frequency / 128.0f;
  Serial.println(targetF);
  long targetFreq = (long)targetF;
  Serial.println(targetFreq);
  si5351aSetFrequency(targetFreq);
}

void datenSpeichern()
{
  m24c08.write(0, (uint8_t*)&daten, sizeof(daten));
}

void datenLaden()
{
  m24c08.read(0, (uint8_t*)&daten, sizeof(daten));
}

void anzeigen()
{
  lcdline line1 = menu_currentItem();
  lcdline line2 = menu_nextItem();
  lcd.setCursor(0,0);
  lcd.print(line1.text);
  lcd.setCursor(0,1);
  lcd.print(line2.text);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1);//simulate a delay as if other tasks are running
  runButton();
  runPTT();
  anzeigen();
}
