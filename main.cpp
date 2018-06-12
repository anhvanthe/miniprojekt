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

void refreshScreen(char *lines[]);

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

struct linkedListS { //Doubly linked list for strings

  char *value;
  int *size;
  int *id;
  struct linkedListS *parent;
  struct linkedListS *child;
};

char **llsToLiteralArr(struct linkedListS *lls, int start, int end){
  char **literalArr = malloc(end - start + 1);
  
  struct linkedListS *current = lls;
  char *buf[2];
  while(*((*current).id) != start){
    if((*current).child == NULL) break;
    sprintf(buf[0], "%d", (*current).id);
    buf[1] = NULL;
    refreshScreen(buf);
    current = (*current).child;
  }
  
  int i = 0;
  while((*current).child != NULL && i < end - start){
    literalArr[i] = (*current).value;
    current = (*current).child;
    i++;
  }
  literalArr[i] = NULL;
  return literalArr;
}

int appendTolls(struct linkedListS *lls, char *s){
  *((*lls).size)++;
  struct linkedListS *current = lls;
  while((*current).child != NULL) 
  {
    (*current).size = (*lls).size;
    current = (*current).child;
  }
  struct linkedListS *newLink = malloc(sizeof(struct linkedListS));
  if(newLink == NULL) return 0;
  (*newLink).value = s;
  (*newLink).size = (*lls).size;
  int *id = malloc(sizeof(int));
  if(id == NULL) return 0;
  memcpy(&((*newLink).size), id, sizeof(int));
  *id = *((*newLink).size) - 1;
  (*newLink).id = id;
  (*newLink).parent = current;
  (*newLink).child = NULL;
  (*current).child = newLink;
  return 1;
  
}

struct linkedListS *arrTolls(char **arr){
  struct linkedListS *lls = malloc(sizeof(struct linkedListS));
  (*lls).value = NULL;
  int *id = malloc(sizeof(int));
  *id = 0;
  (*lls).id = id;
  int *size = malloc(sizeof(int));
  *size = 0;
  (*lls).size = size;
  (*lls).child = NULL;
  (*lls).parent = NULL;
  for(int i = 0; i < sizeof(arr)/sizeof(arr[0]); i++){
    if(!appendTolls(lls, arr[i])){
      char buf[35];
      sprintf(buf, "%s%d%s", "malloc failed in appendTolls at: ", i, '\0');
      char *error[2];
      error[0] = buf;
      error[1] = NULL;
      refreshScreen(error);
      while(1);
    }
  }
  return lls;
}

struct profile {
  char **name;
  struct linkedListS *commands; //Contains the commands
};

struct linkedListP {//Doubly linked list for profiles
  struct profile *prfl;
  int size;
  int id ;
  struct linkedListP *parent;
  struct linkedListP *child;
};

struct menu {
  int pointer_pos;
  int currentMenu;
  
  struct linkedListP *profiles;
  
  struct linkedListS *mainMenu;
  struct linkedListS *profMenu;
  struct linkedListS *profInterMenu;
  struct linkedListS *cmdMenu;
  struct linkedListS *newCmdStart;
  struct linkedListS *newCmdSave;
  
  struct linkedListS *current;
  
};

struct keyboard {
  int selectedLine;
  int selected;
  int numberOfLines;
  char *lines[5];
  
  
};

void refreshScreen(char *lines[]){
  RIT128x96x4Clear();
  
  int offset = 0;
  int i = 0;
  char *line = lines[0];
  while(line != NULL){
    RIT128x96x4StringDraw (line, 5, 5 + offset, 3);
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
  refreshScreen(ui);
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
      refreshScreen(ui);
    }
    if(btnUpLastState != 0 && btnUpState == 0){
      nextLine(&kb, -1);
      buldUI(kb.lines, 1, &ui);
      refreshScreen(ui);
    }
    if(btnLeftLastState != 0 && btnLeftState == 0){
      nextChar(&kb, -1);
      buldUI(kb.lines, 1, &ui);
      refreshScreen(ui);
    }
    if(btnRightLastState != 0 && btnRightState == 0){
      nextChar(&kb, 1);
      buldUI(kb.lines, 1, &ui);
      refreshScreen(ui);
    }
    if(btnSelectLastState != 0 && btnSelectState == 0){
      input[pos] = kb.lines[kb.selectedLine][kb.selected];
      pos++;
      input[pos] = '\0';
      ui[0] = input;
      refreshScreen(ui);
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
  (*prfl).name = prfName;
  
  return prfl;
}

void addProfile(struct linkedListP *p){
  struct profile *prfl = createProfile();
  (*p).size = (*p).size + 1;
  struct linkedListP *current = p;
  while((*current).child != NULL){
    current = (*p).child;
    (*current).size = (*p).size;
  }
  struct linkedListP newP;
  newP.prfl = prfl;
  newP.size = (*p).size;
  newP.id = newP.size - 1;
  newP.parent = current;
  (*current).child = &newP;
  
}

void changeMenu(struct menu *m){
  /*
    0: main menu
    1: profile menu
    2: profile intermediate menu
    3: cmd menu
    5: add new cmd
    6: save cmd
  */
  
  switch((*m).currentMenu){
    
    case 0:
      switch((*m).pointer_pos){
        case 0:
          (*m).currentMenu = 1;
          break;
        case 1:
          addProfile((*m).profiles);
          break;
      }
      break;
      
    case 1:
      
  }
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
  
  struct linkedListP *profiles;
  
  char *mainMenuOptions[2] = {"Select profile", "New profile"};
  struct linkedListS *mainMenu = arrTolls(mainMenuOptions);
  struct linkedListS *profMenu;
  char *profInterOptions[2] = {"Select command", "New command"};
  struct linkedListS *profInterMenu = arrTolls(profInterOptions);
  struct linkedListS *cmdMenu;
  char *newCmdStartOptions[1] = {"Start"};
  struct linkedListS *newCmdStart = arrTolls(newCmdStartOptions);
  char *newCmdSaveOptions[1] = {"Save"};
  struct linkedListS *newCmdSave = arrTolls(newCmdSaveOptions);
  
  
  struct menu m = {0, 0, profiles, mainMenu, profMenu, profInterMenu, cmdMenu, newCmdStart, newCmdSave};
  
  int btnUpLastState = 0;
  int btnUpState = 0;
  int btnDownLastState = 0;
  int btnDownState = 0;
  int btnRightLastState = 0;
  int btnRightState = 0;
  int btnLeftLastState = 0;
  int btnLeftState = 0;
  
  char *menuUI[4];
  while(1){
    
    btnUpState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0) & GPIO_PIN_0;
    btnDownState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) & GPIO_PIN_1 ;
    btnLeftState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) &GPIO_PIN_2;
    btnRightState = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) & GPIO_PIN_3;
    
    char **elements = llsToLiteralArr(m.current, 0, 3);
    buildUI(elements, 0, &menuUI);
    refreshScreen(menuUI);
    for(int i = 0; i < 10000000; i++){}
    btnUpLastState = btnUpState;
    btnDownLastState = btnDownState;
    btnLeftLastState = btnLeftState;
    btnRightLastState = btnRightState;
  }
}
