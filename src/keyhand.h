#include<stdio.h>
#include "c:\source\include\key.h"
#include "c:\source\include\code.h"

#define SCANCODES 0
#define THENAMES  1

void setupSingleHitKeys(void)
 {
 toggleSingleHitKey(KEY_TAB);
// toggleSingleHitKey(KEY_F); 
 toggleSingleHitKey(KEY_1);
 toggleSingleHitKey(KEY_2);
 toggleSingleHitKey(KEY_3);
 toggleSingleHitKey(KEY_4);
 toggleSingleHitKey(KEY_5);
 toggleSingleHitKey(KEY_6);
 toggleSingleHitKey(KEY_ENTER);
 }

void setupAliasKeys(void)
 {
 setAliasKey(ALIAS_GOFORWARD,KEY_UPARROW);
 setAliasKey(ALIAS_GOBACKWARD,KEY_DOWNARROW);
 setAliasKey(ALIAS_ROTATELEFT,KEY_LEFTARROW);
 setAliasKey(ALIAS_ROTATERIGHT,KEY_RIGHTARROW);
 setAliasKey(ALIAS_MAP,KEY_TAB);
 }

void printScanCodes(void)
 {
 long i,flag;
 flag=0;for(i=0;i<256;i++){if(checkKey(i)){flag=1;printf("%x ",i);}}
 if(flag)printf("\n");
 }

void printKeys(char whichWay)
 {
 if(whichWay==SCANCODES){printScanCodes();return;}
 
 if(checkKey(KEY_BREAK))
  {
  printf("KEY_BREAK\n");
  printf("Hit a key to continue...\n");
  clearKeys();
  waitKey();
  return;
  }
 
 
 if(checkKey(ALIAS_GOFORWARD))printf("ALIAS_GOFORWARD\n");
 if(checkKey(ALIAS_GOBACKWARD))printf("ALIAS_GOBACKWARD\n");
 if(checkKey(ALIAS_ROTATELEFT))printf("ALIAS_ROTATELEFT\n");
 if(checkKey(ALIAS_ROTATERIGHT))printf("ALIAS_ROTATERIGHT\n");
 if(checkKey(ALIAS_MAP))printf("ALIAS_MAP\n");
 
 if(checkKey(KEY_ESC))printf("KEY_ESC\n");
 if(checkKey(KEY_F1))printf("KEY_F1\n");
 if(checkKey(KEY_F2))printf("KEY_F2\n");
 if(checkKey(KEY_F3))printf("KEY_F3\n");
 if(checkKey(KEY_F4))printf("KEY_F4\n");
 if(checkKey(KEY_F5))printf("KEY_F5\n");
 if(checkKey(KEY_F6))printf("KEY_F6\n");
 if(checkKey(KEY_F7))printf("KEY_F7\n");
 if(checkKey(KEY_F8))printf("KEY_F8\n");
 if(checkKey(KEY_F9))printf("KEY_F9\n");
 if(checkKey(KEY_F10))printf("KEY_F10\n");
 if(checkKey(KEY_F11))printf("KEY_F11\n");
 if(checkKey(KEY_F12))printf("KEY_F12\n");
 if(checkKey(KEY_SYSRQ))printf("KEY_SYSREQ\n");
 if(checkKey(KEY_SCROLLLOCK))printf("KEY_SCROLLLOCK\n");
 
 
 if(checkKey(KEY_TICK))printf("KEY_TICK\n");
 if(checkKey(KEY_1))printf("KEY_1\n");
 if(checkKey(KEY_2))printf("KEY_2\n");
 if(checkKey(KEY_3))printf("KEY_3\n");
 if(checkKey(KEY_4))printf("KEY_4\n");
 if(checkKey(KEY_5))printf("KEY_5\n");
 if(checkKey(KEY_6))printf("KEY_6\n");
 if(checkKey(KEY_7))printf("KEY_7\n");
 if(checkKey(KEY_8))printf("KEY_8\n");
 if(checkKey(KEY_9))printf("KEY_9\n");
 if(checkKey(KEY_0))printf("KEY_0\n");
 if(checkKey(KEY_MINUS))printf("KEY_MINUS\n");
 if(checkKey(KEY_EQUAL))printf("KEY_EQUAL\n");
 if(checkKey(KEY_BACKSPACE))printf("KEY_BACKSPACE\n");
 if(checkKey(KEY_INSERT))printf("KEY_INSERT\n");
 if(checkKey(KEY_HOME))printf("KEY_HOME\n");
 if(checkKey(KEY_PAGEUP))printf("KEY_PAGEUP\n");
 if(checkKey(KEY_NUMLOCK))printf("KEY_NUMLOCK\n");
 if(checkKey(KEY_PADSLASH))printf("KEY_PADSLASH\n");
 if(checkKey(KEY_PADSTAR))printf("KEY_PADSTAR\n");
 if(checkKey(KEY_PADMINUS))printf("KEY_PADMINUS\n");
 
 if(checkKey(KEY_TAB))printf("KEY_TAB\n");
 if(checkKey(KEY_Q))printf("KEY_Q\n");
 if(checkKey(KEY_W))printf("KEY_W\n");
 if(checkKey(KEY_E))printf("KEY_E\n");
 if(checkKey(KEY_R))printf("KEY_R\n");
 if(checkKey(KEY_T))printf("KEY_T\n");
 if(checkKey(KEY_Y))printf("KEY_Y\n");
 if(checkKey(KEY_U))printf("KEY_U\n");
 if(checkKey(KEY_I))printf("KEY_I\n");
 if(checkKey(KEY_O))printf("KEY_O\n");
 if(checkKey(KEY_P))printf("KEY_P\n");
 if(checkKey(KEY_LEFTBRACKET))printf("KEY_LEFTBRACKET\n");
 if(checkKey(KEY_RIGHTBRACKET))printf("KEY_RIGHTBRACKET\n");
 if(checkKey(KEY_BACKSLASH))printf("KEY_BACKSLASH\n");
 if(checkKey(KEY_DELETE))printf("KEY_DELETE\n");
 if(checkKey(KEY_END))printf("KEY_END\n");
 if(checkKey(KEY_PAGEDOWN))printf("KEY_PAGEDOWN\n");
 if(checkKey(KEY_PAD7))printf("KEY_PAD7\n");
 if(checkKey(KEY_PAD8))printf("KEY_PAD8\n");
 if(checkKey(KEY_PAD9))printf("KEY_PAD9\n");
 if(checkKey(KEY_PADPLUS))printf("KEY_PADPLUS\n");
 
 if(checkKey(KEY_CAPSLOCK))printf("KEY_CAPSLOCK\n");
 if(checkKey(KEY_A))printf("KEY_A\n");
 if(checkKey(KEY_S))printf("KEY_S\n");
 if(checkKey(KEY_D))printf("KEY_D\n");
 if(checkKey(KEY_F))printf("KEY_F\n");
 if(checkKey(KEY_G))printf("KEY_G\n");
 if(checkKey(KEY_H))printf("KEY_H\n");
 if(checkKey(KEY_J))printf("KEY_J\n");
 if(checkKey(KEY_K))printf("KEY_K\n");
 if(checkKey(KEY_L))printf("KEY_L\n");
 if(checkKey(KEY_SEMICOLON))printf("KEY_SEMICOLON\n");
 if(checkKey(KEY_APOSTROPHE))printf("KEY_APOSTROPHE\n");
 if(checkKey(KEY_ENTER))printf("KEY_ENTER\n");
 if(checkKey(KEY_PAD4))printf("KEY_PAD4\n");
 if(checkKey(KEY_PAD5))printf("KEY_PAD5\n");
 if(checkKey(KEY_PAD6))printf("KEY_PAD6\n");
 
 if(checkKey(KEY_LEFTSHIFT))printf("KEY_LEFTSHIFT\n");
 if(checkKey(KEY_Z))printf("KEY_Z\n");
 if(checkKey(KEY_X))printf("KEY_X\n");
 if(checkKey(KEY_C))printf("KEY_C\n");
 if(checkKey(KEY_V))printf("KEY_V\n");
 if(checkKey(KEY_B))printf("KEY_B\n");
 if(checkKey(KEY_N))printf("KEY_N\n");
 if(checkKey(KEY_M))printf("KEY_M\n");
 if(checkKey(KEY_COMMA))printf("KEY_COMMA\n");
 if(checkKey(KEY_PERIOD))printf("KEY_PERIOD\n");
 if(checkKey(KEY_SLASH))printf("KEY_SLASH\n");
 if(checkKey(KEY_RIGHTSHIFT))printf("KEY_RIGHTSHIFT\n");
 if(checkKey(KEY_UPARROW))printf("KEY_UPARROW\n");
 if(checkKey(KEY_PAD1))printf("KEY_PAD1\n");
 if(checkKey(KEY_PAD2))printf("KEY_PAD2\n");
 if(checkKey(KEY_PAD3))printf("KEY_PAD3\n");
 if(checkKey(KEY_PADENTER))printf("KEY_PADENTER\n");

 if(checkKey(KEY_LEFTCTRL))printf("KEY_LEFTCTRL\n");
 if(checkKey(KEY_LEFTWINDOW))printf("KEY_LEFTWINDOW\n");
 if(checkKey(KEY_LEFTALT))printf("KEY_LEFTALT\n");
 if(checkKey(KEY_SPACE))printf("KEY_SPACE\n");
 if(checkKey(KEY_RIGHTALT))printf("KEY_RIGHTALT\n");
 if(checkKey(KEY_RIGHTWINDOW))printf("KEY_RIGHTWINDOW\n");
 if(checkKey(KEY_KEYBOARDMOUSE))printf("KEY_KEYBOARDMOUSE\n");
 if(checkKey(KEY_RIGHTCTRL))printf("KEY_RIGHTCTRL\n");
 if(checkKey(KEY_LEFTARROW))printf("KEY_LEFTARROW\n");
 if(checkKey(KEY_DOWNARROW))printf("KEY_DOWNARROW\n");
 if(checkKey(KEY_RIGHTARROW))printf("KEY_RIGHTARROW\n");
 if(checkKey(KEY_PAD0))printf("KEY_PAD0\n");
 if(checkKey(KEY_PADPERIOD))printf("KEY_PADPERIOD\n");
 }

void codeProcess(char *printType)
 {
 char code;
 code=checkCode(getCodeBuffer());
 switch(code)
  {
  case CODE_NONE:break;
  case CODE_SCAN:(*printType)=SCANCODES;break;
  case CODE_NAME:(*printType)=THENAMES;break;
  case CODE_SPACESINGLE:toggleSingleHitKey(KEY_SPACE);break;
  }
 }

