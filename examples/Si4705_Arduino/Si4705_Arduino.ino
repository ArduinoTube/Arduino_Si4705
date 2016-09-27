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

Kompilerinfo nach dem letzten Stand (26.09.2016 / 20:30):
"Der Sketch verwendet 19.666 Bytes (64%) des Programmspeicherplatzes. Das Maximum sind 30.720 Bytes.
Globale Variablen verwenden 1.873 Bytes (91%) des dynamischen Speichers, 175 Bytes für lokale Variablen verbleiben. Das Maximum sind 2.048 Bytes.
Wenig Arbeitsspeicher verfügbar, es können Stabilitätsprobleme auftreten."
Dies Betrifft hier den Arduino Pro Mini mit ATmega328P CPU: 8MHz @3,3Volt

Wünsche euch viel Spaß mit dieser Library! Bei Problemen wendet euch bitte an mich. Kommentiert auf meinem Kanal unter dem entsprechenden Video
und bei schnellster Gelegenheit werde ich euch mit Rat und Tat bei Seite Stehen und helfen.
*/


#include <Wire.h>
#include <EEPROM.h>
#include <Si4705.h>
#include <Time.h>

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
  Wire.begin();
  Serial.begin(9600);
  Serial_writeBegin();
  EEPROM_readData();
  Radio.initRadio(Band,volume,resetPin,intPin,auxPin,FMchannel);
  Radio.setAntenna(Radio.LPI);
  Serial_printData(Band);
}

void loop() 
{
  if (Serial.available())
  {
    char ch = Serial.read();
    if (ch == 'A'){Radio.setAntenna(Radio.FMI);Serial.println("\nHeadphone Antenna activated!\n");}
    if (ch == 'a'){Radio.setAntenna(Radio.LPI);Serial.println("\nLoop Antenna activated!\n");}
    if (ch == 'u')Radio_tuneStation( UP );
    if (ch == 'd')Radio_tuneStation(DOWN);
    if (ch == 'f')Radio_tuneFrequency();
    if (ch == 'i')Serial_printData(Band);
    if (ch == '+')Radio_setVolume( UP );
    if (ch == '-')Radio_setVolume(DOWN);
//    if (ch == 'l')Radio_listFavourite();
//	  if (ch == 's')Radio_setFavourite();
    if (ch == '?')Serial_writeBegin();
//				  Radio_tuneFavourite(ch);
  }
  Radio_loopRDS();
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
      Serial.print("\nPS:");
      Serial.print(Radio.PS);
      Serial.print(" | ");
      Serial.print(Radio.PTY);
      sprintf(Buffer,"%02X%02X\0",Radio.PICODE[0],Radio.PICODE[1]);
      Serial.print("\nPI: ");
      Serial.print(Buffer);
      Radio.PSflag=false;
      
    }
    if(Radio.RTflag==true)
    {
      Serial.print("\nRT:");
      Serial.print(Radio.RT);    
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

//void Radio_tuneFavourite (char ch)
//{
//  if((ch>='0')&&(ch<='9'))
//  {
//	if(Band==FM)FMchannel=EEPROM_getFavourite(ch-'0'-1);
//	Radio_tuneStation(0);
//  }
//}

//void Radio_listFavourite (void)
//{
//  char Buffer[16];
//  Serial.println("\nYour Favourite Stations:\n");
//  for(int ls = 0; ls <= 9; ls++)
//  {
//    unsigned lsStation;
//    lsStation = EEPROM_getFavourite(ls);
//    if(Band == FM)sprintf(Buffer, "FM%2d: %3d.%02d MHz", ls+1, lsStation/100, lsStation%100);  
//    Serial.println(Buffer);
//  }
//}

//void Radio_setFavourite (void)
//{
//  Serial.println("\nSet your Favourite Number between 0...9\n");
//  while(Serial.available()==0);
//  while(Serial.available())
//  {
//  	int Station = Serial.parseInt();
//  	if((Station>=0)&&(Station<=9))
//  	{
//  	  if(Band==FM)EEMPROM_setFavourite(Station,FMchannel);
//  	  Serial.println("Done!");
//  	}
//  	else
//  	{
//  	  Serial.println("Invalid Station Number!");
//  	  Serial.println("Please Try again!");
//  	}
//  }
//  Serial_printData(Band);
//}

void Radio_setVolume (int Dir)
{
  volume += Dir;
  if (volume <= 0) volume = 0;
  if (volume >=30) volume =30;
  Radio.setVolume(volume);
  Serial.print("\nVolume: ");Serial.print(volume);
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

void Radio_tuneFrequency (void)
{
  unsigned Number=0;
  Serial.println("\nEnter Frequency: \nFor FM: 10800 --> 108.00 MHz\0");
  
  while(Serial.available()==0);								//Wait For Input...
  while(Serial.available())Number = Serial.parseInt();
  
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
  else Serial.println("Invalid Frequency!\nPlease Try Again!");  
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

//unsigned EEPROM_getFavourite (int Station)
//{
//  unsigned Temp = 0;
//  Temp += (EEPROM.read(2*Station+(22*(Band-1))+10) << 8);
//  Temp += (EEPROM.read(2*Station+(22*(Band-1))+11)&0xFF);
//  if(((Temp<8750)||(Temp>10800))&&(Band==FM))Temp=8750;
//  return (Temp);
//}

//void EEMPROM_setFavourite (int Station, unsigned channel)
//{
//  EEPROM.write(((2*Station)+(22*(Band-1))+10),channel >> 8);
//  EEPROM.write(((2*Station)+(22*(Band-1))+11),channel&0xFF);
//}

void Serial_printData (int Band)
{
  if(Band==FM){Serial.print("\nUKW: ");Serial.print(FMchannel/100.);Serial.println(" MHz");}
  delay(50);
  Radio.readData();
  Serial.print("RSSI:    ");Serial.print(Radio.RSSi);Serial.println(" dB");
  Serial.print("SNR:     ");Serial.print(Radio.SNR); Serial.println(" dB");
  Serial.print("ANT-CAP: ");Serial.print(Radio.TUNE_CAP); Serial.println(" pF");
  Serial.print("ST-BLND: ");Serial.print(Radio.STBLEND); Serial.println(" %");
  Serial.print("OFFSET:  ");Serial.print(Radio.OFFSET); Serial.println(" kHz");
  
  if(Radio.AFC==0)Serial.print("valid Channel found!\n");
  if(Radio.AFC!=0)Serial.print("AFC is tuning now...\n");
}

void Serial_writeBegin (void)
{
  Serial.println("Si4705 DSP Radio");
  Serial.println("COMMAND | FUNCTION");
  Serial.println("A / a   | Headphone / Wireantenna");
  Serial.println("+ and - | set Volume up/down");
  Serial.println("u and d | set Frequency up/down");
  Serial.println("f       | Directly Tuning"); 
  Serial.println("i       | Show any Informations");
//  Serial.println("0...9   | Your Favourite Stations");
//  Serial.println("l       | list all your Favourite Stations");
//  Serial.println("s       | set your Favourite Station");  
  Serial.println("?       | List Commands and Functions again");  
}
