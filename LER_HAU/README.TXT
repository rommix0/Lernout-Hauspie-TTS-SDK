Lernout & Hauspie Speech Products N.V.
======================================
TTS Division
7 Sint-Krispijnstraat
B-8900 Ieper, Belgium
Phone: +32 57 22 8888  Fax: +32 57 20 8489


Getting Started
===============

Product Name:	 SDK TTS Eng.USA 2 C2 V4.00 for Windows 95
Product Number:	 A90323
Version:   	 4.00
Date:            10 May 1996



Deliverables
============

Diskette 1
	
Disk labeled Setup, including files:

Files for Setup:
 DISK1           File to indicate Diskette 1
 SETUP.EXE       Setup Program
 SETUP.INF       Setup Information File
 SETUP.INI       Setup Information File
 _ISDEL.EXE      File for Setup
 _SETUP.DLL      File for Setup
 _INST32I.EX_    File for Setup
 SETUP.DBG       File for Setup
 ISDBGN.DLL      File for Setup
 SETUP.INS       File for Setup 
 README.WRI      Readme file for WordPad
 README.TXT      Readme text file

 \TARGET\BIN
 LHSTTS.HL_      TTS Evaluator Compressed Help Text
 ENG_USA2.HL_    TTS Evaluator Compressed Help Text for 
                 American English
 ENGLISH.TXT     English Sample Text
 SAMPLE.DCT      English Sample Dictionary File
 LHSTTS32.INI    TTS Evaluator INI file
 SDK_AE40.WRI    Release Note File


 \TARGET\INCLUDE
 TTSSDK32.H      Include file for TTS32 SDK API

 \TARGET\LIB
 TTSMGR32.LIB    Import library of TTS Manager DLL for Visual C++
 TTSMGRBR.LIB    Import library of TTS Manager DLL for Borland C++

 \TARGET\REDIST
 DISTAE40.TXT    L&H TTS32 SDK distribution information file

 \WIN\LHSP\LANGUAGE
 ENG_U232.DLL    Text-to-Speech 32-bit DLL for American English


Diskette 2

Disk labeled Executables, including files:

 DISK2           File to indicate Diskette 2

 \TARGET\BIN
 LHSTTS32.EXE    TTS Evaluator 4.00 Executable

 \WIN\LHSP\
 TTSDCT32.DLL    TTS User Dictionary Engine DLL
 TTSMGR32.DLL    TTS Manager DLL for SDK

 \WIN\LHSP\DATAFILE
 ENG_USA2.C21    Part of the American English data file

 \WIN\LHSP\G2P
 ENG_UL32.DLL    American English Language Engine


Diskette 3

Disk labeled Data File, including files:

 DISK3           File to indicate Diskette 3

 \WIN\LHSP\DATAFILE
 ENG_USA2.C20    Part of the American English data file


Diskette 4
	
Disk labeled Sample Programs, including files:
	
 DISK4           File to indicate Diskette 4
	
 \TARGET\SAMPLES\ENGPRG01
 LHSBLUE.ICO     ICON file
 PROGRAM1.C      Source file
 PROGRAM1.DEF    Definition file
 PROGRAM1.EXE    Executable file
 PROGRAM1.H      Include file
 PROGRAM1.MAK    Makefile for Visual C++
 PROGRAM1.RC     Resource file

 \TARGET\SAMPLES\ENGPRG02
 LHSBLUE.ICO     ICON file
 PROGRAM2.C      Source file
 PROGRAM2.DEF    Definition file
 PROGRAM2.EXE    Executable file
 PROGRAM2.H      Include file
 PROGRAM2.MAK    Makefile for Visual C++
 PROGRAM2.RC     Resource file

 \TARGET\SAMPLES\ENGPRG03
 LHSBLUE.ICO     ICON file
 PROGRAM3.C      Source file
 PROGRAM3.DEF    Definition file
 PROGRAM3.EXE    Executable file
 PROGRAM3.H      Include file
 PROGRAM3.MAK    Makefile for Visual C++
 PROGRAM3.RC     Resource file

 \TARGET\SAMPLES\ENGPRG04
 LHSBLUE.ICO     ICON file
 PROGRAM4.C      Source file
 PROGRAM4.DEF    Definition file
 PROGRAM4.EXE    Executable file
 PROGRAM4.H      Include file
 PROGRAM4.MAK    Makefile for Visual C++
 PROGRAM4.RC     Resource file
 SAMPLE.TXT      Sample text file

 \TARGET\SAMPLES\ENGPRG05
 LHSBLUE.ICO     ICON file
 PROGRAM5.C      Source file
 PROGRAM5.DEF    Definition file
 PROGRAM5.EXE    Executable file
 PROGRAM5.H      Include file
 PROGRAM5.MAK    Makefile for Visual C++
 PROGRAM5.RC     Resource file

 \TARGET\SAMPLES\ENGPRG06
 LHSBLUE.ICO     ICON file
 PROGRAM6.C      Source file
 PROGRAM6.DEF    Definition file
 PROGRAM6.EXE    Executable file
 PROGRAM6.H      Include file
 PROGRAM6.MAK    Makefile for Visual C++
 PROGRAM6.RC     Resource file

 \TARGET\SAMPLES\ENGPRG07
 LHSBLUE.ICO     ICON file
 PROGRAM7.C      Source file
 PROGRAM7.DEF    Definition file
 PROGRAM7.EXE    Executable file
 PROGRAM7.H      Include file
 PROGRAM7.MAK    Makefile for Visual C++
 PROGRAM7.RC     Resource file

 \TARGET\SAMPLES\ENGPRG08
 LHSBLUE.ICO     ICON file
 PROGRAM8.C      Source file
 PROGRAM8.DEF    Definition file
 PROGRAM8.EXE    Executable file
 PROGRAM8.H      Include file
 PROGRAM8.MAK    Makefile for Visual C++
 PROGRAM8.RC     Resource file
 SAMPLE.DCB      Binary dictionary file

 \TARGET\SAMPLES\ENGPRG09
 LHSBLUE.ICO     ICON file
 PROGRAM9.C      Source file
 PROGRAM9.DEF    Definition file
 PROGRAM9.EXE    Executable file
 PROGRAM9.H      Include file
 PROGRAM9.MAK    Makefile for Visual C++
 PROGRAM9.RC     Resource file
 SAMPLE.DCB      Binary dictionary file



System Requirements
===================

 - PC with 80386DX (33 MHz) or higher
 - Microsoft Windows NT 3.51 or Windows 95
 - About 2 M bytes of available RAM for the TTS system
 - A Windows compatible sound board for speech output, 
   preferably 16-bit	



Installation from Disk
=======================

 In order to install the L&H TTS SDK on your computer, 
 proceed as follows:
	
 1. Start Microsoft Windows 95 or Windows NT
 2. Insert disk 1 ('Setup') in drive a:
 3. Windows NT : 
    From the File Menu in either the Program Manager 
    or the File Manager, choose 'Run'
    Windows 95 : 
    From the Start button in the Taskbar, choose 'Run'
 4. Type 'a:\setup' and press ENTER.
 5. Follow the Setup instructions on the screen.



Changes & Bug Fixes (Compared To TTS SDK V3.xx)
===============================================

 - This version supports multi-threading and it also fully 
   supports multiple instances and multi-tasking. 
   (Only compared to TTS SDK V3.00 Beta version.)
 - The bug generating PCM data buffer (during the function 
   TtsGenNextPcmBuf) has been fixed. 
   (Only compared to TTS SDK V3.00 Beta version.)	
 - The parameter type for the functions TtsSpeakText, 
   TtsGenPcmBuf, and TtsGenPcmFile has been changed from 
   LPSTR to LPVOID. 
   (Only compared to TTS SDK V3.00 Beta version.)	
 - The restriction on the header format of the user dictionary 
   has been removed.
 - Five API functions have been added as follows:
	            - TtsPauseSpeechEx
	            - TtsStopSpeechEx
	            - TtsResumeSpeechEx
	            - TtsSetMrkMode
	            - TtsGetMrkMode
   See details in the API reference guide.
 - The setup program registers all installed files to the system 
   registry data base and all files which are generated or are 
   copied into the Windows directory or Windows System directory 
   will be removed automatically. If you have installed the TTS 
   SDK V3.xx, the setup program of V4.xx will remove the 
   following files during installation:
		Windows Directory:
			- LHTTSEVA.INI
			- LHTTSSDK.INI
		Windows\System Directory
			- TTSMGR32.DLL
			- TTSEVA32.DLL
 - The setup program creates a sub-directory (LHSP) in the 
   Windows directory and copies the language-independent system 
   file into this directory.
 - The setup program creates a file, named "SETUP.LOG" containing 
   information about the files installed on your system.
 - The document ROADMAP.DOC has been added to this release for 
   developers who are using L&H TTS SDK V1.xx or V2.xx.



Limitations and Features not implemented
========================================

 - The directory <WINDOWS dir>\LHSP must be added to the PATH in 
   the AUTOEXEC.BAT (for Windows 95), or in the system environment
   (for Windows NT). Or this path must be added to the application  
   specific path (ONLY valid in Windows 95). 
 - When uninstalling the TTS SDK some files may be left in the 
   installation directory. Manually delete these files from your 
   harddisk. 
 - This version must be installed on Windows NT 3.51 or Windows 95.
 - A PC with 386DX 33 is a minimal configuration.  Speech output 
   can be choppy on such a platform, especially at sampling frequencies 
   different from 10 kHz and in combination with certain types of 
   sound boards.
 - All makefiles for sample programs are made by Visual C++ V2.00.
 - Since the makefile of program 9 is made by Visual C++ V2., you 
   should change the makefile when you use Visual C++ V4.xx. To use 
   the makefile with Visual C++  V4, you should delete file, 
   "\MSVC20\LIB\WINMM.LIB" and insert the file "\MSDEV\LIB\WINMM.LIB".
 - To use makefiles for sample programs you must specify the 
   directory <target directory>\INCLUDE in your compiler environment.
 - If you have no German engine in your system, then you can not 
   execute sub-menu "German" under the "Language" menu in program 6.
 - When you use the embedded command <ESC>H<NUMBER>, you may NOT 
   specify the number with commas. So you should use <esc>H100000 
   instead of <esc>H100,000.
 - The default value for the wait period between messages (<ESC>W) 
   is set to 2 instead of 4. The latter value is erroneously 
   documented as the default value.
 - Manually added sentence accents overrule default accentuation 
   rules. These accents are either added to orthographic input 
   using <ESC>", or in phonetic input by using " instead of '.
   <ESC>/ DEr ar "tu "æk-sEnts ?In DI 'sEn-t$ns <ESC>/ 
   To <ESC>"be or <ESC>"not to be, <ESC>"that is the question.
   In a message, only those words preceded by <ESC>" (or " in 
   phonetic input) will get a sentence accent. 
   As a consequence, a message containing only one manual accent 
   will have an almost flat intonation on all the other words.
      E.g.:
           To be or not to be, <ESC>" that is the question.
           To be or not to be, <ESC>/ "Dæt Iz <ESC>/ the question.
 - If homographs (i.e. words that have more than one pronunciation)
   can only be distinguished on the basis of their meaning (and not 
   on part-of-speech), then they only receive one pronunciation in 
   the current version. Thus, words like "bass" or "lead" will 
   always be pronounced the same way, regardless of the context. 
   Also, using <ESC>@<part-of-speech> will not influence the 
   pronunciation of these words. 
 - Character strings consisting of letters followed by digits are 
   always spelled. 
      E.g.:
           DX2  sample1



Distributing applications using L&H TTS32 SDK
=============================================

 - The file including the information to re-distribute L&H TTS32 
   SDK is copied to the directory <installation directory>\REDIST.
   The filename is  "DISTAE40.TXT".
 - For more details, refer to the <General Introduction to the 
   L&H TTS32 SDK>.


© 1996 L&H. All rights reserved.