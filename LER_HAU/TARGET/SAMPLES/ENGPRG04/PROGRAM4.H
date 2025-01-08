/*
 *      Program4.H
 *
 *      Product : Text-To-Speech Software Development Kit
 *      Version : 4.00
 *      Purpose : Header file for TTS Sample Program4.C
 *      Comment : Do not change the contents of this file!
 *
 *      Copyright (c) 1996, Lernout & Hauspie Speech Products.
 *      All rights reserved.
 *
 */

#include <windows.h>
#include <string.h>
#define IDM_FILE                        1000
#define IDM_F_FILE                     	1100
#define IDM_F_EXIT                     	1200
#define IDM_T_SPEAK                    	1300
#define IDM_T_GENERATE                 	1400
#define IDM_HELP                        2000
#define IDM_H_ABOUT                    	2050

#define ID_COUNTRY                     	1000
#define ID_CODING                      	1001
#define ID_NUMBER                      	1002
#define ID_VERSION                     	1003
#define ID_SAMPLE                      	1004
#define ID_LANGUAGE                    	1005
#define IDC_STATIC                        -1

#define ID_COMPONENT                    1007
#define ID_MANUFACTURER                 1009
#define ID_COPYRIGHT                    1010
#define ID_PRODUCTVERSION               1011


#define IDS_ERR_REGISTER_CLASS             1
#define IDS_ERR_CREATE_WINDOW              2

#define	IDD_WAIT_GEN_BOX                3000     
#define	WM_GEN_SAMPLE                   7000     


LONG FAR PASCAL WndProc(HWND, WORD, WORD, LONG);
BOOL FAR PASCAL About(HWND, WORD, WORD, LONG);
int nCwRegisterClasses(void);
void CwUnRegisterClasses(void);
BOOL FAR PASCAL WaitGenFile(HWND hDlg, unsigned message, WORD wParam, LONG lParam);
