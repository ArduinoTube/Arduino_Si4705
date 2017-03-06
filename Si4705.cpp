#include "Arduino.h"
#include <Wire.h>
#include "Si4705.h"

/*
Si4705 Bibliothek
Version:   1.1-01022017
Ersteller: ArduinoTube

###########################FM Radio###########################
:: Empfang von 87.50 MHz bis 108.00 MHz                     ::
:: Volle RDS unterstützung (PS,PTY,RT,CT,AF)                ::
:: Signalstärke und -rauschabstand können ausgelesen werden ::
:: ...                                                      ::
**************************************************************
*/

/**********************************************************
*-----------------------Konstruktor-----------------------*
**********************************************************/
Si4705::Si4705()
{}


/*******************************************************
*----------Si4705 Initialisieren fuer Empfang----------* 
********************************************************/
void Si4705::initRadio(int Power, int volume, int resetPin, int intPin, int auxPin, unsigned int FMchannel)
{
	Wire.begin();
	pinMode(resetPin, OUTPUT);
	pinMode(intPin,   INPUT);
	pinMode(auxPin,   INPUT);
	resetRadio(resetPin);
	digitalWrite(intPin,HIGH);  
	initFM();
	audioMute(muteON);
	if(Power==FM)
	{
	  setAntenna(LPI);
	  seekLimit(channelTOP, channelBTM);
	  seekStep(_50kHzSeek);
	  seekThreshold(seekSNR,seekRSSI);
	  setFMDeemph(Deemphasis);
	  setChFilter(ChFilterAuto);
	  setHiCut_Off(HiCutDis_MaxAudioTrans,_6kHz,SNRHighThreshold,SNRLowThreshold);
	  setSNC(StereoRSSIThrold,MonoRSSIThrold,StereoSNRThrhold,MonoSNRThrhold);
	  delay(500);
	  setFM(FMchannel);
	  setVolume(volume);
	  digitalWrite(auxPin,LOW);
	  audioMute(muteOFF);
	}
	else if(Power==LINE)
	{
	  digitalWrite(auxPin,HIGH);
	  audioMute(muteOFF);
	}
	else digitalWrite(intPin,LOW); 
	
}

/*******************************************************
*----------------------Set Audio Mute------------------*
*******************************************************/
void Si4705::audioMute(uint8_t OnOff)
{
	//Set GPIO1 State  
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setAudioMute);
	if(OnOff==0)Wire.write(AudioMuteDs);
	if(OnOff!=0)Wire.write(AudioMuteEn);
	Wire.endTransmission();
	delay(20);
}

/*******************************************************
*-----------------FM Channel Filter--------------------*
*******************************************************/
void Si4705::setChFilter (int channelFilter_)
{
	audioMute(muteON);
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
	Wire.write(setProperty2);	
	Wire.write(channelFilter1);
	Wire.write(channelFilter2);
	Wire.write(channelFilter_ >> 8);
	Wire.write(channelFilter_&0xFF);
	Wire.endTransmission();
	delay(40);
	audioMute(muteOFF);
	channelFilter=channelFilter_;
}

/*******************************************************
*-----------------FM Hi-Cut Filter--------------------*
*******************************************************/
void Si4705::setHiCut_Off (int maxFreq, int Freq, int SNRHigh, int SNRLow)
{	
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
	Wire.write(setProperty2);	
	Wire.write(propertyHiCut);
	Wire.write(propertyHiCutHigh);
	Wire.write(RegHiCut);
	Wire.write(SNRHigh&SNRMask);
	Wire.endTransmission();
	delay(10);
	
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
	Wire.write(setProperty2);	
	Wire.write(propertyHiCut);
	Wire.write(propertyHiCutLow);
	Wire.write(RegHiCut);
	Wire.write(SNRLow&SNRMask);
	Wire.endTransmission();
	delay(10);
	
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
	Wire.write(setProperty2);	
	Wire.write(propertyHiCut);
	Wire.write(HiCutFreq);
	Wire.write(RegHiCut);
	Wire.write((Freq)|(maxFreq << 4));
	Wire.endTransmission();
	delay(10);
}

/*******************************************************
-------Deemphasis Initialisieren FM EUROPA/USA---------*
*******************************************************/
void Si4705::setFMDeemph (int Deemphasis)
{
	
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
	Wire.write(setProperty2);
	Wire.write(propertyDeemph1);
	Wire.write(propertyDeemph2);
	Wire.write(registerDeemph);
	Wire.write(Deemphasis);
	Wire.endTransmission();
	delay(10);
}

/*******************************************************
-------Antenneneingang Initialisieren FMI / LPI--------*
*******************************************************/
void Si4705::setAntenna (int FM_ANTENNA)
{
	
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
	Wire.write(setProperty2);
	Wire.write(propertyAntenna1);
	Wire.write(propertyAntenna2);
	Wire.write(registerAntenna1);
	Wire.write(FM_ANTENNA);
	Wire.endTransmission();
	delay(10);
}

/*******************************************************
*----------Chip Initialisieren für FM Empfang----------* 
********************************************************/
void Si4705::initFM (void)
{
    Wire.beginTransmission(Si4705_Addr);
    Wire.write(startUp);
    Wire.write(FMwithoutInt|GPO2OEN);
    Wire.write(analogAudioMode);
    Wire.endTransmission();
    delay(10);
    
    setRefClock();

    Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(propertyRDS);
    Wire.write(registerRDS1);
    Wire.write(registerRDSFifo);
    Wire.write(RDSFifo);
    Wire.endTransmission();
    delay(10);
      
    Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(propertyRDS);
    Wire.write(registerRDS2);
    Wire.write(RDS_BLER1);
    Wire.write(RDS_BLER2);
    Wire.endTransmission();
    delay(10);
	
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(GPIO1Conf1);
	Wire.write(GPIO1Conf2);
	Wire.endTransmission();
	delay(20);

}

/*******************************************************
*----------------Suchgrenzen einstellen----------------* 
********************************************************/
void Si4705::seekThreshold(int seekSNR, int seekRSSI)
{
	Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(SeekSNRProperty);
    Wire.write(SeekSNRProperty1);
	Wire.write(SeekSNRProperty2);
	Wire.write(seekSNR&0x7F);
	Wire.endTransmission();
	delay(50);

	Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(SeekRSSIProperty);
    Wire.write(SeekRSSIProperty1);
	Wire.write(SeekRSSIProperty2);
	Wire.write(seekRSSI&0x7F);
	Wire.endTransmission();
	delay(50);
}

void Si4705::seekStep (int seekStep)
{
	Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(SeekStepProperty);
    Wire.write(SeekStepProperty1);
	Wire.write(SeekStepProperty2);
	Wire.write(seekStep&0x1F);
	Wire.endTransmission();
	delay(50);	
}

/*******************************************************
*----------------Suchgrenzen einstellen----------------* 
********************************************************/
void Si4705::seekLimit(unsigned int seekTop, unsigned int seekBottom)
{
	Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(SeekBottomProperty);
    Wire.write(SeekBottomProperty1);
	Wire.write(seekBottom >> 8);
	Wire.write(seekBottom&0xFF);
	Wire.endTransmission();
	delay(50);
	
	Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(SeekTopProperty);
    Wire.write(SeekTopProperty1);
	Wire.write(seekTop >> 8);
	Wire.write(seekTop&0xFF);
	Wire.endTransmission();
	delay(50);	
}

/*******************************************************
*-----Referenz-Taktquelle festlegen und einstellen-----* 
********************************************************/
void Si4705::setRefClock (void)
{
    Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(propertyRefClock1);
    Wire.write(propertyRefClock2);
    Wire.write(intRefClock1);
    Wire.write(intRefClock2);
    Wire.endTransmission();
    delay(100);
    
    Wire.beginTransmission(Si4705_Addr);
    Wire.write(setProperty1);
    Wire.write(setProperty2);
    Wire.write(propertyQuarz1);
    Wire.write(propertyQuarz2);
    Wire.write(QuarzFreq1);     
    Wire.write(QuarzFreq2);
    Wire.endTransmission();
    delay(100);
}

/*******************************************************
*------------FM Empfangsfrequenz einstellen------------* 
********************************************************/
void Si4705::setFM (unsigned int FMchannel)
{ 
   if(FMchannel>channelTOP)FMchannel=channelBTM;
   if(FMchannel<channelBTM)FMchannel=channelTOP;
   Wire.beginTransmission(Si4705_Addr);
   Wire.write(setFMchannel1);
   Wire.write(setFMchannel2);
   Wire.write(FMchannel>>8);
   Wire.write(FMchannel&0xFF);
   Wire.write(setFMchannel3);
   Wire.endTransmission();
   delay(10);
}

/*******************************************************
*--------------FM Stereo/Mono Umschalten---------------* 
********************************************************/
void Si4705::forceMono (int forceMono)
{
	if(forceMono==true) setSNC(127,127,127,127);
	if(forceMono==false)setSNC(StereoRSSIThrold,MonoRSSIThrold,StereoSNRThrhold,MonoSNRThrhold);
}

/*******************************************************
*----------------FM Sendersuchlauf Si4735--------------* 
********************************************************/
void Si4705::seekAuto (int Direction, unsigned int &channel)
{
    clearRDS(); 
    Wire.beginTransmission(Si4705_Addr);
    Wire.write(seekStationCmd);
	if(Direction>0)Wire.write(seekUpCmd);
	if(Direction<0)Wire.write(seekDownCmd);
	Wire.endTransmission();
    delay(1000);
	while(AFC&(1<<0)){seekData();delay(500);}
	delay(500);
	seekData();
	channel = CHANNEL;
}


/*******************************************************
*Set Stereo/Mono Threshold fuer SNC abh. von RSSI / SNR*
********************************************************/
void Si4705::setSNC (int StereoThreshold, int MonoThreshold, int StereoSNR, int MonoSNR)
{
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
    Wire.write(setProperty2);
	Wire.write(StMoRSSIProperty);
	Wire.write(SterRSSIProperty1);
	Wire.write(SterRSSIProperty2);
	Wire.write(StereoThreshold);
    Wire.endTransmission();
    delay(10);
   
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
    Wire.write(setProperty2);
	Wire.write(StMoRSSIProperty);
	Wire.write(MonoRSSIProperty1);
	Wire.write(SterRSSIProperty2);
	Wire.write(MonoThreshold);
	Wire.endTransmission();
    delay(10);
	
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
    Wire.write(setProperty2);
	Wire.write(StMoSNRProperty);
	Wire.write(SterSNRProperty1);
	Wire.write(SterSNRProperty2);
	Wire.write(StereoSNR);
    Wire.endTransmission();
    delay(10);
   
	Wire.beginTransmission(Si4705_Addr);
	Wire.write(setProperty1);
    Wire.write(setProperty2);
	Wire.write(StMoSNRProperty);
	Wire.write(MonoSNRProperty1);
	Wire.write(MonoSNRProperty2);
	Wire.write(MonoSNR);
	Wire.endTransmission();
    delay(10);
}

/*******************************************************
*--------------------Radiochip reset-------------------* 
********************************************************/
void Si4705::resetRadio (int _resetPin)
{
  digitalWrite( _resetPin, HIGH );
  delay(10);
  digitalWrite( _resetPin, LOW  );
  delay(10);
}

/*******************************************************
*-----------------Lautstärke einstellen----------------* 
********************************************************/
void Si4705::setVolume (int volume)
{
  if(volume>=40)volume=0;
  if(volume>=30)volume=30;
  if(volume<=0)volume=0;
  Wire.beginTransmission(Si4705_Addr);
  Wire.write(setProperty1);
  Wire.write(setProperty2);
  Wire.write(propertyVolume1);
  Wire.write(propertyVolume2);
  Wire.write(propertyVolume3);
  Wire.write(volume*2);
  Wire.endTransmission();
  delay(10);
}

/*******************************************************
*--------------Lese RDS Daten aus dem Chip-------------* 
********************************************************/
void Si4705::readRDS (void)
{
  Wire.beginTransmission(Si4705_Addr);
  Wire.write(getRDS1);
  Wire.write(getRDS2);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(Si4705_Addr, 32);
  for(int Times=0; ((Times<=12)&&(Wire.available())); Times++)
  RDS[Times] = Wire.read();
  Wire.endTransmission();
  delay(40);
}

/*******************************************************
*-----------------Decodiere RDS-PI-Code----------------* 
********************************************************/
void Si4705::decodePI (void)
{
	int GroupType = RDS[6]>>4;
	int RDSSynch  = RDS[2]&1;
	if((GroupType==0)&&(RDSSynch==1))
	{
		if(RDS[4])PICODE[0]=RDS[4];
		if(RDS[5])PICODE[1]=RDS[5];
		RDS[4]=0;
		RDS[5]=0;
	}
}

/*******************************************************
*---------------Decodiere RDS-Sendernamen--------------* 
********************************************************/
void Si4705::decodePS (void)
{
  int GroupType = RDS[6]>>4;
  int PSAdress  = 0;
  int RDSSynch  = RDS[2]&1;
  if((GroupType==0)&&(RDSSynch==1)&&(PSflag==false))
  {
    PSAdress=RDS[7]&3;
    PSAdress*=2;
    PSTemp[PSAdress]   = RDS[10];
    PSTemp[PSAdress+1] = RDS[11];
    PSTemp[PSAdress+2] = '\0';
    if(strlen(PSTemp)==8)
    {
      PSflag = true;
      for(int copyPS = 0; copyPS<=8; copyPS++)PS[copyPS]=PSTemp[copyPS];
      for(int clearPS = 0; clearPS<=8; clearPS++)PSTemp[clearPS]=0;
    }
  }
}

/*******************************************************
*----------------Decodiere RDS-Radiotext---------------* 
********************************************************/
void Si4705::decodeRT (void)
{
  volatile uint8_t GroupType = RDS[6] >> 4;
  volatile uint8_t RDSSynch  = RDS[2]&1;
  volatile uint8_t RTAdress  = 0;
  volatile uint8_t LZA       = 0;
  volatile uint8_t LZE       = 0;
  volatile uint8_t strl      = 0;
  volatile uint8_t BLER		 = RDS[12];
  if((GroupType==2)&&(RT_decodeProgress<2)&&(RDSSynch==1)&&(RTflag==false))
  {
      RTAdress=RDS[7]&15;
      RTAdress*=4;
      if((RT_decodeProgress==0)&&(RTAdress==0))
      {
         RT_decodeProgress=1;
         for(int Clear=0; Clear<=64; Clear++)RTTemp[Clear]= 0;
      }
	  strl=strlen(RTTemp);
      if((RT_decodeProgress==1)&&(strl)&&(RTAdress<oldRTcount))
      {
		  for(int Clear=0; Clear<=64; Clear++)RT[Clear] = 0;
          for(LZA=0;      RTTemp[LZA]<=32; LZA++);
          for(LZE=64;      RTTemp[LZE]<=32; LZE--);
		  
          for(int copy = LZA; copy<=LZE; copy++)
		  {
			  RT[copy-LZA] = RTTemp[copy];
			  RT[copy-LZA+1] ='\0';
		  }
          for(int Clear=0; Clear<=64; Clear++)RTTemp[Clear]= 0;
          RT_decodeProgress=0;
          RTflag = true;
		  oldRTcount=0;
      }
	  oldRTcount=RTAdress;
	  //if((BLER)&&(RT_decodeProgress==1))RT_decodeProgress=0;
      if(RT_decodeProgress==1)
      {	  
          RTTemp[RTAdress]   = RDS[ 8];
          RTTemp[RTAdress+1] = RDS[ 9];
          RTTemp[RTAdress+2] = RDS[10];
          RTTemp[RTAdress+3] = RDS[11];
		  RTTemp[RTAdress+4] = '\0';
      }
  }
}

/*******************************************************
*---------------Decodiere RDS-Programmtyp--------------* 
********************************************************/
void Si4705::decodePTY (void)
{
  int GroupType = RDS[6]>>4;
  int RDSSynch  = RDS[2]&1;
  if(GroupType==0)
  {
    _PTY1 = ((RDS[7]&0xE0)>>5);
    _PTY2 = (((RDS[6]&0x3))<<3);
    _PTY = _PTY1+_PTY2;
  } 
  switch(_PTY)
  {
	case  0:strcpy(PTY,"unb. Format \0");break;
    case  1:strcpy(PTY,"Nachrichten \0");break;
    case  2:strcpy(PTY,"Aktuelles   \0");break;
    case  3:strcpy(PTY,"Information \0");break;
    case  4:strcpy(PTY,"Sport       \0");break;
    case  5:strcpy(PTY,"Bildung     \0");break;
	
    case  6:{strcpy(PTY,"H_rspiel   \0"); 
			PTY[1]=char(239);}		break;
    
	case  7:strcpy(PTY,"Kultur      \0");break;
    case  8:strcpy(PTY,"Wissenschaft\0");break;
    case  9:strcpy(PTY,"Wortprogramm\0");break;
    case 10:strcpy(PTY,"Pop Musik   \0");break;
    case 11:strcpy(PTY,"Rock Musik  \0");break;
    case 12:strcpy(PTY,"Easy Listen.\0");break;
    case 13:strcpy(PTY,"Light Class.\0");break;
    case 14:strcpy(PTY,"Ernste Musik\0");break;
    case 15:strcpy(PTY,"Andere Musik\0");break;
	case 31:strcpy(PTY,"Alarm       \0");break;
  }
  if(!RDSSynch)strcpy(PTY,"Sendertyp nV\0");
  if(RDSSynch )PTYflag = true;
  else         PTYflag = false;
}

/*******************************************************
*------------Decodiere RDS-Uhrzeit und Datum-----------* 
********************************************************/
void Si4705::decodeCT (void)
{
  int GroupType = RDS[BH]>>4;
  int RDSSynch  = RDS[RDSindicator]&1;
  if((GroupType==4)&&(CTflag==false)&&(RDSSynch))
  {
	_MJD=0;
	_Utcoffset = RDS[DL]&31;                 
	_Utcoffset = _Utcoffset / 2;
	_Lo = RDS[DL] / 64;                      
	_Hi = RDS[DH]&15;                        
	_Hi = _Hi * 4;
	_Minuten = _Hi + _Lo;
	_Lo = RDS[DH] / 16;                      
		
	_Hi = RDS[CL]&1;                          
	_Hi *= 16;
	_STD = _Hi + _Lo;
	if(RDS[11]>>5==1)
	{_STD -= _Utcoffset;}
	else
	{_STD += _Utcoffset;}
	if(_STD>=24){_STD-=24;_MJD+=1;}
	_Stunden=_STD;
	_Utcoffsetdate=_Utcoffset;
	_MJD=(((RDS[CH]<<7)+(RDS[CL]>>1)+((RDS[BL]&0b11)<<15)));
	if((_STD!=32)&&(_Minuten!=32)&&((_Stunden!=0)||(_Minuten!=0)))
	{
		CLOCK[0]=_Stunden;
		CLOCK[1]=_Minuten;
		_Sekunden=0;
	}
	if(_MJD)
	{
		convertMJD(_MJD);
		DATE[0]=_d;
		DATE[1]=_m;
		DATE[2]=_y;
		DATE[3]=_Wochentag;
		_MJD=0;
		if((CLOCK[0]<=23)&&(CLOCK[0]>=0)&&(CLOCK[1]<=59)&&(CLOCK[1]>=0)&&(DATE[0]>0)&&(DATE[1]>0)&&(DATE[2]>0))
		CTflag = true;
	updateTime = true;
	}
  }
}

/*******************************************************
*------------Wandle den RDS-MJD zum Datum um-----------* 
********************************************************/
void Si4705::convertMJD (unsigned long MJD)
{
	_d = 0;
	_m = 0;
	_y = 14;
	_j = MJD-56658+1;  // 56658 = MJD of 01.01.2014
	_Wochentag=2;
	for(int zaheler=_Wochentag; zaheler<=(_j); zaheler++)
	{
		_Wochentag++;
		if(_Wochentag>6)_Wochentag=0;
	}
	while (_j>0)
	{
		_d = _j;
		if (_m>=12) {_m = 0; _y++;} // Wenn das Jahr vollendet ist, dann Jahreswechsel und Monatsbeginn "Januar"

		_j = _j-_nd[_m];
		
		_m++;
		if (_y%4==0 && _m==2) _j--;
	}
}

/*******************************************************
*----------------------RDS TC--------------------------*
*******************************************************/
void Si4705::decodeTC (void)
{
  int GroupType = RDS[6]>>4;
  int RDSSynch  = RDS[2]&1;
  if(GroupType==0)
  {
	  if(((RDS[6]&(1<<2))==0)&&((RDS[7]&(1<<4)))) 		TC=1;	// EON is available
	  else if(((RDS[6]&(1<<2)))&&((RDS[7]&(1<<4))==0)) 	TC=2;	// TP Signal is available
	  else if(((RDS[6]&(1<<2)))&&((RDS[7]&(1<<4))))  	TC=3;	// Traffic announcement is now
	  else TC=0;
  }
}

/*******************************************************
*----------------------RDS AF--------------------------*
*******************************************************/
void Si4705::decodeAF (void)
{
	int GroupType = RDS[6]>>4;
	int RDSSynch  = RDS[2]&1;
	if((AF_Zaehler<40)&&(GroupType==0)&&(RDSSynch))
	{
		_Puffer0=((RDS[8]+channelBTM/10)*10);
		_Puffer1=((RDS[9]+channelBTM/10)*10);
		if ((_Puffer0>channelBTM)&&(_Puffer0<=channelTOP)&&(_Puffer1>channelBTM)&&(_Puffer1<=channelTOP))
		{
			for (_AF_scan_a = 0; ((_AF_scan_a <= 40) && ((AF[_AF_scan_a] != _Puffer0)) && (_Puffer0 != _Puffer1)); _AF_scan_a ++);
			for (_AF_scan_b = 0; ((_AF_scan_b <= 40) && ((AF[_AF_scan_b] != _Puffer1)) && (_Puffer0 != _Puffer1)); _AF_scan_b ++);
			
			if (_AF_scan_a >= 40){AF[AF_Zaehler]=_Puffer0;AF_Zaehler++;}
			if (_AF_scan_b >= 40){AF[AF_Zaehler]=_Puffer1;AF_Zaehler++;}
		}
		AF_trying = 0;
	}
}

/*******************************************************
*----------------------AF Suchlauf---------------------* 
********************************************************/
void Si4705::loopAF (unsigned int &channel)
{
	AF_PICODE[0]=PICODE[0];
	AF_PICODE[1]=PICODE[1];
	PICODE[0]=RDS[4]=0;
	PICODE[1]=RDS[5]=0;
	if((AF_PICODE[0]>0)&&(AF_PICODE[1]>0))
	{
		audioMute(muteON);
		for(int AFcount = 0; AFcount <= AF_Zaehler; AFcount++)
		{
			while(channel==AF[AFcount])
			{
				AFcount++;
				if(AFcount==AF_Zaehler)break;
			}
			setFM(AF[AFcount]);
			PICODE[0]=PICODE[1]=0;
			for(int countRDS = 0; countRDS <= 200; countRDS++)
			{
				readData();
				readRDS();
				decodePI();
				if((PICODE[0])&&(PICODE[1]))break;
				if((countRDS>10)&&((SNR<14)||(OFFSET>50)))break;
			}
			
			if(AF_PICODE[1]==PICODE[1])
			{
				AF_trying=0;
				channel=AF[AFcount];
				break;
			}
			if(AFcount==AF_Zaehler)
			{
				AF_trying++;
				PICODE[0]=AF_PICODE[0];
				PICODE[1]=AF_PICODE[1];
				setFM(channel);
				break;
			}
		}
		audioMute(muteOFF);
		AF_PICODE[0]=0;
		AF_PICODE[1]=0;
	}
}

/*******************************************************
*-------------------Loesche RDS-Daten------------------* 
********************************************************/
void Si4705::clearRDS (void)
{
  PSflag  = false;
  RTflag  = false;
  RDSflag = false;
  CTflag  = false;
  updateTime   = false;
  strcpy(PTY,"kein PTY  \0");
  AF_trying = 0;
  _PTY=_PTY1=_PTY2=0;
  AF_PICODE[0]=AF_PICODE[1]=0;
  oldRTcount=0;
  PICODE[0]=PICODE[1]=0;
  for(int clearRDS = 0; clearRDS <= 64; clearRDS++)
  {
	if(clearRDS<50)AF[clearRDS]=0;
    if(clearRDS<9){PSTemp[clearRDS]=0;PS[clearRDS]  = 0;}
    RT[clearRDS]=0;
    RTTemp[clearRDS]=0;
    if(clearRDS<14)RDS[clearRDS] = 0;
  }
  TC=0;
  AF_Zaehler=0;
}

/*******************************************************
*--------------Lese Daten aus dem Receiver-------------* 
********************************************************/
void Si4705::seekData (void)
{ 
  Wire.beginTransmission(Si4705_Addr);
  Wire.write(FMTuneStatus2);
  Wire.write(TuneReg);
  Wire.endTransmission();
  delay(5);

  Wire.requestFrom(Si4705_Addr, 32);
  for(int Times=0; ((Times<8)&&(Wire.available())); Times++)
  Data[Times] = Wire.read();
  RSSi      = Data[4];
  SNR       = Data[5]; 
  CHANNEL   = Data[2] << 8;
  CHANNEL  += Data[3]&0xFF;
  AFC       = Data[0];
  TUNE_CAP  = Data[7];
  delay(5);
}

void Si4705::readData (void)
{
  Wire.beginTransmission(Si4705_Addr);
  Wire.write(FMTuneStatus1);
  Wire.write(TuneReg);
  Wire.endTransmission();
  delay(5);

  Wire.requestFrom(Si4705_Addr, 32);
  for(int Times=0; ((Times<8)&&(Wire.available())); Times++)
  Data[Times] = Wire.read();
  RSSi      = Data[4];
  SNR       = Data[5];  
  FMSTEREO  = Data[3]>>7;
  STBLEND   = Data[3]&0x7F;
  OFFSET    = Data[7];
  Wire.endTransmission();
  delay(5);
}

/**********************************************************
*-----------------Vorinstantiiertes Objekt----------------*
**********************************************************/
Si4705 Radio = Si4705();
