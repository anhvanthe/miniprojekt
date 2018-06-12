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


struct profile {
  char **name;
  char *cmdMenu[128];
};

struct menu {
  int pointer_pos;
  int currentMenu;
  
  struct profile **profiles;
  
  char **mainMenu;
  char **profMenu;
  char **profInterMenu;
  char **cmdMenu;
  char **newCmdStart;
  char **newCmdSave;
  
};

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
    case 4:
      return m->newCmdStart;
    case 5:
      return m->newCmdSave;
  }
}

void updateMenuPointer(struct menu *m, int dir){
  int newPos = m->pointer_pos + dir;
  if(newPos > -1 && getMenu(m)[m->currentMenu][m->pointer_pos] != NULL){
    m->pointer_pos = newPos;
  }
}

void changeMenu(struct menu *m){
  /*
    0: main menu
    1: profile menu
    2: profile intermediate menu
    3: cmd menu
    4: add new cmd
    5: save cmd
  */
  
  switch((*m).currentMenu){
    
    case 0:
      switch(m->pointer_pos){
        case 0:
          m->currentMenu = 1;
          break;
        case 1:
          //addProfile((*m).profiles);
          break;
      }
      break;
      
    case 1:
      //get profile
      m->currentMenu = 2;
      break;
      
    case 2:
      switch(m->pointer_pos){
        case 0:
          //cmd menu
          break;
        case 1:
          m->currentMenu = 4;
          break;
      }
      break;
    case 3:
      //cmdMenu
      m->currentMenu = 2;
      break;
    case 4:
      m->currentMenu = 5;
      break;
    case 5:
      m->currentMenu = 2;
      break;
  }
  m->pointer_pos = 0;
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

char* userInput(){
  char input[17];
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
  buldUI(kb.lines, 1, &ui);
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
      buldUI(kb.lines, 1, &ui);
      refreshScreen(ui, -1);
    }
    if(btnUpLastState != 0 && btnUpState == 0){
      nextLine(&kb, -1);
      buldUI(kb.lines, 1, &ui);
      refreshScreen(ui, -1);
    }
    if(btnLeftLastState != 0 && btnLeftState == 0){
      nextChar(&kb, -1);
      buldUI(kb.lines, 1, &ui);
      refreshScreen(ui, -1);
    }
    if(btnRightLastState != 0 && btnRightState == 0){
      nextChar(&kb, 1);
      buldUI(kb.lines, 1, &ui);
      refreshScreen(ui, -1);
    }
    if(btnSelectLastState != 0 && btnSelectState == 0){
      input[pos] = kb.lines[kb.selectedLine][kb.selected];
      pos++;
      input[pos] = '\0';
      ui[0] = input;
      refreshScreen(ui, -1);
      if(pos >= 17){
        break;
      }
    }
    
    btnUpLastState = btnUpState;
    btnDownLastState = btnDownState;
    btnLeftLastState = btnLeftState;
    btnRightLastState = btnRightState;
    btnSelectLastState = btnSelectState;
  }
  
  return strdup(input);
}


struct profile *createProfile(){
  struct profile *prfl = malloc(sizeof(struct profile));
  int pos;
  char temp[16];
  char *prfName[16] = malloc(sizeOf(temp));
  *prfName = userInput();
  prfl->name = prfName;
  
  return prfl;
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
  
  struct profile *profiles;
  
  char *mainMenu[3] = {"Select profile", "New profile", NULL};
  char *profInterMenu[3] = {"Select command", "New command", NULL};
  char *profMenu[19];
  profMenu[0] = "";
  profMenu[1] = NULL;
  char *cmdMenu[129];
  cmdMenu[0] = "";
  cmdMenu[1] = NULL;
  char *newCmdStart[2] = {"Start", NULL};
  char *newCmdSave[2] = {"Save", NULL};
 
  struct menu m;
  m.currentMenu = 0;
  m.pointer_pos = 0;
  m.profiles = &profiles;
  m.mainMenu = mainMenu;
  m.profInterMenu = profInterMenu;
  m.profMenu = profMenu;
  m.cmdMenu = cmdMenu;
  m.newCmdStart = newCmdStart;
  m.newCmdSave = newCmdSave;
  
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
  
  char *menuUI[129];
  while(1){
    
    btnUpState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) & GPIO_PIN_0;
    btnDownState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) & GPIO_PIN_1 ;
    btnLeftState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) &GPIO_PIN_2;
    btnRightState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) & GPIO_PIN_3;
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
    btnLeftLastState = btnLeftState;
    btnRightLastState = btnRightState;
    btnSelectLastState = btnSelectState;
  }
}
