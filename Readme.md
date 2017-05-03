******************************************SI4705 FM Radio Bibliothek für Arduino******************************************

// Diese Bibliothek dient zum Ansteuern des Si4705 ELV Radioplatine mit einem Arduino und beinhaltet       

// verschiedene Funktionen, bezüglich der RDS-Datenverarbeitung. Diese wurde meinerseits erstellt und befindet sich noch im Ausbau 

// Achtet darauf, dass diese Library eine menge Speicher benötigt und ich euch zu einem Arduino Mega                        

// Board oder noch höher rate.                                                                                              



// Version:   1.1-01022017                                                                                                  

// Ersteller: ArduinoTube                                                                                                   

// Zu den Spezifikationen:                                                                                                  



// RDS:                                                                                                                     

// - RDS Decodierung		Radio.readRDS();                                                                            

// - Sendername			Radio.decodePS();  Radio.PS	       String	8  Zeichen                                  

// - Sendertext			Radio.decodeRT();  Radio.RT	       String	64 Zeichen                                  

// - Sendertyp			Radio.decodePTY(); Radio.PTY           String	12 Zeichen                                  

// - Senderkennung (ID Code)	Radio.decodePI();  Radio.PICODE[2]     Interger Array                                       

// - AF Suche			Radio.decodeAF();  Radio.loopAF(unsigned &FMchannel);	Interger (Ref)                      

// - RTC Update			Radio.decodeCT();  Radio.CLOCK[3]; Radio.DATE[3];		  Interger Array            

// - TP / TA / EON Kennung (EON wird nur erkannt!)	Radio.decodeTC();  Radio.TC         Interger                        



// Sendersuche:                                                                                                             

// - Direkteingabe per Frequenz                                                                                             

// - Manuelle Suche (Auf- und Abwärtssuche)                                                                                 

// - Automatische Suche                                                                                                     

// Weitere Features:                                                                                                        

// - Stereo / Mono Umschaltung                                                                                              

// - Umschalten zwischen den beiden Antennen (Kopfhörer / LPI-Antenne)                                                      

// - Radiofrequenzbereich kann verstellt werden, durch Änderung folgender Werte:                                            

//   Radio.channelBTM und Radio.channelTOP (z.B.: Radio.channelBTM = 8700; und Radio.channelTOP = 10400;)                   
                                                  
// - Die I2C Adresse kann auch verstellt werden, je nach SEN Pin: Radio.Si4705_Addr = ...;                                  



// Anzeige von Parametern:                                                                                                  

// - aktuell eingestellter Sender:         Radio.CHANNEL                                                                    

// - aktuelle Signalstärke:                Radio.RSSi                                                                       

// - aktuelle Signalrauschabstand:         Radio.SNR                                                                        

// - aktueller Kapazitätswert der Antenne: Radio.TUNE_CAP                                                                   

// - aktueller Stereo/Mono Blendwert:      Radio.STBLEND                                                                    

// - aktueller Stereo/Mono Empfang:	   Radio.FMSTEREO                                                                   

// - aktueller Offset Frequenzwert AFC:    Radio.OFFSET                                                                     
                                                  
// - gültiger Sender:                      Radio.AFC&(1<<0) // == 1 Sender gültig oder   == 0 Sender nicht gültig           

// - AFC-Relais Zustand:                   Radio.AFC&(1<<1) // == 2 AFC stimmt nach oder == 0 AFC ist gerade inaktiv        



// Bei weiteren Fragen oder Problemen, kommentiert mein YouTube Video!                                                      

// Ich wünsche euch viel Spaß damit!                                                                                        



// Installation: Entpackt den Ordner Si4705_Arduino in eure Library (Pfad: C:\Users\UserName\Documents\Arduino\libraries)   

// Falls Ihr eine ältere Versionnoch habt, sichert diesen vorher ab.                                                        



// all rights reserved                                                                                                      

// ArduinoTube 2017                                                                                                         

//******************************************SI4705 FM Radio Bibliothek für Arduino******************************************
