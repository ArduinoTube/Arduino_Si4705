#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>

class Si4705
{
  public:
  volatile int 			RSSi;
  volatile int 			SNR;
  volatile int 			FMSTEREO;
  volatile int 			STBLEND;
  volatile int 			AFC;
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
  unsigned  channelFilter     			=  0;
  
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
  char     PTY[16];
  uint16_t CLOCK[5];
  uint16_t DATE[5];
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
  const unsigned int SNRLowThreshold   	=  2;
  const unsigned int StereoRSSIThrold  	= 22;
  const unsigned int MonoRSSIThrold    	= 15;
  const unsigned int StereoSNRThrhold  	= 20;
  const unsigned int MonoSNRThrhold    	= 10;
  const unsigned int seekSNR			=  4;
  const unsigned int seekRSSI		  	=  6;
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
