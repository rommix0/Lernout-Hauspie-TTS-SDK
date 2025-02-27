/*
 *      Program7.H
 *
 *      Product : Text-To-Speech Software Development Kit
 *      Version : 4.00
 *      Purpose : Header file for TTS Sample Program7.C
 *      Comment : Do not change the contents of this file!
 *
 *      Copyright (c) 1996, Lernout & Hauspie Speech Products.
 *      All rights reserved.
 *
 */

#include <windows.h>
#include <string.h>

#define ID_PLAY                  100
#define ID_PAUSE                 101
#define ID_STOP                  102
#define ID_RESUME                103
#define ID_PLAYTEXT              104


#define IDM_FILE                1000
#define IDM_F_EXIT              1200
#define IDM_TTS                 2000
#define IDM_TTS_PLAY            3000
#define IDM_HELP                4000
#define IDM_H_ABOUT             4050


#define ID_COUNTRY              1000
#define ID_CODING               1001
#define ID_NUMBER               1002
#define ID_VERSION              1003
#define ID_SAMPLE               1004
#define ID_LANGUAGE             1005

#define ID_COMPONENT            1007
#define ID_PRODUCT              1008
#define ID_MANUFACTURER         1009
#define ID_COPYRIGHT            1010
#define ID_PRODUCTVERSION       1011


#define IDS_ERR_REGISTER_CLASS     1
#define IDS_ERR_CREATE_WINDOW      2

#define IDC_STATIC                -1



LONG FAR PASCAL WndProc(HWND, WORD, WORD, LONG);
BOOL FAR PASCAL About(HWND, WORD, WORD, LONG);
int nCwRegisterClasses(void);
void CwUnRegisterClasses(void);
