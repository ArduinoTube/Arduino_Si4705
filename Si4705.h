#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>

class Si4705
{
  public:
  volatile int RSSi;
  volatile int SNR;
  volatile int FMSTEREO;
  volatile int STBLEND;
  volatile int AFC;
  volatile int OFFSET;
  volatile int CHANNEL;
  volatile int TUNE_CAP;
  
  volatile int PSflag   	= false;
  volatile int RTflag   	= false;
  volatile int PTYflag  	= false;
  volatile int CTflag   	= false;
  volatile int RDSflag      = false;
  const    int channelBTM 	= 8750;
  const    int channelTOP 	= 10800;
  volatile int updateTime   = false;
  enum muteState  {muteOFF, muteON};
  enum antState	  {FMI, LPI};
  enum HiCutState {HiCutDis_MaxAudioTrans, _2kHz, _3kHz, _4kHz, _5kHz, _6kHz, _8kHz, _11kHz};
  enum FMdeemph   {default_, EUROPA, USA};
  enum radioState {OFF, FM, LINE};
  
  uint16_t RDS[14];
  uint16_t AF[50];
  char PS[9];
  char RT[65];
  unsigned int PICODE[2];
  char PTY[16];
  unsigned  CLOCK[5];
  unsigned  DATE[5];
  int  TC;
  int  AF_Zaehler;
  int  AF_trying;
  
       Si4705();
  void initRadio   (int Radio, int volume, int resetPin, int intPin, int auxPin, unsigned FMchannel);
  void initFM      (void);
  void audioMute   (uint8_t OnOff);
  void setAntenna  (int FM_ANTENNA);
  void setFMDeemph (int Deemphasis);
  void setHiCut_Off(int maxFreq, int Freq, int SNRHigh, int SNRLow);
  void setChFilter (int channelFilter);
  void setFM       (unsigned int FMchannel);
  void setSNC      (int StereoTreshold, int MonoTreshold, int StereoSNR, int MonoSNR);
  void resetRadio  (int _resetPin);
  void setVolume   (int volume);
  void readRDS     (void);
  void decodePI    (void);
  void decodePS    (void);
  void decodeRT    (void);
  void decodeAF    (void);
  void decodePTY   (void);
  void decodeCT    (void);
  void decodeTC    (void);
  void loopAF 	   (unsigned int &channel);
  void clearRDS    (void);
  void readData    (void);

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
  unsigned int _Puffer0;
  unsigned int _Puffer1;
  unsigned int _AF_scan_a;
  unsigned int _AF_scan_b;
  
  const int     _nd[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int16_t       _j;
  
  enum RDSconst {STATUS,RDSindicator,RDSflags,RDSFIFO,AH,AL,BH,BL,CH,CL,DH,DL};
  
  void setRefClock (void);
  void convertMJD  (unsigned long MJD);

  //--++--++Default Settings--++--++//
  const int Si4705_Addr       = 0xC6 >> 1;
  const int RDS_BLER1         = 0x00;
  const int RDS_BLER2         = 0x01;
  unsigned  QuarzFreq1        = 0x80; //MSB --> 32768 Hz
  unsigned  QuarzFreq2        = 0x00; //LSB --> 32768 Hz 
  const int RDSFifo			  = 0x01;
  const int SNRHighThreshold  = 0x0A;
  const int SNRLowThreshold   = 0x06;
  const int StereoRSSIThrold  = 0x15;
  const int MonoRSSIThrold    = 0x0A;
  const int StereoSNRThrhold  = 0x15;
  const int MonoSNRThrhold    = 0x10;
  const int Deemphasis 		  = EUROPA;
  
  
  enum channelFilterState {ChFilterAuto, ChFilter110, ChFilter84, ChFilter60, ChFilter40};
  
  //::-------------RDS-------------:://
  char PSTemp[9];
  char RTTemp[65];
  unsigned int AF_PICODE[2];
  unsigned _PTY, _PTY1, _PTY2;
  
  char Data[8];
  int  RT_decodeProgress      = 0x00;
  
  //Tune Register
  const int FMTuneStatus1     = 0x23;
  const int FMTuneStatus2     = 0x22;
  const int TuneReg           = 0x01;

  //RDS
  const int getRDS1           = 0x24;
  const int getRDS2           = 0x01;

  //Property
  const int setProperty1      = 0x12;
  const int setProperty2      = 0x00;
  //Stereo - Mono SNC Property
  const int StMoRSSIProperty  = 0x18;
  const int SterRSSIProperty1 = 0x00;
  const int SterRSSIProperty2 = 0x00;
  const int MonoRSSIProperty1 = 0x01;
  const int MonoRSSIProperty2 = 0x00;
  const int StMoSNRProperty	  = 0x18;
  const int SterSNRProperty1  = 0x04;
  const int SterSNRProperty2  = 0x00;
  const int MonoSNRProperty1  = 0x05;
  const int MonoSNRProperty2  = 0x00;
  
  //Quarz - Property
  const int propertyRefClock1 = 0x00;
  const int propertyRefClock2 = 0x01;
  const int intRefClock1      = 0x00;
  const int intRefClock2      = 0x09;
  
  const int propertyQuarz1    = 0x02;
  const int propertyQuarz2    = 0x01;

  //Volume - Property
  const int propertyVolume1   = 0x40;
  const int propertyVolume2   = 0x00;
  const int propertyVolume3   = 0x00;

  //RDS - Property
  const int propertyRDS       = 0x15;
  const int registerRDS1      = 0x01;
  const int registerRDS2      = 0x02;
  const int registerRDSFifo   = 0x00;
  
  //Antenneneingang (Si4705)
  const int propertyAntenna1  = 0x11;
  const int propertyAntenna2  = 0x07;
  const int registerAntenna1  = 0x00;
  const int registerAntenna2  = 0x01;
  
  //FM Deemphasis
  const int propertyDeemph1   = 0x11;
  const int propertyDeemph2   = 0x00;
  const int registerDeemph    = 0x00;
  
  //Channelfilter Property
  const int channelFilter1    = 0x11;
  const int channelFilter2    = 0x02;
  
  //Hi-Cut Cutoff Property
  const int propertyHiCut     = 0x1A;
  const int RegHiCut	      = 0x00;
  const int HiCutFreq         = 0x06;
  
  //Hi-Cut SNR_High_Threshold
  const int propertyHiCutHigh = 0x00;
  
  //Hi-Cut SNR_LOW_Threshold
  const int propertyHiCutLow  = 0x01;
  const int SNRMask			  = 0x7F;
  
  //Power
  const int startUp           = 0x01;
  const int shutDown          = 0x11;
  const int AMwithoutInt      = 0x11;
  const int FMwithoutInt      = 0x10;
  const int GPO2OEN			  = 0x40;
  const int analogAudioMode   = 0x05;
  
  //Mute
  const int setAudioMute 	  = 0x81;
  const int AudioMuteEn       = 0x02;
  const int AudioMuteDs       = 0x00;
  
  const int GPIO1Conf1        = 0x80;
  const int GPIO1Conf2        = 0x02;

  //Channel
  const int setAMchannel1     = 0x40;
  const int setAMchannel2     = 0x00;
  const int setAMchannel3     = 0x00;

  const int setFMchannel1     = 0x20;
  const int setFMchannel2     = 0x01;
  const int setFMchannel3     = 0x00;
  
};

extern Si4705 Radio;
