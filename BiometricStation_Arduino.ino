#define USE_ARDUINO_INTERRUPTS true    // Dit Set-up een low-level interrupt voor een accurate BPM berekening.
#include <PulseSensorPlayground.h>     // Dit programma gebruikt de PulseSensorPlayground Library.   
#include <Wire.h> // Dit wordt gebruikt voor het instellen van seriële communicatie op de I2C-bus.
#include "SparkFunTMP102.h" // Dit zorgt ervoor om specifieke informatie van onze sensor te verzenden en te ontvangen
#include <SparkFun_MMA8452Q.h> // Dit programma gebruikt de SFE_MMA8452Q library
#include <LiquidCrystal.h> // Dit programma gebruikt de LiquidCrystal library.

byte heart[8] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,  
  B00000
}; // Met deze code kan er een hart worden gedisplayed.

const int PulseWire = 0;       // PulseSensor aangesloten aan de ANALOGE PIN A0.
int Threshold = 550;           // Bepaalt welk signaal er wordt gezien als een hartpulse en welke te negeren.                              
const int ALERT_PIN = A3;     // Alert PIN is aangesloten aan de ANALOGE PIN A3.

MMA8452Q accel; // Dit is om de accelerometer te initialiseren.
TMP102 sensor0(0x48); // Dit is om de sensor op I2C address 0x48 te initialiseren.
PulseSensorPlayground pulseSensor;  // Dit creeërt een instantie van het PulseSensorPlayground-object genaamd "pulseSensor"
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Hier worden de pins bepaald van de LCD.

void setup() {   

  Serial.begin(115200);          // Voor de seriale connectie baudrate van 115200 
  lcd.createChar(0, heart);      // Hier maken we een char dat een hart displayed
  lcd.begin(16, 2);              // Hier wordt de range ingesteld van de LCD
//Heartbeat 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);   // In deze 2 lijnen wordt het PulseSensor-object door onze variabelen Configureer.
 
   if (pulseSensor.begin()) { //Deze if controleerd als het object pulseSensor is gemaakt en als we een singaal binnen krijgen.
    Serial.println("");  //Hier wordt er een bericht geprint wanneer de arduino power-up of in reset is.
  }   
//Temperature
    pinMode(ALERT_PIN,INPUT);  // Hier wordt de alert pin als input geplaatst.
  
  sensor0.begin();  // In volgende lijnen code is de Temperatuur sensor geinitialiseert. 
  sensor0.setFault(0);  // 0 is zorgt ervoor dat wanneer er een fout is het alarm direct wordt getriggerd.
  sensor0.setAlertPolarity(1); // De 1 staat voor de polariteit het staat nu actief hoog.
  sensor0.setAlertMode(0); // Hier kan je instellen als je de sensor in de Comparator-modus (0) wilt of in de Onderbrekingsmodus (1). 
  sensor0.setConversionRate(2); //Hier stel je de snelheid in van de sensor hoe snel hij data moet binnenlezen je kan kiezen tussen deze 
                                //voorafgegeven waardes 0-3: 0:0.25Hz, 1:1Hz, 2:4Hz, 3:8Hz ik heb gekoren voor de gemiddelde waarde. 
  sensor0.setExtendedMode(0); // Hier een extra setting om de range te kiezen van de temperatuurs sensor 0 is voor 12bit 1 voor 13.
  sensor0.setHighTempC(40); //Hier heb ik ingestelt wanneer de themperatuur boven 30 graden is gaat het alarm aan
  sensor0.setLowTempC(28); //Hier heb ik ingestelt wanneer de themperatuur onder 26 graden is gaat het alarm af  
//accelero
  accel.init();   // Dit is om de accelerometer te starten  
}


void loop(){
  Serial.println(" "); // Dit is om een tekst lijn tussen de Begin of program tekst en eind of program te hebben.
  Serial.println("Begin of program");
  //Heartbeat
 int myBPM = pulseSensor.getBeatsPerMinute();  // Hier wordt het pulseSensor-object opgeroept en dat heeft ons een BPM terug als een int met de naam "myBPM". 
  if (pulseSensor.sawStartOfBeat()) {          // Hier wordt er steeds controleerd als er een pulse is. 
 Serial.print("This is your heartbeat : ");   // Als de test geslaagt is dan print hij de info.
 Serial.print(myBPM);                        
 Serial.println(" BPM");                        
 lcd.setCursor(0,0);                          // Hier heb ik de plaats ingesteld waar de tekst wordt gedisplayed op de LCD.
 lcd.print(char(0));                          // De char 0 display een hart
 lcd.print(":");
 lcd.print(myBPM); }
  delay(20);                                  // Deze delay zorgt ervoor dat de data niet te snel aankomt.
   
//Themperature  
  float temperature;
  boolean alertPinState, alertRegisterState;  
  sensor0.wakeup();    // Hier wordt de temperatuurs sensor gestart.
  temperature = sensor0.readTempC();   // Dit zorgt ervoor dat de temperatuur data wordt binnengelezen in °C.
  alertPinState = digitalRead(ALERT_PIN); // Hier wordt de alarm pin gecontroleerd en gelezen op de pin.
  alertRegisterState = sensor0.alert();   // Hier wordt de alarm pin gecontroleerd en gelezen in het register. 
  sensor0.sleep(); // Hier wordt er een slaap mode geinstalleerd dit zorgt voor minder verbruik.
  Serial.print("This is the temperature: ");
  Serial.print(temperature);
  Serial.println(String("°C"));
  lcd.setCursor(7,0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print((char)223);     // char(223) is om het ° teken te displayen.
  lcd.print("C");   
  delay(20);
  
  //accelero
  if (accel.available()){ //Hier wordt er gecontroller als er al data is op de accelerometer.
    accel.read(); // Als eerst een read om de data binnen te lezen.
    printCalculatedAccels(); //Hier wordt er verwijst naar 2 klasses
    printOrientation();    
    Serial.println(); // Zorgt rrvoor dat de tekst mooi van elkaar wordt onderscheiden.
  }
  delay(20); 
  Serial.println("END of program");
  delay(1000);
}

void printCalculatedAccels() // Hier wordt de X Y Z value uitgeprint
{ 
  Serial.print("This is the x value :  ");
  Serial.println(accel.cx, 2);
  lcd.setCursor(0,1);
  lcd.print(accel.cx, 2);  
  Serial.print("This is the y value :  ");
  Serial.println(accel.cy, 2);
  lcd.setCursor(6,1);
  lcd.print(accel.cy, 2);
  Serial.print("This is the z value :  ");
  Serial.println(accel.cz, 2);
  lcd.setCursor(11,1);
  lcd.print(accel.cz, 2);
  Serial.print("This is the PCD position :  ");
}
void printOrientation() // Hier wordt de positie van de accelerometer uitgeprint
{
  byte pl = accel.readPL();
  switch (pl)
  {
  case PORTRAIT_U:
    Serial.print("Landscape Left");
    break;
  case PORTRAIT_D:
    Serial.print("Landscape Right");
    break;
  case LANDSCAPE_R:
    Serial.print("Portrait Up");
    break;
  case LANDSCAPE_L:
    Serial.print("Portrait Down");
    break;
  case LOCKOUT:
    Serial.print("Flat");
    break;
  }
}
