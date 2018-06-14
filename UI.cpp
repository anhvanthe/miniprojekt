#include "hw_memmap.h"
#include "hw_sysctl.h"
#include "hw_types.h"

#include "sysctl.h"
#include "gpio.h"

#include "rit128x96x4.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//void sendCommand(String cmd){

//}

void refreshScreen(char**, int);

char *replace_char(char* str, int pos, char replace){
    char buf[16];
    if(strlen(str) < 16){

      for(int i = 0; i < strlen(str); i++){
        if(i != pos){
          buf[i] = str[i];
        } else {
          buf[i] = replace;
        }
      }
      buf[strlen(str)] = '\0';
      char *newStr = strdup(buf);

      free(str);
      return newStr;
    } else {
      free(buf);
      return str;
    }
}

void buildUI(char *lines[], int startPos, char *(*ui)[]){
  int i = 0;
  int pos = startPos;
  char *currentLine = lines[0];
  while(currentLine != NULL){
    (*ui)[pos] = currentLine;
    i++;
    pos++;
    currentLine = lines[i];
  }
  (*ui)[pos] = NULL;
}

struct keyboard {
  int selectedLine;
  int selected;
  int numberOfLines;
  char *lines[5];


};

void refreshScreen(char **lines, int pointer){
  RIT128x96x4Clear();
  if(pointer > -1) RIT128x96x4StringDraw (">", 0, 5 + pointer * 10, 3);
  int offset = 0;
  int i = 0;
  char *line = lines[0];
  while(line != NULL){
    RIT128x96x4StringDraw (line, 10, 5 + offset, 3);
    offset += 10;
    i++;
    line = lines[i];
  }

}

void nextChar(struct keyboard *k, int dir){
  int arrLen = strlen((*k).lines[(*k).selectedLine]);
  (*k).lines[(*k).selectedLine] = replace_char((*k).lines[(*k).selectedLine], (*k).selected, tolower((*k).lines[(*k).selectedLine][(*k).selected]));

  if((*k).selected + dir < arrLen && (*k).selected + dir > -1){
    (*k).selected += dir;
  }

  (*k).lines[(*k).selectedLine] = replace_char((*k).lines[(*k).selectedLine], (*k).selected, toupper((*k).lines[(*k).selectedLine][(*k).selected]));
}

void nextLine(struct keyboard *k, int dir){
  (*k).lines[(*k).selectedLine] = replace_char((*k).lines[(*k).selectedLine], (*k).selected, tolower((*k).lines[(*k).selectedLine][(*k).selected]));

  if((*k).selectedLine + dir < (*k).numberOfLines && (*k).selectedLine + dir > -1){
    (*k).selectedLine += dir;
  }

  int arrLen = strlen((*k).lines[(*k).selectedLine]);

  if((*k).selected >= arrLen) (*k).selected = arrLen - 1;
  (*k).lines[(*k).selectedLine] = replace_char((*k).lines[(*k).selectedLine], (*k).selected, toupper((*k).lines[(*k).selectedLine][(*k).selected]));
}

struct cmds{
  int count;
  char **name;
  char **cmd;
};

struct profile {
  char *name;
  struct cmds *cmds;
};

void userInput(char *buf){
  char *input = buf;
  struct keyboard kb = {1, 0, 4, {"1234567890", "Qwertyuiop", "asdfghjkl", "zxcvbnm", NULL}};

  int btnUpLastState = 0;
  int btnUpState = 0;
  int btnDownLastState = 0;
  int btnDownState = 0;
  int btnRightLastState = 0;
  int btnRightState = 0;
  int btnLeftLastState = 0;
  int btnLeftState = 0;
  int btnSelectLastState = 0;
  int btnSelectState = 0;

  int pos = 0;

  char *ui[6];
  ui[0] = "";
  buildUI(kb.lines, 1, &ui);
  refreshScreen(ui, -1);
  while(1){

    btnUpState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) & GPIO_PIN_0;
    btnDownState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) & GPIO_PIN_1 ;
    btnLeftState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) &GPIO_PIN_2;
    btnRightState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) & GPIO_PIN_3;
    btnSelectState = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) & GPIO_PIN_1;
    if(btnLeftState == 0 && btnRightState == 0){
      break;
    }
    if((btnDownLastState != 0 && btnDownState == 0)){
      nextLine(&kb, 1);
      buildUI(kb.lines, 1, &ui);
      refreshScreen(ui, -1);
    }
    if(btnUpLastState != 0 && btnUpState == 0){
      nextLine(&kb, -1);
      buildUI(kb.lines, 1, &ui);
      refreshScreen(ui, -1);
    }
    if(btnLeftLastState != 0 && btnLeftState == 0){
      nextChar(&kb, -1);
      buildUI(kb.lines, 1, &ui);
      refreshScreen(ui, -1);
    }
    if(btnRightLastState != 0 && btnRightState == 0){
      nextChar(&kb, 1);
      buildUI(kb.lines, 1, &ui);
      refreshScreen(ui, -1);
    }
    if(btnSelectLastState != 0 && btnSelectState == 0){
      input[pos] = kb.lines[kb.selectedLine][kb.selected];
      pos++;
      input[pos] = '\0';
      ui[0] = input;
      refreshScreen(ui, -1);
      if(pos >= 5){
        break;
      }
    }

    btnUpLastState = btnUpState;
    btnDownLastState = btnDownState;
    btnLeftLastState = btnLeftState;
    btnRightLastState = btnRightState;
    btnSelectLastState = btnSelectState;
  }
}

struct menu {
  int pointer_pos;
  int currentMenu;
  int profileCount;

  struct profile profiles[5];
  struct profile *currentProfile;

  char **mainMenu;
  char **profMenu;
  char **profInterMenu;
  char **cmdMenu;

};

void createCommand(struct profile *p){
  userInput(p->cmds->name[p->cmds->count + 1]);
  //listenForInput(p->cmds[p->cmdCount]);
  p->cmds->count++;
}

char **getMenu(struct menu *m){

  switch(m->currentMenu){
    case 0:
      return m->mainMenu;
    case 1:
      return m->profMenu;
    case 2:
      return m->profInterMenu;
    case 3:
      return m->cmdMenu;
  }
  return NULL;
}

void updateMenuPointer(struct menu *m, int dir){
  int newPos = m->pointer_pos + dir;
  if(newPos < 0 || getMenu(m)[newPos] == NULL){
    newPos -= dir;;
  }
  m->pointer_pos = newPos;
}

void listenRC5(char *dest){
  char recvd[14];
  int currentPos = 1;
  char *info[2] = {"Send signal", NULL};
  refreshScreen(info, 0);
  while(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_1) & GPIO_PIN_1);
  recvd[0] = '1';
  int us = 900*(SysCtlClockGet()/1000000);
  while(currentPos < 15){
    int val = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_1) & GPIO_PIN_1;
    for(int i= 0; i < us; i++);
    if(!val) recvd[currentPos] = '1';
    else recvd[currentPos] = '0';
    currentPos++;
  }

  for(int i = 0; i < 15; i++){
    replace_char(dest, i, recvd[i]);
  }
}

void addSignal(char **cmdNames){
  int pointerPos = 0;
  int btnUpLastState = 0;
  int btnUpState = 0;
  int btnDownLastState = 0;
  int btnDownState = 0;
  int btnSelectLastState = 0;
  int btnSelectState = 0;

  refreshScreen(cmdNames, pointerPos);
  while(1){

    btnUpState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) & GPIO_PIN_0;
    btnDownState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) & GPIO_PIN_1 ;
    btnSelectState = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) & GPIO_PIN_1;

    if(!btnDownLastState && btnDownState){
      if(*cmdNames[pointerPos + 1] != NULL) pointerPos++;
      refreshScreen(cmdNames, pointerPos);
    }
    if(!btnUpLastState && btnUpState){
      if(*cmdNames[pointerPos - 1] > 0) pointerPos--;
      refreshScreen(cmdNames, pointerPos);
    }
    if(btnSelectLastState && !btnSelectState){
      break;
    }

    btnUpLastState = btnUpState;
    btnDownLastState = btnDownState;
    btnSelectLastState = btnSelectState;
  }
  listenRC5(cmdNames[pointerPos]);
}

void changeMenu(struct menu *m){
  /*
    0: main menu
    1: profile menu
    2: profile intermediate menu
    3: cmd menu
  */

  switch(m->currentMenu){

    case 0:
      switch(m->pointer_pos){
        case 0:
          m->currentMenu = 1;
          break;
        case 1:
          userInput(m->profiles[m->profileCount].name);
          m->profileCount++;
          m->profMenu[m->profileCount] = m->profiles[m->profileCount].name;
          m->profMenu[m->profileCount + 1] = NULL;
          break;
      }
      break;

    case 1:
      switch(m->pointer_pos){
        case 0:
          m->currentMenu = 0;
          m->currentProfile = &m->profiles[m->pointer_pos];
          break;
        default:
          m->currentMenu = 2;
          break;
      }
      break;

    case 2:
      switch(m->pointer_pos){
        m->cmdMenu = m->currentProfile->cmds->name;
        case 0:
          m->currentMenu = 3;
          break;
        case 1:
          addSignal(m->cmdMenu);
          break;
        case 2:
          m->currentMenu = 1;
          break;
      }
      break;
    case 3:
      if(m->pointer_pos != 0); //execute appropriate command
      m->currentMenu = 2;
      break;
  }
  m->pointer_pos = 0;
}

int main()
{
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_8MHZ);
  RIT128x96x4Init(1000000);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
  GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);
  GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

  char profileNames[17][5];
  struct cmds cmnds[5];
  char *cmdnList[6] = {"CMD1", "CMD2", "CMD3", "CMD4", "CMD5", NULL};
  char *cmdNames[6][5];

  char *cmdInit[6] = {"00000000000000\0", "00000000000000\0","00000000000000\0","00000000000000\0","00000000000000\0"};
  char *commands[5][5];

  char *mainMenu[3] = {"Select profile", "New profile", NULL};
  char *profInterMenu[4] = {"Select command", "New command", "Back", NULL};
  char *profMenu[5];
  profMenu[0] = "Back";
  profMenu[1] = NULL;

  struct menu m;

  for(int i = 0; i < 5; i++){
    m.profiles[i].name = profileNames[i];
    m.profiles[i].cmds = &cmnds[i];
    m.profiles[i].cmds->count = 5;
    for(int j = 0; j < 6; j++) cmdNames[j][i] = strdup(cmdnList[j]);
    m.profiles[i].cmds->name = cmdNames[i];
    for(int j = 0; j < 5; j++) commands[j][i] = strdup(cmdInit[j]);
    m.profiles[i].cmds->cmd = commands[i];
  }

  m.currentMenu = 0;
  m.pointer_pos = 0;
  m.profileCount = 0;
  m.mainMenu = mainMenu;
  m.profInterMenu = profInterMenu;
  m.profMenu = profMenu;

  int btnUpLastState = 0;
  int btnUpState = 0;
  int btnDownLastState = 0;
  int btnDownState = 0;
  int btnSelectLastState = 0;
  int btnSelectState = 0;

  char *menuUI[129];
  while(1){

    btnUpState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) & GPIO_PIN_0;
    btnDownState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) & GPIO_PIN_1 ;
    btnSelectState = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) & GPIO_PIN_1;

    if(!btnDownLastState && btnDownState){
      updateMenuPointer(&m, 1);
      char **elements = getMenu(&m);
      buildUI(elements, 0, &menuUI);
      refreshScreen(menuUI, m.pointer_pos);
    }
    if(!btnUpLastState && btnUpState){
      updateMenuPointer(&m, -1);
      char **elements = getMenu(&m);
      buildUI(elements, 0, &menuUI);
      refreshScreen(menuUI, m.pointer_pos);
    }
    if(btnSelectLastState && !btnSelectState){
      changeMenu(&m);
      char **elements = getMenu(&m);
      buildUI(elements, 0, &menuUI);
      refreshScreen(menuUI, m.pointer_pos);
    }

    btnUpLastState = btnUpState;
    btnDownLastState = btnDownState;
    btnSelectLastState = btnSelectState;
  }
}
