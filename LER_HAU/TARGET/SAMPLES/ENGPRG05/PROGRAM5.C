/*
Product  :  Text-To-Speech Software Development Kit
Version  :  4.00
Program  :  PROGRAM5.c
Purpose  :  This program demonstrates how to change pitch, rate, volume and
            speaker. Select pitch, rate, speaker, volume or the random
            option from the menu. Depending on your input, the program will
            change the appropriate settings from minimum up to the maximum,
            or generate random combinations, and then synthesize a sentence
            with these settings.
Comments :  Comments to the source code are given as they pertain to
            the usage of TTS.

Copyright (c) 1996, Lernout & Hauspie Speech Products.
All rights reserved.
*/

#include "PROGRAM5.h"
#include <stdlib.h>     /* needed for the random function */
#include <time.h>       /* needed for the random function */
#include <stdio.h>      /* needed for the string formatting functions */
#include "ttssdk32.h"	/* Include this header so all TTS routines are known. */

#define FREQUENCY     M_FREQ_11KHZ       /* Use sampling rate of 11 kHz. */

HTTSINSTANCE    hTtsInst;
TTSSYSTEMINFO   gSysInfo;
int             nTaskNo;
int             nEngineId;
char            gszString[128];
char            gszAppName[20];
HINSTANCE       ghInst;
HWND            ghWndMain;

/* 
 *  routine to create a random figure between minimum and maximum: 
 */
short nRandom(int min, int max)
{ 
    double d,d2,d3,d4;
    short n;

    d = rand() ;
    d2 = d * (max-min);
    d3 = d2 / RAND_MAX;
    d4 = d3 + min;
    n = (short) d4;
    return n;
}


/*
 *  Windows Main function
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    MSG     msg;
        /* function to start random generator: */
    srand((unsigned)time( NULL ));
    strcpy(gszAppName, "PROGRAM5");
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
                     gszAppName,"L&H TTS Sample Program 5",WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_CLIPCHILDREN|WS_OVERLAPPED,
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
 *  Windows Message Handling Function
 */
LONG FAR PASCAL WndProc(HWND hWnd, WORD Message, WORD wParam, LONG lParam)
{
    int     nRc=0, i;
    char    szTextStr[200];

    switch (Message)
    {
        case WM_COMMAND:
            switch (wParam)
            {
                case IDM_F_EXIT:
                    PostMessage(hWnd,WM_CLOSE,0,0);
                    break;
                case IDM_T_SPEAK_PITCH:
                {
                    short nPitch;
                    char szText[50];
                    for(nPitch=1; nPitch<10; nPitch +=2)
                    {
                            /* Set pitch with random number	*/
                        TtsSetSpeechMode(hTtsInst, TRUE,-1, -1, -1, nPitch,-1,-1);
                        sprintf(szText,"This is pitch %i.",nPitch);
                            /* Play a text */
                        nRc = TtsSpeakText(hTtsInst, 1, szText, strlen(szText),
                                           M_ANSI_TEXT, &nTaskNo);
                        if(nRc!= R_SUCCESSFUL && nRc != E_TTSINSTBUSY)
                        {
                            MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK);
                            PostMessage(hWnd,WM_CLOSE,0,0);
                        }
                        if(nRc == E_TTSINSTBUSY)
                            MessageBox(NULL,"TTS instance is busy!", NULL, MB_OK);
                    }
                        /* Set speech parameter to default */
                    TtsSetSpeechMode(hTtsInst, TRUE, -1, -1, -1, -1, -1, -1);
                }
                break;
                
                case IDM_T_SPEAK_RATE:
                {
                    short nRate;
                    char szText[50];
                    for(nRate=1; nRate<10; nRate +=2 )
                    {
                            /* Set speech rate with random number */
                        TtsSetSpeechMode(hTtsInst, TRUE, -1, -1, nRate, -1 ,-1, -1);
                        sprintf(szText,"This is rate %i.",nRate);
                            /* Play a text */
                        nRc = TtsSpeakText(hTtsInst, 1, szText, strlen(szText),
                                           M_ANSI_TEXT, &nTaskNo);
                        if(nRc!= R_SUCCESSFUL && nRc != E_TTSINSTBUSY)
                        {
                            MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK);
                            PostMessage(hWnd,WM_CLOSE,0,0);
                        }
                        if(nRc == E_TTSINSTBUSY)
                            MessageBox(NULL,"TTS instance is busy!", NULL, MB_OK);
                    }
                        /* Set speech parameter to the default */
                    TtsSetSpeechMode(hTtsInst, TRUE, -1, -1, -1, -1, -1, -1);
                }
                break;

                case IDM_T_SPEAK_SPEAKER:
                {
                    short nSpeaker;
                    char szText[50];
                    for(nSpeaker=1; nSpeaker<10; nSpeaker +=2)
                    {
                        /* Set speaker with random number */
                        TtsSetSpeechMode(hTtsInst, TRUE, -1, nSpeaker, -1, -1, -1, -1);
                        sprintf(szText,"This is speaker %i.",nSpeaker);
                            /* Play a text   */
                        nRc = TtsSpeakText(hTtsInst, 1, szText, strlen(szText),
                                           M_ANSI_TEXT, &nTaskNo);
                        if(nRc!= R_SUCCESSFUL && nRc != E_TTSINSTBUSY)
                        {
                            MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK);
                            PostMessage(hWnd,WM_CLOSE,0,0);
                        }
                        if(nRc == E_TTSINSTBUSY)
                            MessageBox(NULL,"TTS instance is busy!", NULL, MB_OK);
                    } 
                        /* Set speech parameter to the default */
                    TtsSetSpeechMode(hTtsInst, TRUE, -1, -1, -1, -1, -1, -1);
                }
                break;

                case IDM_T_SPEAK_VOLUME:
                {
                    short nVolume;
                    char szText[50];
                    for(nVolume=1; nVolume<10; nVolume +=2)
                    {
                            /* Set volume with random number */
                        TtsSetSpeechMode(hTtsInst, TRUE, nVolume, -1, -1, -1, -1, -1);
                        sprintf(szText,"This is volume %i.",nVolume);
                            /* Play a text   */
                        nRc = TtsSpeakText(hTtsInst, 1, szText, strlen(szText),
                                           M_ANSI_TEXT, &nTaskNo);
                        if(nRc!= R_SUCCESSFUL && nRc != E_TTSINSTBUSY)
                        {
                            MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK);
                            PostMessage(hWnd,WM_CLOSE,0,0);
                        }
                        if(nRc == E_TTSINSTBUSY)
                            MessageBox(NULL,"TTS instance is busy!", NULL, MB_OK);
                    }
                        /* Set speech parameter to the default */
                    TtsSetSpeechMode(hTtsInst, TRUE, -1, -1, -1, -1, -1, -1);
                }
                break;


                case IDM_T_SPEAK_RANDOM:
                {
                    short nCount,nSpeaker,nRate,nPitch;
                    char szText[50];

                    for(nCount=1; nCount<5; nCount++)
                    {

                     /*	Define a random value between one and nine, both included.
                        The program will choose a random value for speaker, rate and
                        pitch. The random function itself is of no interest here. */

                        nSpeaker = nRandom(1,9); /* generate a random speaker */
                        nRate    = nRandom(1,9); /* generate a random rate */
                        nPitch   = nRandom(1,9); /* generate a random pitch */
                        TtsSetSpeechMode(hTtsInst, TRUE, -1, nSpeaker, nRate, nPitch, -1, -1);

                     /*  Generate a sentence specifying all random generated
                         parameter values, and play it back. */

                        sprintf(szText,"This is speaker %i, rate %i, and pitch %i.",nSpeaker,nRate,nPitch);
                            /* Play a text */
                        nRc = TtsSpeakText(hTtsInst, 1, szText, strlen(szText),
                                           M_ANSI_TEXT, &nTaskNo);
                        if(nRc!= R_SUCCESSFUL && nRc != E_TTSINSTBUSY)
                        {
                            MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK);
                            PostMessage(hWnd,WM_CLOSE,0,0);
                        }
                        if(nRc == E_TTSINSTBUSY)
                            MessageBox(NULL,"TTS instance is busy!", NULL, MB_OK);
                    }
                        /* Set speech parameter to the default */                       
                    TtsSetSpeechMode(hTtsInst, TRUE, -1, -1, -1, -1, -1, -1);
                }
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
                /* Initialize the TTS instance */
            nRc  = TtsInitialize(&hTtsInst, NULL, &gSysInfo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Initialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
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
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
                break;
            }
                /* Set frequency to 11.025 kHz */
            TtsSetFreq(hTtsInst, FREQUENCY);
                /* Playing an introductory text */
            strcpy(szTextStr, "LHS. This is sample program 5.");
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Speak TTS text failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0); /* This message also stops the TTS system. */
            }
            break;      /* End of WM_CREATE */

        case WM_CLOSE:  /* Close the window; Uninitialize the TTS instance */
                /* Set speech parameter to the default */
            TtsSetSpeechMode(hTtsInst, TRUE,-1, -1, -1, -1,-1,-1);
            strcpy(szTextStr, "Thank you for using the L&H TTS system!");
            nRc = TtsSpeakText(hTtsInst, 1, szTextStr, strlen(szTextStr),
                               M_ANSI_TEXT, &nTaskNo);
                /* Uninitialize the TTS instance */
            nRc = TtsUninitialize(hTtsInst);
            if (nRc!= R_SUCCESSFUL)
            {
                MessageBox(NULL,"Uninitialize TTS instance failed!",NULL,MB_OK); /* clarify error */
                PostMessage(hWnd,WM_CLOSE,0,0);
            }

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
 *  Dialog function to display TTS product information
 */
BOOL FAR PASCAL About(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:
        {
            char            szDummy[20];  
            TTSENGINEINFO   mEngine;
            TTSIDENTITY     mIden;

                /* Get TTS engine information */          
            TtsGetEngineInfo(hTtsInst, nEngineId, &mEngine);
            SendDlgItemMessage(hWndDlg, ID_LANGUAGE, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szLanguage);
            SendDlgItemMessage(hWndDlg, ID_COUNTRY, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szLocation);
            SendDlgItemMessage(hWndDlg, ID_CODING, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szCodingScheme);
            SendDlgItemMessage(hWndDlg, ID_PRODUCTVERSION, WM_SETTEXT, 0, (LPARAM) (LPSTR) mEngine.szVersion);
            sprintf(szDummy,"%i",mEngine.nSpeaker);
            SendDlgItemMessage(hWndDlg, ID_NUMBER, WM_SETTEXT, 0, (LPARAM) (LPSTR) szDummy);
                /* Get TTS product information */
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
