/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM1.c
Purpose  :  Program1 demonstrates the initialization (by calling
            TtsInitialize), playback (by calling TtsSpeakText), and
            generation of a PCM data file (by calling TtsGenPcmFile) of a
            TTS program. The program plays back the hard coded sentence
            "LHS. This is sample program 1." and generates a wave file for
            it. 
Comments :   

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/

#include "PROGRAM1.h"
#include <stdio.h>
#include "TTSSDK32.H"   /* Include this header so all TTS routines are known. */

#define FREQUENCY   M_FREQ_11KHZ    /* Use sampling rate of 11 kHz. */

/*
 *  Global variables
 */						   
HTTSINSTANCE    hTtsInst;
TTSSYSTEMINFO   gSysInfo;
int             nTaskNo;
int             nEngineId;
char            gszString[128];
char            gszAppName[20];
HINSTANCE       ghInst;
HWND            ghWndMain;

/*
 *  Main windows function
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
 	MSG     msg;

 	strcpy(gszAppName, "PROGRAM1");
 	ghInst = hInstance;
 	if(!hPrevInstance)
   	{
     	WNDCLASS   wndclass;
     	memset(&wndclass, 0x00, sizeof(WNDCLASS));

     	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
     	wndclass.lpfnWndProc = (WNDPROC) WndProc;
     	wndclass.cbClsExtra = 0;
     	wndclass.cbWndExtra = 0;
     	wndclass.hInstance = ghInst;
     	wndclass.hIcon = LoadIcon(ghInst, "PROGRAMICON");
     	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
     	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
     	wndclass.lpszMenuName = gszAppName;
     	wndclass.lpszClassName = gszAppName;
     	if(!RegisterClass(&wndclass))
        {
            LoadString(ghInst, IDS_ERR_REGISTER_CLASS, gszString, sizeof(gszString));
            MessageBox(NULL, gszString, NULL, MB_ICONEXCLAMATION);
            return -1;
        }
    }

 	ghWndMain = CreateWindow(
                     gszAppName, "L&H TTS Sample Program 1",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_OVERLAPPED,
                     CW_USEDEFAULT,0,CW_USEDEFAULT,0,NULL,NULL,ghInst,NULL);

    if(ghWndMain == NULL)
    {
        LoadString(ghInst, IDS_ERR_CREATE_WINDOW, gszString, sizeof(gszString));
        MessageBox(NULL, gszString, NULL, MB_ICONEXCLAMATION);
        return IDS_ERR_CREATE_WINDOW;
    }

 	ShowWindow(ghWndMain, nCmdShow);
 	while(GetMessage(&msg, NULL, 0, 0))
   	{
        TranslateMessage(&msg);
        DispatchMessage(&msg);
   	}

    UnregisterClass(gszAppName, ghInst);
    return msg.wParam;
}


/*
 *  Message handling function
 */
LONG FAR PASCAL WndProc(HWND hWnd, WORD Message, WORD wParam, LONG lParam)
{
    int         nRc=0, i;
    HMENU       hMenu;
    FARPROC     lpProcGen;

    switch (Message)
    {
        case WM_COMMAND:
            switch (wParam)
            {
                case IDM_F_EXIT:
                    PostMessage(hWnd,WM_CLOSE,0,0);
                    break;
                case IDM_T_SPEAK:
                        /* Initialize the TTS instance */
                    nRc = TtsInitialize(&hTtsInst, NULL, &gSysInfo);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Initialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    for(i=0;i<gSysInfo.nMaxEngines;i++)
                    {
                        /* Find the "American English" TTS engine */
                        if(strcmp(gSysInfo.mTtsEg[i].szLanguage, "American English") == 0)
                        {
                            nEngineId = gSysInfo.mTtsEg[i].nEngineId;
                            break;	
                        }
                    }
                        /* Select the "American English" TTS engine	*/
                    nRc = TtsSelectEngine(hTtsInst, nEngineId, TRUE);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Select TTS engine failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                        /* Set frequency to 11.025 kHz */
                    TtsSetFreq(hTtsInst, FREQUENCY);
                    hMenu = GetMenu(hWnd);

                        /* Play pre-defined input text */
					
                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_GRAYED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_GRAYED);
                    nRc = TtsSpeakText(hTtsInst, 1, "LHS. This is sample program 1.", 30,M_ANSI_TEXT, &nTaskNo);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                        TtsUninitialize(hTtsInst);
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }

                        /* Generate a wave file */
                    lpProcGen = MakeProcInstance(WaitGenFile, ghInst);
                    DialogBox(ghInst, MAKEINTRESOURCE(3000), hWnd, lpProcGen);
                    FreeProcInstance(lpProcGen);

                        /* Uninitialize the TTS instance */
                    nRc = TtsUninitialize(hTtsInst);
                    if (nRc!= R_SUCCESSFUL)
                    {
                        MessageBox(NULL,"Uninitialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                        PostMessage(hWnd,WM_CLOSE,0,0);
                        break;
                    }
                    EnableMenuItem(hMenu, IDM_F_EXIT, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_T_SPEAK, MF_ENABLED);
                    EnableMenuItem(hMenu, IDM_H_ABOUT, MF_ENABLED);
                    break;
            	
                case IDM_H_ABOUT:
                {
                    FARPROC lpfnDIALOGMsgProc;

                    lpfnDIALOGMsgProc = MakeProcInstance((FARPROC)About, ghInst);
                    nRc = DialogBox(ghInst, MAKEINTRESOURCE(100), hWnd, lpfnDIALOGMsgProc);
                    FreeProcInstance(lpfnDIALOGMsgProc);
                }
                break;

            default:
                return DefWindowProc(hWnd, Message, wParam, lParam);
            }
            break;

        case WM_CREATE:
            TtsGetSysInfo(hTtsInst, &gSysInfo );
            break;       /* End of WM_CREATE */
        case WM_CLOSE:
            DestroyWindow(hWnd);
            if (hWnd == ghWndMain)
                PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0L;
}


/*
 *  A dialog function to display TTS product information
 */
BOOL FAR PASCAL About(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:
        {
            char            szDummy[20];  
            int             nRc, i;
            TTSENGINEINFO   mEngine;
            TTSIDENTITY     mIden;

                /* Initialize the TTS instance	*/
            nRc  = TtsInitialize(&hTtsInst, GetCurrentThread(), &gSysInfo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Initialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWndDlg,WM_CLOSE,0,0);
                break;
            }

            for(i=0;i<gSysInfo.nMaxEngines;i++)
            {
                /* Find the "American English" TTS engine	*/
                if(strcmp(gSysInfo.mTtsEg[i].szLanguage, "American English") == 0)
                {
                    nEngineId = gSysInfo.mTtsEg[i].nEngineId;
                    break;	
                }	
            }	
                /* Get TTS engine information	*/			
            nRc = TtsGetEngineInfo(hTtsInst, nEngineId, &mEngine);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Get TTS engine information failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWndDlg,WM_CLOSE,0,0);
                break;
            }

                /* Uninitialize the TTS instance	*/
            nRc = TtsUninitialize(hTtsInst);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Uninitialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWndDlg,WM_CLOSE,0,0);
                break;
            }

            SendDlgItemMessage(hWndDlg, ID_LANGUAGE, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szLanguage);
            SendDlgItemMessage(hWndDlg, ID_COUNTRY, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szLocation);
            SendDlgItemMessage(hWndDlg, ID_CODING, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szCodingScheme);
            SendDlgItemMessage(hWndDlg, ID_PRODUCTVERSION, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szVersion);
            sprintf(szDummy,"%i",mEngine.nSpeaker);
            SendDlgItemMessage(hWndDlg, ID_NUMBER, WM_SETTEXT, 0, (LPARAM) (LPSTR) szDummy);
                /* Get TTS product information	*/
            TtsGetIdentity(&mIden);
            SendDlgItemMessage(hWndDlg, ID_COPYRIGHT, WM_SETTEXT, 0, (LPARAM) (LPSTR) mIden.szCopyright);
            SendDlgItemMessage(hWndDlg, ID_MANUFACTURER, WM_SETTEXT, 0, (LPARAM) (LPSTR) mIden.szManufacturer);
        }
        break;
    	
        case WM_CLOSE:
            PostMessage(hWndDlg, WM_COMMAND, IDCANCEL, 0L);
            break;

        case WM_COMMAND:
            switch(wParam)
            {
                case IDOK:
                    EndDialog(hWndDlg, TRUE);
                    break;
                case IDCANCEL:
                    EndDialog(hWndDlg, FALSE);
                    break;
            }
            break;

        default:
            return FALSE;
    }
    return TRUE;
}


/*
 *  A dialog function to generate a PCM data file
 */
BOOL FAR PASCAL WaitGenFile(HWND hDlg, unsigned message, WORD wParam, LONG lParam)
{
    char    szTextStr[200];
    int     nRc;

    switch (message)
    {
        case WM_INITDIALOG:
            PostMessage(hDlg, WM_GEN_SAMPLE, (WORD)hDlg, 0);
            break;
        case WM_GEN_SAMPLE:
            strcpy(szTextStr, "LHS. This is sample program 1.");
            nRc = TtsGenPcmFile(hTtsInst, szTextStr, strlen(szTextStr), 
                                "tts.wav", M_OUT_16WAVE,
                                M_ANSI_TEXT, M_OUT_STANDARD, &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Generate wave file failed!",NULL,MB_OK); /* clarify error */
                TtsUninitialize(hTtsInst);
                EndDialog(hDlg, TRUE);
                break;
            }
            EndDialog(hDlg, TRUE);
            break;
    }
    return (FALSE);
}
