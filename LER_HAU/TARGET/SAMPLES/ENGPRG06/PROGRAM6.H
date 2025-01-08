/*
 *      Program6.H
 *
 *      Product : Text-To-Speech Software Development Kit
 *      Version : 4.00
 *      Purpose : Header file for TTS Sample Program6.C
 *      Comment : Do not change the contents of this file!
 *
 *      Copyright (c) 1996, Lernout & Hauspie Speech Products.
 *      All rights reserved.
 *
 */

#include <windows.h>
#include <string.h>
#define IDM_FILE                       1000
#define IDM_F_EXIT                     1200
#define ENG                            2050
#define GER                            2100
#define FRE                            2150
#define SPA                            2200
#define ITA                            2250
#define IDM_T_SPEAK                    3000

#define IDM_HELP                       4000
#define IDM_H_ABOUT                    4050


#define ID_COUNTRY                     1000
#define ID_CODING                      1001
#define ID_NUMBER                      1002
#define ID_VERSION                     1003
#define ID_SAMPLE                      1004
#define ID_LANGUAGE                    1005
#define IDC_STATIC                       -1

#define ID_COMPONENT                   1007
#define ID_MANUFACTURER                1009
#define ID_COPYRIGHT                   1010
#define ID_PRODUCTVERSION              1011


#define IDS_ERR_REGISTER_CLASS            1
#define IDS_ERR_CREATE_WINDOW             2


LONG FAR PASCAL WndProc(HWND, WORD, WORD, LONG);
BOOL FAR PASCAL About(HWND, WORD, WORD, LONG);
int nCwRegisterClasses(void);
void CwUnRegisterClasses(void);
