#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#define PGMSTR(x) (__FlashStringHelper*)(x)

//RDS Europe...
//Link: https://www.electronics-notes.com/articles/audio-video/broadcast-audio/rds-radio-data-system-pty-codes.php
const char PTY_0 []  PROGMEM  = { "Not defined Type\0" };
const char PTY_1 []  PROGMEM  = { "News            \0" };
const char PTY_2 []  PROGMEM  = { "Current Affairs \0" };
const char PTY_3 []  PROGMEM  = { "Information     \0" };
const char PTY_4 []  PROGMEM  = { "Sport           \0" };
const char PTY_5 []  PROGMEM  = { "Education       \0" };
const char PTY_6 []  PROGMEM  = { "Drama           \0" };
const char PTY_7 []  PROGMEM  = { "Culture         \0" };
const char PTY_8 []  PROGMEM  = { "Science         \0" };
const char PTY_9 []  PROGMEM  = { "Varied          \0" };
const char PTY_10[]  PROGMEM  = { "Popular Music   \0" };
const char PTY_11[]  PROGMEM  = { "Rock Music      \0" };
const char PTY_12[]  PROGMEM  = { "Easy Listening  \0" };
const char PTY_13[]  PROGMEM  = { "Light Classical \0" };
const char PTY_14[]  PROGMEM  = { "Serious Classic \0" };
const char PTY_15[]  PROGMEM  = { "Other Music     \0" };
const char PTY_16[]  PROGMEM  = { "Weather         \0" };
const char PTY_17[]  PROGMEM  = { "Finance         \0" };
const char PTY_18[]  PROGMEM  = { "Children's Progr\0" };
const char PTY_19[]  PROGMEM  = { "Social Affairs  \0" };
const char PTY_20[]  PROGMEM  = { "Religious Talk  \0" };
const char PTY_21[]  PROGMEM  = { "Phone-in        \0" };
const char PTY_22[]  PROGMEM  = { "Travel / Public \0" };
const char PTY_23[]  PROGMEM  = { "Leisure         \0" };
const char PTY_24[]  PROGMEM  = { "Jazz Music	   \0" };
const char PTY_25[]  PROGMEM  = { "Country Music   \0" };
const char PTY_26[]  PROGMEM  = { "National Music  \0" };
const char PTY_27[]  PROGMEM  = { "Oldies Music    \0" };
const char PTY_28[]  PROGMEM  = { "Folk Music      \0" };
const char PTY_29[]  PROGMEM  = { "Documentary     \0" };
const char PTY_30[]  PROGMEM  = { "Alarm Test!!    \0" };
const char PTY_31[]  PROGMEM  = { "Alarm           \0" };
const char* const string_table[] PROGMEM = {
PTY_0,  PTY_1,  PTY_2,  PTY_3, 
PTY_4,  PTY_5,  PTY_6,  PTY_7, 
PTY_8,  PTY_9,  PTY_10, PTY_11, 
PTY_12, PTY_13, PTY_14, PTY_15, 
PTY_16, PTY_17, PTY_18, PTY_19, 
PTY_20, PTY_21, PTY_22, PTY_23, 
PTY_24, PTY_25, PTY_26, PTY_27, 
PTY_28, PTY_29, PTY_30, PTY_31};

class Si4705
{
  public:
  volatile int 			RSSi;
  volatile int 			SNR;
  volatile int 			FMSTEREO;
  volatile int 			STBLEND;
  volatile int 			AFC;
  volatile int			BLTF;	
  volatile int          STC;
  volatile int 			OFFSET;
  volatile unsigned int CHANNEL;
  volatile int 			TUNE_CAP;
  
  volatile int 			PSflag   	 = false;
  volatile int 			RTflag   	 = false;
  volatile int 			PTYflag  	 = false;
  volatile int 			CTflag   	 = false;
  volatile int 			RDSflag      = false;
  const unsigned int 	channelBTM 	 = 8750;
  const unsigned int 	channelTOP 	 = 10800;
  volatile int 			updateTime   = false;
  int 					Si4705_Addr  = 0xC6 >> 1;
  unsigned  channelFilter     		 =  0;
  unsigned  oldchannelFilter  		 =  5;
  
  enum channelFilterState {ChFilterAuto, ChFilter110, ChFilter84, ChFilter60, ChFilter40};
  const int _50kHzSeek  = 5;
  const int _100kHzSeek = 10;
  const int _200kHzSeek = 20;
  
  enum muteState  {muteOFF, muteON};
  enum antState	  {FMI, LPI};
  enum HiCutState {HiCutDis_MaxAudioTrans, _2kHz, _3kHz, _4kHz, _5kHz, _6kHz, _8kHz, _11kHz};
  enum FMdeemph   {default_, EUROPA, USA};
  enum radioState {OFF, FM, LINE};
  
  uint8_t  RDS[14];
  uint16_t AF[50];
  char 	   PS[10];
  char     RT[70];
  uint16_t PICODE[2];
  char     PTY[18];
  uint16_t CLOCK[5];
  uint16_t DATE[5];
   int8_t  Timezone;
  uint8_t  TC;
  uint8_t  AF_Zaehler;
  uint8_t  AF_trying;
  uint8_t  oldRTcount;
  
       Si4705();
  void initRadio   	(int Radio, int volume, int resetPin, int intPin, int auxPin, unsigned int FMchannel);
  void initFM      	(void);
  void audioMute   	(uint8_t OnOff);
  void setAntenna  	(int FM_ANTENNA);
  void setFMDeemph 	(int Deemphasis);
  void setHiCut_Off	(int maxFreq, int Freq, int SNRHigh, int SNRLow);
  void autoChFilter (void);
  void setChFilter 	(int channelFilter);
  void setFM       	(unsigned int FMchannel);
  void setSNC      	(int StereoTreshold, int MonoTreshold, int StereoSNR, int MonoSNR);
  void forceMono    (int forceMono);
  void seekThreshold(int seekSNR, int seekRSSI);
  void seekAuto 	(int Direction, unsigned int &channel);
  void seekLimit	(unsigned int seekTop, unsigned int seekBottom);
  void seekStep		(int seekStep);
  void resetRadio  	(int _resetPin);
  void setVolume   	(int volume);
  char* fStr		(const char* str);
  void readRDS     	(void);
  void decodePI    	(void);
  void decodePS    	(void);
  void decodeRT    	(void);
  void decodeAF    	(void);
  void decodePTY   	(void);
  void decodeCT    	(void);
  void decodeTC    	(void);
  void loopAF 	   	(unsigned int &channel);
  void clearRDS    	(void);
  void readData    	(void);
  void seekData 	(void);

  private:
  unsigned int  _Utcoffset;
  unsigned int  _Utcoffsetdate;
  unsigned int  _Lo;
  unsigned int  _Hi;
  unsigned int  _Stunden;
  unsigned int  _Minuten;
  unsigned int  _Sekunden;
  unsigned int  _STD;
  unsigned long _MJD;
  unsigned int  _d;
  unsigned int  _m;
  unsigned int  _y;
  unsigned int  _Wochentag;
  unsigned int  _Puffer0;
  unsigned int  _Puffer1;
  unsigned int  _AF_scan_a;
  unsigned int  _AF_scan_b;
  
  const int     _nd[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int16_t       _j;
  
  enum RDSconst {STATUS,RDSindicator,RDSflags,RDSFIFO,AH,AL,BH,BL,CH,CL,DH,DL};
  
  void setRefClock (void);
  void convertMJD  (unsigned long MJD);

  //--++--++Default Settings--++--++//
		
  const unsigned int RDS_BLER1			= 0x00;
  const unsigned int RDS_BLER2 			= 0x01;
  const unsigned int  QuarzFreq1		= 0x80; //MSB --> 32768 Hz
  const unsigned int  QuarzFreq2		= 0x00; //LSB --> 32768 Hz 
  const unsigned int RDSFifo		   	= 0x01;
  const unsigned int SNRHighThreshold  	= 11;
  const unsigned int SNRLowThreshold   	=  6;
  const unsigned int StereoRSSIThrold  	= 35;
  const unsigned int MonoRSSIThrold    	= 12;
  const unsigned int StereoSNRThrhold  	= 26;
  const unsigned int MonoSNRThrhold    	= 11;
  const unsigned int seekSNR			=  4;
  const unsigned int seekRSSI		  	=  3;
  const int Deemphasis 		  			= EUROPA;

  //::-------------RDS-------------:://
  char 		   PSTemp[10];
  char 		   RTTemp[70];
  unsigned int AF_PICODE[2];
  unsigned     _PTY, _PTY1, _PTY2;
  
  char Data[8];
  int  RT_decodeProgress      	= 0x00;
  
  //Tune Register
  const int FMTuneStatus1     	= 0x23;
  const int FMTuneStatus2     	= 0x22;
  const int TuneReg           	= 0x01;

  //RDS
  const int getRDS1           	= 0x24;
  const int getRDS2           	= 0x01;

  //Property
  const int setProperty1      	= 0x12;
  const int setProperty2      	= 0x00;
  
  //Stereo - Mono SNC Property
  const int StMoRSSIProperty  	= 0x18;
  const int SterRSSIProperty1 	= 0x00;
  const int SterRSSIProperty2 	= 0x00;
  const int MonoRSSIProperty1 	= 0x01;
  const int MonoRSSIProperty2 	= 0x00;
  const int StMoSNRProperty	  	= 0x18;
  const int SterSNRProperty1  	= 0x04;
  const int SterSNRProperty2  	= 0x00;
  const int MonoSNRProperty1  	= 0x05;
  const int MonoSNRProperty2  	= 0x00;
  
  //Seek SNR Property
  const int SeekSNRProperty   	= 0x14;
  const int SeekSNRProperty1  	= 0x03;
  const int SeekSNRProperty2  	= 0x00;

  //Seek RSSI Property
  const int SeekRSSIProperty  	= 0x14;
  const int SeekRSSIProperty1 	= 0x04;
  const int SeekRSSIProperty2 	= 0x00;
  
  //Seek Band Top
  const int SeekTopProperty  	= 0x14;
  const int SeekTopProperty1 	= 0x01;
  
  //Seek Band Bottom
  const int SeekBottomProperty  = 0x14;
  const int SeekBottomProperty1 = 0x00;
  
  //Seek Step
  const int SeekStepProperty    = 0x14;
  const int SeekStepProperty1   = 0x02;
  const int SeekStepProperty2   = 0x00;
  
  //Quarz - Property
  const int propertyRefClock1 	= 0x00;
  const int propertyRefClock2 	= 0x01;
  const int intRefClock1      	= 0x00;
  const int intRefClock2      	= 0x09;
  
  const int propertyQuarz1    	= 0x02;
  const int propertyQuarz2    	= 0x01;

  //Volume - Property
  const int propertyVolume1   	= 0x40;
  const int propertyVolume2   	= 0x00;
  const int propertyVolume3   	= 0x00;

  //RDS - Property
  const int propertyRDS       	= 0x15;
  const int registerRDS1      	= 0x01;
  const int registerRDS2      	= 0x02;
  const int registerRDSFifo   	= 0x00;
  
  //Antenneneingang (Si4705)
  const int propertyAntenna1  	= 0x11;
  const int propertyAntenna2  	= 0x07;
  const int registerAntenna1  	= 0x00;
  const int registerAntenna2  	= 0x01;
  
  //FM Deemphasis
  const int propertyDeemph1   	= 0x11;
  const int propertyDeemph2   	= 0x00;
  const int registerDeemph    	= 0x00;
  
  //Channelfilter Property
  const int channelFilter1    	= 0x11;
  const int channelFilter2    	= 0x02;
  
  //Hi-Cut Cutoff Property
  const int propertyHiCut     	= 0x1A;
  const int RegHiCut	      	= 0x00;
  const int HiCutFreq         	= 0x06;
  
  //Hi-Cut SNR_High_Threshold
  const int propertyHiCutHigh 	= 0x00;
  
  //Hi-Cut SNR_LOW_Threshold
  const int propertyHiCutLow  	= 0x01;
  const int SNRMask			  	= 0x7F;
  
  //Seek Station
  const int seekStationCmd    	= 0x21;
  const int seekUpCmd         	= 0x0C;
  const int seekDownCmd       	= 0x04;
  
  //Power
  const int startUp           	= 0x01;
  const int shutDown          	= 0x11;
  const int AMwithoutInt      	= 0x11;
  const int FMwithoutInt      	= 0x10;
  const int GPO2OEN			  	= 0x40;
  const int analogAudioMode   	= 0x05;
  
  //Mute
  const int setAudioMute 	 	= 0x81;
  const int AudioMuteEn       	= 0x02;
  const int AudioMuteDs       	= 0x00;
  
  const int GPIO1Conf1        	= 0x80;
  const int GPIO1Conf2        	= 0x02;

  //Channel
  const int setAMchannel1     	= 0x40;
  const int setAMchannel2     	= 0x00;
  const int setAMchannel3     	= 0x00;

  const int setFMchannel1     	= 0x20;
  const int setFMchannel2     	= 0x00;
  const int setFMchannel3     	= 0x00;
  
};

extern Si4705 Radio;
