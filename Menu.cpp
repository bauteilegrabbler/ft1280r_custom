#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "Menu.h"

menuEvent eventFunktion = 0;
byte currentItem = 0;

void setMenuEvent(menuEvent func)
{
  eventFunktion = func;
}

menuItemInfo menuItems[] = 
{
  {"VFO",           eAction_VFO,     1,  0,  0},//0
  
  {"STEP",          eAction_STEP,    7,  6,  2},//1
  {"SIM",           eAction_SIM,    16,  1,  3},//2
  {"OFFSET",        eAction_OFFSET, 17,  2,  4},//3
  {"Memory Read",   eAction_MEM,    20,  3,  5},//4
  {"Memory Write",  eAction_MEM,    29,  4,  6},//5
  {"SCAN",          eAction_SCAN,   38,  5,  1},//6

  //{title,action,springeZu,vorherigerEintrag,naechsterEintrag}
  //STEP
  {"1MHz",          eAction_STEP_1M,    0,  15, 8},//7
  {"100 kHz",       eAction_STEP_100k,  0,  7,  9},//8
  {"50 kHz",        eAction_STEP_50k,   0,  8,  10},//9
  {"25 kHz",        eAction_STEP_25k,   0,  9,  11},//10
  {"12.5kHz",       eAction_STEP_12k5,  0,  10,  12},//11
  {"10 kHz",        eAction_STEP_10k,   0,  11, 13},//12
  {"6.25 kHz",      eAction_STEP_6k25,  0,  12, 14},//13
  {"5 kHz",         eAction_STEP_5k,    0,  13, 15},//14
  {"1 kHz",         eAction_STEP_1k,    0,  14, 7},//15

  //{title,action,springeZu,vorherigerEintrag,naechsterEintrag}
  //SIM
  {"OFFSET=0kHz",   eAction_OFFSET_000,        0,   16,  16},//16 

  //OFFSET
  {"OFFSET = -28MHz",eAction_OFFSET_N28,    0,    19, 18},//17
  {"OFFSET = +28MHz",eAction_OFFSET_P28,    0,    17, 19},//18
  {"OFFSET = +59MHz",eAction_OFFSET_P59,    0,    18, 17},//19
  
  //MEMORY_READ
  {"SPEICHERPLATZ 1",      eAction_MEM_R1,           0,   28,      21},//20
  {"SPEICHERPLATZ 2",      eAction_MEM_R2,           0,   20,      22},//21
  {"SPEICHERPLATZ 3",      eAction_MEM_R3,           0,   21,      23},//22
  {"SPEICHERPLATZ 4",      eAction_MEM_R4,           0,   22,      24},//23
  {"SPEICHERPLATZ 5",      eAction_MEM_R5,           0,   23,      25},//24
  {"SPEICHERPLATZ 6",      eAction_MEM_R6,           0,   24,      26},//25
  {"SPEICHERPLATZ 7",      eAction_MEM_R7,           0,   25,      27},//26
  {"SPEICHERPLATZ 8",      eAction_MEM_R8,           0,   26,      28},//27
  {"SPEICHERPLATZ 9",      eAction_MEM_R9,           0,   27,      29},//28


  //MEMORY_WRITE
  {"SPEICHERPLATZ 1",      eAction_MEM_S1,           0,   37,      30},//29
  {"SPEICHERPLATZ 2",      eAction_MEM_S2,           0,   29,      31},//30
  {"SPEICHERPLATZ 3",      eAction_MEM_S3,           0,   30,      32},//31
  {"SPEICHERPLATZ 4",      eAction_MEM_S4,           0,   31,      33},//32
  {"SPEICHERPLATZ 5",      eAction_MEM_S5,           0,   32,      34},//33
  {"SPEICHERPLATZ 6",      eAction_MEM_S6,           0,   33,      35},//34
  {"SPEICHERPLATZ 7",      eAction_MEM_S7,           0,   34,      36},//35
  {"SPEICHERPLATZ 8",      eAction_MEM_S8,           0,   35,      37},//36
  {"SPEICHERPLATZ 9",      eAction_MEM_S9,           0,   36,      29},//37
  
  
  //RUFTON
  //{"Tonruf+1750Hz",eAction_RUFTON,-1,0,0},

  
  
  //{title,action,springeZu,vorherigerEintrag,naechsterEintrag}
  //scan
  {"Relaiskanäle",      eAction_SCAN_RCHANNEL,    0,   40,      39},//38
  {"Simplexkanäle",     eAction_SCAN_SCHANNEL,    0,   38,     40},//39
  {"Memoryscan",        eAction_SCAN_CHANNEL,     0,   39,      38},//40
  
  //{"irgendwas",e_null, 5,12,7}//43
};

void mystrncpy(char* dest, char* src, int len)
{
  for(int i = 0; i < len;i++)
  {
    dest[i] = src[i];
  }
}

byte mystrlen(char* text)
{
  for(unsigned char i = 0; i < 254;i++)
  {
    if(text[i] == 0)
    return i;
  }
  return 0;
}


lcdline menu_currentItem()
{
  char* text = menuItems[currentItem].title;
  lcdline returnWert;
  mystrncpy(returnWert.text,"                ",16);
  mystrncpy(returnWert.text,text,mystrlen(text));
  if(eventFunktion)eventFunktion(menuItems[currentItem].action, eATyp_DARSTELLEN_Z1,(returnWert.text));
  return returnWert;
}

lcdline menu_nextItem()
{
  byte next = menuItems[currentItem].naechsterEintrag;
  char* text = menuItems[next].title;
  lcdline returnWert;
  mystrncpy(returnWert.text,"                ",16);
  mystrncpy(returnWert.text,text,mystrlen(text));
  if(eventFunktion)eventFunktion(menuItems[currentItem].action, eATyp_DARSTELLEN_Z2,(returnWert.text));
  return returnWert;
}


//druecken
void menu_select()
{
  if(eventFunktion)eventFunktion(menuItems[currentItem].action, eATyp_AUSGEWAEHLT,0);
  currentItem = menuItems[currentItem].springeZu;
}

//nachlinksdrehen
void menu_down()
{
  if(eventFunktion)eventFunktion(menuItems[currentItem].action, eATyp_TURN_DOWN,0);
  currentItem = menuItems[currentItem].vorherigerEintrag;
}

//nachrechtsdrehen
void menu_up()
{
  if(eventFunktion)eventFunktion(menuItems[currentItem].action, eATyp_TURN_UP,0);
  currentItem = menuItems[currentItem].naechsterEintrag;
}
