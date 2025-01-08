/*
 *      TTSSDK32.H
 *
 *      Product : Text-To-Speech Software Development Kit
 *      Version : 4.00
 *      Date    : March. 31 1996
 *      Purpose : This include file contains structure definition,
 *                type definition and the error code definition. It
 *                also lists all API functions available in the 
 *				  L&H TTS32 SDK.
 *      Comment : Do not change the contents of this file!
 *
 *      Copyright (c) 1996, Lernout & Hauspie Speech Products.
 *      All rights reserved.
 *
 */


#ifndef __TTSSDK32_H__
#define __TTSSDK32_H__

#include "windows.h"
/*------------------------------------------------------------------------
							Data Structures
 -----------------------------------------------------------------------*/
typedef int	HUSRDICT;				/* Type of User dictionary			*/
typedef int	HTTSINSTANCE;			/* Type of TTS instance				*/
typedef HTTSINSTANCE *LPHTTSINSTANCE;/* Points to type of TTS instance	*/
typedef HUSRDICT *LPHUSRDICT;		/* Points to handle of dictionary	*/

#define	M_MAX_TTSENGINE			25	/* Maximum available number of TTS 	*/
									/* engine							*/

/*
 *	Available frequency information block
 */
typedef struct
{
    BOOL  	    	bFreq8;         /* 8 kHz Sampling       			*/
    BOOL            bFreq10;        /* 10 kHz Sampling      			*/
    BOOL            bFreq11;        /* 11.025 kHz Sampling  			*/
    BOOL            bReserved1;     /* For future use       			*/
    BOOL            bReserved2;     /* For future use       			*/
} TTSFREQTYPE, *LPTTSFREQTYPE;


/*
 *	TTS Engine information block
 */
typedef struct
{
	int 			nEngineId;		/* engine id						*/
    char			szEngine[40];	/* engine name						*/
	char			szLanguage[40];	/* language name					*/
	char			szVersion[10];	/* version number					*/
} TTSENGINELIST, *LPTTSENGINELIST;

/*
 *	Global TTS system information block
 */
typedef struct
{
	char    		szProduct[40];	/* Name of product					*/	
	char    		szVersion[10];  /* product Version          		*/
	TTSFREQTYPE   	FreqType;      	/* Available TTS sampling frequency	*/
	int				nMaxTtsInst;	/* Max number of availbale TTS 		*/
									/* instance for applications		*/
	int				nUsedTtsInst;	/* Number of used TTS instances		*/
	int				nMaxEngines;	/* Number of availbale engines		*/
	int				nLoadedEngines;	/* Number of loaded engines			*/
	int				nLoadedUsrDict;	/* Number of loaded dictionary		*/
									/* simple TTS engine info			*/
	TTSENGINELIST	mTtsEg[M_MAX_TTSENGINE];	
} TTSSYSTEMINFO, *LPTTSSYSTEMINFO;

/*
 *	TTS system product information block
 */
typedef struct
{
	char   		szManufacturer[40];	/* Name of the manufacturer			*/
    char   		szProduct[40];		/* Name of the product				*/
    char   		szComponent[40];	/* The component name				*/
    char   		szCopyright[40];    /* The copyright					*/
    char   		szVersion[10];      /* DLL Version              		*/
} TTSIDENTITY, *LPTTSIDENTITY;

/*
 *	Engine information block
 */
typedef struct
{
	char    	szProduct[40];		/* Name of product					*/	
	char    	szVersion[10];  	/* product Version          		*/
	int			nEngineId;			/* Engine id.						*/
	char		szLanguage[40];		/* Name of language					*/
	char		szDialect[40];		/* Type of dialect					*/
	char		szLocation[40];		/* Location of language				*/
	int     	nSpeaker;       	/* Speaker identification           */	
	int     	nGender;        	/* 1:Male 2:Female 3:Neutral        */	
	char		szKeyWord[20];		/* Mostly used for name of speaker  */
	char		szCodingScheme[20];	/* Available coding method			*/
} TTSENGINEINFO, *LPTTSENGINEINFO;


/*
 *	Speech mode information block
 */
typedef struct
{
	int				nVolume;		/* Volume			       			*/
    int             nPitch;        	/* Pitch   							*/
    int             nRate;        	/* Speech rate						*/
    int             nSpeaker;     	/* Speaker							*/
	int             nPauseDur;     	/* Pause duration between messages	*/
    int             nReadMode;     	/* Read mode						*/
} TTSSPEECHMODE, *LPTTSSPEECHMODE;

/*
 *	User Dictionary information block
 */
typedef struct
{
	char	szLogicalName[40];	/* Logical name of a dictionary			*/
	char	szLanguage[40];		/* Language Name						*/
	char	szAlgorithm[40];	/* Name of used Algorithm 				*/
	char	szDataType[16];		/* Type of input text character 				*/
	char	szDescription[256];	/* Description about the dictionary 	*/
	char	szDate[20];			/* Date (e.g. Wed Aug 23 10:25:45 1995)	*/
	int		nNumOfEntry;		/* Total number of existing entries		*/
} TTSUSRDICTINFO, *LPTTSUSRDICTINFO;

/*
 *	Application information block
 */
typedef struct
{
	HTTSINSTANCE	hTtsInst;		/* Handle of TTS instance			*/
	HANDLE			hThread;		/* Handle of application thread		*/
	int				nEngineId;		/* Selected engine Id				*/
	int				nLoadedUsrDict;	/* Number of loaded dictionaries 	*/
	int				nSystemMode;	/* Used system mode					*/
	TTSSPEECHMODE	SpeechMode;		/* Used speech mode					*/
	int				nCurFreq;		/* Current frequency				*/
	int				nMsgInQueue;	/* Number of unprocessed messages	*/
} TTSAPPINFO, *LPTTSAPPINFO;

/*
 *	Pcm data buffer information block
 */
typedef struct
{
	HTTSINSTANCE	hTtsInst;		/* associated TTS instance			*/
	int				nTaskNo;		/* reserved for future				*/
	int				nOutType; 		/* output pcm data type				*/
	long 			lPcmBufLen;		/* length of pcm data buffer		*/
	HANDLE			hPcmBuf;		/* handle of pcm buffer				*/
	int				nNoOfMarkers;	/* Number of markers for current 	*/
									/* text unit 						*/
	HANDLE			hMrkInfo;		/* handle of marker information 	*/
									/* block (TTSMARKINFO) array		*/
	int 			nCurTextUnitLen;/* length of current text unit		*/
} TTSPCMDATA, *LPTTSPCMDATA;

/*
 *	Marker information block
 */
typedef enum { M_MRK_TEXTUNIT=0x01, M_MRK_WORD=0x02, M_MRK_PHONEME=0x04, 
				M_MRK_BOOKMARK=0x08, M_MRK_SILENCE=0x10} mrkType;

typedef struct
{
	int			nTaskNo;		/* task number (reserved for future)	*/
	mrkType		nMrkType;		/* type of marker						*/
    long 		lSrcPos;  		/* marker position in the input text	*/
    int			nSrcTextLen;	/* text length of associated marker		*/
	long 		lDestPos;  		/* marker position in the output pcm data	*/
    long		lDestLen;  		/* pcm data length of associated marker	*/
	union {
			int		nPhoneme;	/* value of phoneme symbol 	*/
			long	lMrkId;		/* marker id				*/
			long	lReserved;	/* reserved for future		*/
		  } mrkData;
	int			nReserved;		/* future reserved						*/
} TTSMARKINFO, *LPTTSMARKINFO;

/*
 *	Lip Synchronization Structure
 */
typedef struct
{
	short int		sJawOpen;	
	short int		sTeethUpVisible;	
	short int		sTeethLoVisible;	
	short int		sMouthHeight;	
	short int		sMouthWidth;	
	short int		sMouthUpturn;	
	short int		sTonguePos;	
	short int		sLipTension;
} TTSLIPSYNC, *LPTTSLIPSYNC;

/*
 *	Error message block
 */
typedef struct
{
	int				nErrCode;		/* Error code		       			*/
    char            szErrMsg[80];  	/* Error message					*/
    char            szErrLoc[32];  	/* Location of error				*/
    int             nGravity;     	/* Gravity							*/
} TTSERRORMSG, *LPTTSERRORMSG;

/*------------------------------------------------------------------------
                        	Parameters
 -----------------------------------------------------------------------*/            
/* Available frequencies */
#define		M_FREQ_7KHZ			7		/* 7.2 KHz sample frequency		*/
#define     M_FREQ_8KHZ         8   	/* 8 KHz sample frequency       */
#define     M_FREQ_10KHZ        10  	/* 10 KHz sample frequency      */
#define     M_FREQ_11KHZ        11  	/* 11.025 KHz sample frequen    */

/* Available PCM output */
#define     M_OUT_16LINEAR      0       /* 16 bit linear PCM format     */
#define     M_OUT_MU_LAW        1       /* 8 bit Mu-law PCM format      */
#define     M_OUT_A_LAW         2       /* 8 bit A-law PCM format       */
#define     M_OUT_8WAVE         3       /* 8 bit waveform PCM format    */
#define     M_OUT_16WAVE        4       /* 16 bit waveform PCM format   */
#define     M_OUT_8LINEAR       5       /* 8 bit linear PCM format    	*/

/* Available output word format */
#define     M_OUT_STANDARD      0       /* Standard format of integer   */
#define     M_OUT_SWAPPED       1       /* Swapped format of integer    */

/* Available input character format */
#define     M_OEM_TEXT          0       /* Text consists of OEM character 		*/
#define     M_ANSI_TEXT         1       /* Text consists of ANSI character 		*/
#define     M_UNICODE_TEXT      2       /* Text consists of UNICODE character 	*/

/* Available jumping direction */
#define     M_JMP_FORWARD       1       /* jump forward					*/
#define     M_JMP_BACKWARD      2       /* jump backward				*/

/* Available jumping mode */
#define     M_JMP_TEXTUNIT      1       /* jump by text unit			*/
#define     M_JMP_WORD      	2       /* jump by word					*/
#define     M_JMP_BOOKMARK     	3       /* jump by bookmark				*/

/* Available pause & stop mode */
#define     M_MODE_IMMEDIATELY  0       /* pause or stop immediately	*/
#define     M_MODE_TEXTUNIT     1       
#define     M_MODE_WORD      	2       
#define     M_MODE_BOOKMARK     3       

/* Available notification mode */
#define     M_NOTIFY_MSG      	1       /* notify via message			*/
#define     M_NOTIFY_CALLBACK   2       /* notify via call-back function*/

/* Available marker mode */
#define     M_MRK_TEXTUNIT     	0x0001  /* marker for text unit			*/
#define     M_MRK_WORD			0x0002  /* marker for word				*/
#define     M_MRK_PHONEME		0x0004  /* marker for phoneme			*/
#define     M_MRK_BOOKMARK		0x0008  /* marker for bookmarker		*/

/* Available Notification Message Type */
#define		M_NOTIFY_TEXTUNIT		0x0001
#define		M_NOTIFY_ENDGENDATA		0x0002
#define		M_NOTIFY_ENDSPEAK		0x0004
#define		M_NOTIFY_BOOKMARK		0x0008
#define		M_NOTIFY_WORDMARK		0x0010
#define		M_NOTIFY_PHONEMEMARK	0x0020
#define		M_NOTIFY_PCMBUFDONE		0x0040
#define		M_NOTIFY_PAUSEAT		0x0080
#define		M_NOTIFY_STOPAT			0x0100


/* Available Callback Type */
#define		M_CALLBACK_TEXTUNIT		0x0001
#define		M_CALLBACK_ENDGENDATA	0x0002
#define		M_CALLBACK_ENDSPEAK		0x0004
#define		M_CALLBACK_BOOKMARK		0x0008
#define		M_CALLBACK_WORDMARK		0x0010
#define		M_CALLBACK_PHONEMEMARK	0x0020
#define		M_CALLBACK_PCMBUFDONE	0x0040
#define		M_CALLBACK_PAUSEAT		0x0080
#define		M_CALLBACK_STOPAT		0x0100

/* Available System Mode */
#define		M_SPEAKDIRECTLY			0x0001
#define		M_PUTTEXTINQUEUE		0x0002
#define		M_INTERRUPTSPEAK		0x0004
#define		M_INTERRUPTENABLED		0x0008
#define		M_INTERRUPTDISABLED		0x0010
#define		M_REALTIMESPEAK			0x0020
#define		M_HIGHPRIORITYSPEAK		0x0040
#define		M_NORMALPRIORITYSPEAK	0x0080

/* Phonetic String Mode */
#define		M_LH_PHON_STANDARD		1
#define		M_LH_PHON_PLUS			2


/*---------------------------------------------------------------------------
                        	Return Codes
 ---------------------------------------------------------------------------*/
#define		E_NOTFOUND      	-1      /* File not found                               */
#define     E_FILEOPEN      	-2      /* File creation error                          */
#define     E_FILEWRITE     	-3      /* File writing error                           */
#define     E_MEMALLOC      	-4      /* Memory allocation error                      */
#define     E_MEMLOCK      		-5      /* Memory locking error                         */
#define     E_MEMUNLOCK     	-6      /* Memory unlocking error                       */
#define     E_MEMFREE       	-7      /* Memory free error                            */
#define     E_WAVEOPEN      	-8      /* Wave Device open error                       */
#define     E_WAVECLOSE     	-9      /* Wave device closing error                    */
#define     E_WAVEFORMAT    	-10     /* Specified waveformat not supported           */
#define     E_NODEVICE      	-11     /* No wave devices available                    */
#define     E_NOINIT        	-12     /* TTS has not been initialized                 */
#define     E_FREQNOTAVAIL  	-13     /* Specified frequency not available            */
#define     E_OUTOFRANGE    	-14     /* Specified parameter is out of range.         */
#define     E_OUTNOTAVAIL   	-15     /* Specified output PCM format not available    */
#define     E_TTSBUSY   		-16     /* TTS system is busy                       	*/
#define     E_NOTAUTHORIZED 	-17     /* Not authorized TTS DLL is used.              */
#define     E_LOADDICTERR 		-18     /* Dictionary loading error              		*/
#define     E_BADDICTHANDLE		-19     /* wrong dictionary handle  					*/
#define     E_WAVEWRITE     	-20     /* Wave device writing error                    */
#define     E_NOINPUTTEXT		-21     /* No input text								*/
#define     E_BADCOMMAND		-22     /* Bad command for current state             	*/
#define     E_PHONCONVFAIL		-23     /* Grapheme to phoneme conversion fail       	*/
#define     E_INVALIDFORMAT		-24     /* Unknown dictionary format has been found     */
#define     E_CREATEINSTANCE	-25     /* Creating instance error    					*/
#define     E_NOMOREINSTANCE	-26     /* No more TTS instance available				*/
#define     E_INVALIDINST		-27     /* Invalid TTS instance has been specified		*/
#define     E_INVALIDENGINE		-28     /* Invalid TTS engine has been specified		*/
#define     E_TTSINSTBUSY  		-29     /* TTS instance is busy                       	*/
#define     E_LOADTTSENGINE		-30    	/* TTS engine loading error						*/
#define     E_NOENGINE			-31    	/* No engine has been selected					*/
#define     E_SYSTEMERROR		-32    	/* Internal system error						*/
#define     E_WAVEBUSY			-33    	/* Specified wave device is busy				*/
#define     E_INVALIDINDEX		-34    	/* Invalid dictionary entry has been specified	*/
#define     E_LONGTEXT			-35    	/* Too long source or destination text has been */
										/* used 										*/
#define     E_MAXDICTENTRY		-36    	/* Max. dictionary entries are reached			*/
#define     E_ENTRYEXIST		-37    	/* Specified entry exists already 				*/
#define     E_NOTENOUGHSPACE		-38    	/* Not enough space				*/


#define     R_SUCCESSFUL    	0       /* Function has been successfully executed      */
#define     M_FREQAVAIL     	1       /* Specified frequency available                */
#define		M_PCMGENDONE		-100    /* PCM data generation is done					*/

/*---------------------------------------------------------------------------
						Notification Message Definition
 ---------------------------------------------------------------------------*/
#define		M_MSG_ENDGENDATA	7000	/* end of generating PCM data	*/
#define		M_MSG_ENDSPEAK		7001	/* end of speaking PCM data		*/
#define		M_MSG_TEXTUNIT		7002	/* a text unit is found			*/
#define		M_MSG_BOOKMARK		7003	/* bookmarker is found			*/
#define		M_MSG_WORDMARK		7004	/* a word is found				*/
#define		M_MSG_PHONEMEMARK	7005	/* a phoneme is found			*/
#define		M_MSG_PCMBUFDONE	7006	/* a PCM buffer is generated	*/
#define		M_MSG_PAUSEAT		7007	/* paused at specified position	*/
#define		M_MSG_STOPAT		7008	/* stopped at specified position*/
#define		M_MSG_SILENCE		7009	/* playing silence				*/


#define		M_TYPE_EOF_FOUND	1		/* TTS system found EOF (0x1A)	*/
#define		M_TYPE_END_TEXT		2		/* End of text has been detected*/

/*---------------------------------------------------------------------------
						Error Gravity Definition
 ---------------------------------------------------------------------------*/
#define		M_ERR_CRITICAL		1		/* critical error				*/
#define		M_ERR_MINORERROR	2		/* not serious error			*/
#define		M_ERR_WARNING		3		/* warning						*/

/*---------------------------------------------------------------------------
                        Function Definitions
 ---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif 
int  FAR PASCAL TtsAddUsrDictEntry(HTTSINSTANCE hTtsInst, 
				 		 		   HUSRDICT hUsrDict,
								   LPSTR lpszSourceText,
								   LPSTR lpszDestText); 
int  FAR PASCAL TtsAppendText(HTTSINSTANCE hTtsInst,
										PVOID lpszInText, 
										int nTextLen,
										int nTextMode);

int  FAR PASCAL TtsDeleteUsrDictEntry(HTTSINSTANCE hTtsInst, 
									  HUSRDICT hUsrDict, int nSrcIndex,
									  LPSTR lpszSourceText);
int  FAR PASCAL TtsEnableNotify(HTTSINSTANCE hTtsInst, int nNotifyMode,
								HWND hNotifyWnd); 
int  FAR PASCAL TtsEnableUsrDict(HTTSINSTANCE hTtsInst, HUSRDICT hUsrDict,
								 BOOL bEnable);
int  FAR PASCAL TtsGenNextPcmBuf(HTTSINSTANCE hTtsInst, int nBufSize, 
							  	 LPBYTE lpszPcmBuf, int nOutMode, 
							  	 int *lpnGenSize, int *lpnTaskNo);
int  FAR PASCAL TtsGenPcmBuf( HTTSINSTANCE hTtsInst, PVOID lpszInText, 
							  int nTextLen, int nOutMode, int nTextMode, 
							  int nBufSize,  LPBYTE lpszPcmBuf, 
							  int *lpnGenSize, int *lpnTaskNo);
int  FAR PASCAL TtsGenPcmBufEx( HTTSINSTANCE hTtsInst, PVOID lpszInText, 
							  int nTextLen, int nOutMode, int nTextMode, 
							  int nBufSize,  int nNotifyMode, 
							  int *lpnTaskNo);

int  FAR PASCAL TtsGenPcmFile(HTTSINSTANCE hTtsInst, PVOID lpszInText, 
							  int nTextLen, LPSTR lpszFileName, 
							  int nOutMode, int nTextMode, 
							  int nOutType, int *lpnTaskNo);
int  FAR PASCAL TtsGenPhonString(HTTSINSTANCE hTtsInst, PVOID lpszInText,
								 int nTextLen, int nInTextMode, 
								 LPBYTE lpszPhonString, int nOutTextMode,
								 int *pnOutTextLen);
int  FAR PASCAL TtsGenUsrDict(HTTSINSTANCE hTtsInst,	LPSTR lpszFileName, 
							  int nTextMode, LPSTR lpszDictName); 
int  FAR PASCAL TtsGetAppInfo(HTTSINSTANCE hTtsInst, LPTTSAPPINFO lpAppInfo); 
BOOL FAR PASCAL TtsGetChannelStatus(HTTSINSTANCE hTtsInst, int nDeviceId); 
int  FAR PASCAL TtsGetEngineInfo(HTTSINSTANCE hTtsInst, int, LPTTSENGINEINFO lpEngineInfo); 
int  FAR PASCAL TtsGetFreq(HTTSINSTANCE hTtsInst, int *lpnFreq); 
void FAR PASCAL TtsGetIdentity(LPTTSIDENTITY lpIden); 
int  FAR PASCAL TtsGetLastErrMsg(HTTSINSTANCE hTtsInst, LPTTSERRORMSG lpErrMsg); 

int  FAR PASCAL TtsGetMrkMode(HTTSINSTANCE hTtsInst, int *lpnMrkMode); 
int  FAR PASCAL TtsGetSpeechMode(HTTSINSTANCE hTtsInst, int *lpnVolume, 
								 int *lpnSpeaker, int *lpnSpeechRate, 
								 int *lpnPitch, int *lpnPauseDur, 
								 int *lpnReadMode); 
int  FAR PASCAL TtsGetSysInfo(HTTSINSTANCE hTtsInst, LPTTSSYSTEMINFO lpSysinfo); 
int  FAR PASCAL TtsGetUsrDictEntry(HTTSINSTANCE hTtsInst, 
								   HUSRDICT hUsrDict,int nSrcInx,
								   LPSTR lpszSourceText, LPSTR lpszDestText);
int  FAR PASCAL TtsGetUsrDictInfo(HTTSINSTANCE hTtsInst,
								  HUSRDICT hUsrDict,
								  LPTTSUSRDICTINFO lpUsrDictInfo);
int  FAR PASCAL TtsGetVisual(HTTSINSTANCE hTtsInst, short int sPhoneme,
							 LPTTSLIPSYNC pLipSYnc);
int  FAR PASCAL TtsInitialize(LPHTTSINSTANCE lphTtsInst,
							   HANDLE hThread,
                               LPTTSSYSTEMINFO lpSysInfo);
int  FAR PASCAL TtsLoadUsrDict(HTTSINSTANCE hTtsInst, LPHUSRDICT lphUsrDict,
							   LPSTR lpszDctName); 
int  FAR PASCAL TtsPauseSpeech(HTTSINSTANCE hTtsInst, int nDevId);
int  FAR PASCAL TtsPauseSpeechEx(HTTSINSTANCE hTtsInst, int nDevId,
								 int nPauseMode, long lJmpValue);

int  FAR PASCAL TtsResumeSpeech(HTTSINSTANCE hTtsInst, int nDevId);
int  FAR PASCAL TtsResumeSpeechEx(HTTSINSTANCE hTtsInst, int nDevId,
								  int nDirection, int nJmpMode, long lJmpValue);

int  FAR PASCAL TtsSaveUsrDict(HTTSINSTANCE hTtsInst, HUSRDICT hUsrDict,
							   LPSTR lpszDctName); 
int  FAR PASCAL TtsSelectEngine(HTTSINSTANCE hTtsInst, int nEngineId,
							   BOOL bSoundDev); 
int  FAR PASCAL TtsSetCallback(HTTSINSTANCE hTtsInst, int nCallback,
							   FARPROC lpfCallback); 
int  FAR PASCAL TtsSetCmdSymbol(HTTSINSTANCE hTtsInst, int wCmdSymbol); 
int  FAR PASCAL TtsSetFreq(HTTSINSTANCE hTtsInst, int nFreq); 
int  FAR PASCAL TtsSetMrkMode(HTTSINSTANCE hTtsInst, int nMrkMode); 
int  FAR PASCAL TtsSetSpeechMode(HTTSINSTANCE hTtsInst, int nDefault, 
								 int nVol, int nSpeaker,
                               	 int nSpeechRate, int nPitch, 
                               	 int nPauseDur, int nSpeakMode); 
int  FAR PASCAL TtsSetSpeedUpMode(HTTSINSTANCE hTtsInst, int wRule1, 
								  int wRule2); 
int  FAR PASCAL TtsSetSystemMode(HTTSINSTANCE hTtsInst, int nSystemMode);
 
int  FAR PASCAL TtsSpeakText(HTTSINSTANCE hTtsInst, int nDevId,
							 PVOID lpszInText, int nTextLen, 
							 int nTextMode, int *lpnTaskNo);
int  FAR PASCAL TtsStopSpeech(HTTSINSTANCE hTtsInst, int nDevId);
int  FAR PASCAL TtsStopSpeechEx(HTTSINSTANCE hTtsInst, int nDevId,
								int nPauseMode, long lJmpValue);

int	 FAR PASCAL TtsUninitialize(HTTSINSTANCE hTtsInst);
int  FAR PASCAL TtsUnloadAllUsrDict(HTTSINSTANCE hTtsInst);								  
int  FAR PASCAL TtsUnloadUsrDict(HTTSINSTANCE hTtsInst, HUSRDICT hUsrDict); 
int  FAR PASCAL TtsUpdateUsrDictEntry(HTTSINSTANCE hTtsInst, 
									  HUSRDICT hUsrDict, int nSrcIndex,
									  LPSTR lpszSourceText,
									  LPSTR lpszDestText);

#ifdef __cplusplus
}
#endif 

#endif


