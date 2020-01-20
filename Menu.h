#ifndef MENU_FLORIAN_H
#define MENU_FLORIAN_H

#include <stdint.h>

typedef uint8_t byte;


enum eAction
{
  eAction_VFO,
  eAction_STEP,
  eAction_STEP_1M,
  eAction_STEP_100k,
  eAction_STEP_50k,
  eAction_STEP_25k,
  eAction_STEP_12k5,
  eAction_STEP_10k,
  eAction_STEP_6k25,
  eAction_STEP_5k,
  eAction_STEP_1k,
  eAction_SIM,
  eAction_OFFSET,
  eAction_OFFSET_P59,
  eAction_OFFSET_P28,
  eAction_OFFSET_N28,
  eAction_OFFSET_000,
  eAction_RUFTON,
  eAction_MEM,
  eAction_SCAN,
  eAction_SCAN_RCHANNEL,
  eAction_SCAN_SCHANNEL,
  
  eAction_MEM_S1,
  eAction_MEM_S2,
  eAction_MEM_S3,
  eAction_MEM_S4,
  eAction_MEM_S5,
  eAction_MEM_S6,
  eAction_MEM_S7,
  eAction_MEM_S8,
  eAction_MEM_S9,
  
  eAction_MEM_R1,
  eAction_MEM_R2,
  eAction_MEM_R3,
  eAction_MEM_R4,
  eAction_MEM_R5,
  eAction_MEM_R6,
  eAction_MEM_R7,
  eAction_MEM_R8,
  eAction_MEM_R9,
  
  eAction_SCAN_CHANNEL,
  eAction_Back,
};

struct menuItemInfo
{
  char title[16];
  eAction action;
  byte springeZu;//jump to
  byte vorherigerEintrag;
  byte naechsterEintrag;
};

enum eATyp {
  eATyp_DARSTELLEN_Z1,
  eATyp_DARSTELLEN_Z2,
  eATyp_AUSGEWAEHLT,
  eATyp_TURN_UP,
  eATyp_TURN_DOWN,  
};


struct lcdline
{
  char text[16];
};

void mystrncpy(char* dest, char* src, int len);
byte mystrlen(char* text);


void menu_select();
void menu_down();
void menu_up();
lcdline menu_currentItem();
lcdline menu_nextItem();

typedef void (*menuEvent)(eAction action, eATyp typ, char* text);

void setMenuEvent(menuEvent func);
#endif //MENU_FLORIAN_H
