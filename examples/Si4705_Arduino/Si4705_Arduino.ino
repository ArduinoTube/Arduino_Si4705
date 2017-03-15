/*
 Si4705 ELV Software build by ArduinoTube (Savas Saffak) Betaversion!
 
 Erweiterte RDS Dekodierung:    PS (Sendername), RT (Radiotext), PTY (Sendertyp), 
                                CT (Uhrzeit und Datumssynchronisierung via RDS), 
                                AF (Alternativfrequenz) inkl. Suchlaufroutine in Library eingefügt "Verbesserungswürdig"
                                PI (Programm-Identifikation)

 Veränderte Empfangseigensch.:  Angepasste Stereo/Mono-Blend Werte bezüglich: RSSI und SNR
                                Angepasste Hi-Cut Werte bezüglich:            RSSI, SNR, Grenz- und Durchlassfrequenz (6 kHz, kann beliebig verändert werden)
                                Eingeschalteter Channelfilter für bessere Trennung der Sender, Grenzfrequenz (Min. 40 kHz, kann beliebig verändert werden)
                                ::                            Diese Anpassungen wurden nach dem Programmierheft: AN332 durchgeführt                        ::
Angezeie von Empfangsdaten:     SNR in dB, RSSI in dBµV, Stereoblend in %, AFC-Rail, Offset-Frequenz in kHz

Pinbelegung:
I2C Datenleitungen je nach Arduinoplatine entsprechend anschließen
Resetpin: 12
Intpin:   11
Auxpin:   10

Achtet darauf, dass die Library einiges an Resource braucht:

Kompilerinfo nach dem letzten Stand (02.02.2017 / 19:06):
"Der Sketch verwendet 19.248 Bytes (7%) des Programmspeicherplatzes. Das Maximum sind 253.952 Bytes.
Globale Variablen verwenden 2.419 Bytes (29%) des dynamischen Speichers, 5.773 Bytes für lokale Variablen verbleiben. Das Maximum sind 8.192 Bytes."
Dies Betrifft hier den Arduino Mega 2560

Wünsche euch viel Spaß mit dieser Library! Bei Problemen wendet euch bitte an mich. Kommentiert auf meinem Kanal unter dem entsprechenden Video
und bei schnellster Gelegenheit werde ich euch mit Rat und Tat bei Seite Stehen und helfen.
*/


#include <Wire.h>
#include <EEPROM.h>
#include <Si4705.h>
#include <TimeLib.h>

//Variablen
int       Band;
unsigned  FMchannel     = 8750;
int       volume        = 10;

//Konstanten
const int FM            = 1;
const int resetPin      = 12;
const int intPin		    = 11;
const int auxPin		    = 10;
const int UP			      = 1;
const int DOWN          =-1;

const int FMchanHIEEP   = 1;
const int FMchanLOEEP   = 2;
const int RadioBandEEP  = 5;
const int volumeEEP     = 6;
//EEPROM 10...40 reserved for Favourite Stations

void setup() 
{
  Serial_writeBegin();
  EEPROM_readData();
  Radio_init();
  Serial_printData(Band);
}

void Radio_init (void)
{
  Radio.initRadio(Band,volume,resetPin,intPin,auxPin,FMchannel);
  Radio.forceMono(false);
  Radio.setAntenna(Radio.LPI);  
}

void loop() 
{
  if (Serial.available())
  {
    char ch = Serial.read();
    if (ch == 'A'){Radio.setAntenna(Radio.FMI);Serial.println(F("\nHeadphone Antenna activated!\n"));}
    if (ch == 'a'){Radio.setAntenna(Radio.LPI);Serial.println(F("\nLoop Antenna activated!\n"));}
    if (ch == 'u')Radio_tuneStation( UP );
    if (ch == 'd')Radio_tuneStation(DOWN);
	  if (ch == 'w')Radio_seekStation( UP );
	  if (ch == 'q')Radio_seekStation(DOWN);
	  if (ch == 's'){Radio.forceMono(false);Serial.println(F("Stereo"));}
	  if (ch == 'm'){Radio.forceMono(true);Serial.println(F("Mono"));}
    if (ch == 't')Radio_seekAF();
    if (ch == 'f')Radio_tuneFrequency();
    if (ch == 'i')Serial_printData(Band);
    if (ch == '+')Radio_setVolume( UP );
    if (ch == '-')Radio_setVolume(DOWN);
    if (ch == 'l')Radio_listFavourite();
	  if (ch == 'x')Radio_setFavourite();
    if (ch == '?')Serial_writeBegin();
				  Radio_tuneFavourite(ch);
  }
  Radio_loopRDS();
  if(second()%5==0)Radio.autoChFilter();
}

void Radio_loopRDS (void)
{
  char Buffer[20];
  if(Band==FM)
  {
    Radio.readRDS();
    Radio.decodeRT();
    Radio.decodePS();
    Radio.decodePTY();
    Radio.decodeAF();
    Radio.decodeCT();
    Radio.decodePI();
    if(Radio.PSflag==true)
    {
      Serial.print(F("\nPS:"));
      Serial.print(Radio.PS);
      Serial.print(F(" | "));
      Serial.print(Radio.PTY);
      sprintf(Buffer,"%02X%02X\0",Radio.PICODE[0],Radio.PICODE[1]);
      Serial.print(F("\nPI: "));
      Serial.println(Buffer);
      Radio.PSflag=false;
      
    }
    if(Radio.RTflag==true)
    {
      Serial.print(F("RT:"));
      Serial.println(Radio.RT);    
      Radio.RTflag=false;
    }
    if(Radio.CTflag==true)
    {
      setTime(Radio.CLOCK[0],Radio.CLOCK[1],0,Radio.DATE[0],Radio.DATE[1],Radio.DATE[2]);
      Radio.CTflag=false;
      Radio.updateTime=true;
      sprintf(Buffer,"\nClock:%02d:%02d\0",hour(),minute());
      Serial.println(Buffer);
	  sprintf(Buffer,"Date:%02d.%02d.%04d\n\0",day(),month(),year());
	  Serial.println(Buffer);
    }
  }  
}

void Radio_tuneFavourite (char ch)
{
  if((ch>='0')&&(ch<='9'))
  {
	if(Band==FM)FMchannel=EEPROM_getFavourite(ch-'0'-1);
	Radio_tuneStation(0);
  }
}

void Radio_listFavourite (void)
{
  char Buffer[16];
  Serial.println(F("\nYour Favourite Stations:\n"));
  for(int ls = 0; ls <= 9; ls++)
  {
    unsigned lsStation;
    lsStation = EEPROM_getFavourite(ls);
    if(Band == FM)sprintf(Buffer, "FM%2d: %3d.%02d MHz", ls+1, lsStation/100, lsStation%100);  
    Serial.println(Buffer);
  }
}

void Radio_setFavourite (void)
{
  Serial.println(F("\nSet your Favourite Number between 0...9\n"));
  while(Serial.available()==0);
  while(Serial.available())
  {
  	int Station = Serial.parseInt();
  	if((Station>=0)&&(Station<=9))
  	{
  	  if(Band==FM)EEMPROM_setFavourite(Station,FMchannel);
  	  Serial.println(F("Done!"));
  	}
  	else
  	{
  	  Serial.println(F("Invalid Station Number!"));
  	  Serial.println(F("Please Try again!"));
  	}
  }
  Serial_printData(Band);
}

void Radio_setVolume (int Dir)
{
  volume += Dir;
  if (volume <= 0) volume = 0;
  if (volume >=30) volume =30;
  Radio.setVolume(volume);
  Serial.print(F("\nVolume: "));Serial.print(volume);
  delay(100);
//  Serial_printData(Band); 
  EEPROM.write(volumeEEP,volume);
}

void Radio_tuneStation (signed Dir)
{
  if(Band==FM)
  {
    if(Dir)FMchannel+=5*Dir;
    if(FMchannel>10800)FMchannel=8750;
    if(FMchannel<8750) FMchannel=10800;
    Radio.setFM(FMchannel);
    Radio.clearRDS();
    Serial_printData(Band);
	EEPROM.write(FMchanHIEEP,FMchannel >> 8);
	EEPROM.write(FMchanLOEEP,FMchannel&0xFF);
  }
}

void Radio_seekStation (signed Dir)
{
  if (Band == FM)
  {
    Serial.println(F("Suchen.."));delay(500);
    Radio.seekAuto(Dir,FMchannel);
    EEPROM.write(FMchanHIEEP, FMchannel >> 8);
    EEPROM.write(FMchanLOEEP, FMchannel & 0xFF);
    Serial_printData(Band);
  }
}

void Radio_seekAF (void)
{
  if(Radio.AF[0])
  {
    Radio.loopAF(FMchannel);
    Serial_printData(Band);
  }
  else Serial.println(F("No AF available yet! Please later try again..."));
}

void Radio_tuneFrequency (void)
{
  unsigned Number=0;
  Serial.println(F("\nEnter Frequency: \nFor FM: 10800 --> 108.00 MHz\0"));
  
  while(Number==0)                //Wait For Input...
  if(Serial.available())Number = Serial.parseInt();
  
  if((Number>8750)&&(Number<10800)&&(Band==FM))
  {
    if(Band==FM)
    {
	  FMchannel=Number;
	  Radio.setFM(FMchannel);
	  Radio.clearRDS();
	  EEPROM.write(FMchanHIEEP,FMchannel >> 8);
	  EEPROM.write(FMchanLOEEP,FMchannel&0xFF);
	}
    Serial_printData(Band);          
  }
  else Serial.println(F("Invalid Frequency!\nPlease Try Again!"));  
}

void EEPROM_readData (void)
{
  FMchannel = EEPROM.read(FMchanHIEEP) << 8;
  FMchannel+= EEPROM.read(FMchanLOEEP)&0xFF;
  Band      = EEPROM.read(RadioBandEEP);
  volume    = EEPROM.read(volumeEEP);
  
  if((FMchannel>10800)||(FMchannel<8750))FMchannel=8750;
  
  if (Band!=FM)                  Band    = FM;
  if (volume>30)                volume   = 0;
}

unsigned EEPROM_getFavourite (int Station)
{
  unsigned Temp = 0;
  Temp += (EEPROM.read(2*Station+(22*(Band-1))+10) << 8);
  Temp += (EEPROM.read(2*Station+(22*(Band-1))+11)&0xFF);
  if(((Temp<8750)||(Temp>10800))&&(Band==FM))Temp=8750;
  return (Temp);
}

void EEMPROM_setFavourite (int Station, unsigned channel)
{
  EEPROM.write(((2*Station)+(22*(Band-1))+10),channel >> 8);
  EEPROM.write(((2*Station)+(22*(Band-1))+11),channel&0xFF);
}

void Serial_printData (int Band)
{
  if(Band==FM){Serial.print(F("\nUKW: "));Serial.print(FMchannel/100.);Serial.println(F(" MHz"));}
  delay(50);
  Radio.readData();
  if(Radio.FMSTEREO)Serial.println(F("STEREO"));
  else              Serial.println(F(" MONO "));
  Serial.print(F("RSSI:    "));Serial.print(Radio.RSSi);Serial.println(F(" dB"));
  Serial.print(F("SNR:     "));Serial.print(Radio.SNR); Serial.println(F(" dB"));
  Serial.print(F("ANT-CAP: "));Serial.print(Radio.TUNE_CAP); Serial.println(F(" pF"));
  Serial.print(F("ST-BLND: "));Serial.print(Radio.STBLEND); Serial.println(F(" %"));
  Serial.print(F("OFFSET:  "));Serial.print(Radio.OFFSET); Serial.println(F(" kHz"));
  
  if(Radio.AFC==0)Serial.print(F("valid Channel found!\n"));
  if(Radio.AFC!=0)Serial.print(F("AFC is tuning now...\n"));
}

void Serial_writeBegin (void)
{
  Serial.begin(9600);
  Serial.println(F("Si4705 DSP Radio"));
  Serial.println(F("COMMAND | FUNCTION"));
  Serial.println(F("A / a   | Headphone / Wireantenna"));
  Serial.println(F("+ and - | set Volume up/down"));
  Serial.println(F("u and d | set Frequency up/down"));
  Serial.println(F("q and w | seek auto for a next available Station"));
  Serial.println(F("s and m | switch between stereo and mono reception"));
  Serial.println(F("t       | tune for a alternative frequency by RDS"));
  Serial.println(F("f       | Directly Tuning")); 
  Serial.println(F("i       | Show any Informations"));
  Serial.println(F("0...9   | Your Favourite Stations"));
  Serial.println(F("l       | list all your Favourite Stations"));
  Serial.println(F("x       | set your Favourite Station"));  
  Serial.println(F("?       | List Commands and Functions again"));  
}
