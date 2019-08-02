#include <UbidotsArduinoGPRS.h>


#define TINY_GSM_MODEM_SIM800

//Libraries
#include<Servo.h> //it's used for Sevo Motor
#include <Ultrasonic.h> //it's used for ultrasonic sensor
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>

//Constantes
#define GPS_BAUD  9600
#define GPRS_BAUD 115200
#define ARDUINO_GPS_TX 3 // GPS TX, Arduino RX pin
#define ARDUINO_GPS_RX 2 // GPS RX, Arduino TX pin
#define ARDUINO_GSM_RX 7 // GSM TX, Arduino RX pin
#define ARDUINO_GSM_TX 8 // GSM RX, Arduino TX pin

//Variables
//Ouverture automatique
Servo servo;
int const trigPin = 6;//ultrasonic du servo moteur
int const echoPin = 5;

 const int Trig = 14;     // pin "Trig" du HC-SR04 connectée à pin 14 pour détecter niveau
const int Echo = 13;     // pin "Echo" du HC-SR04 connectée à pin 12 pour détecter niveau

const int LED = 1; // pin connecté à la LED + resistance

const int green = 10;   // LEDs reliées aux pins de l'Arduino via une résistance
const int yellow = 11;  // de 150 Ohms.
const int red = 12;

long cm;                // variable pour stocker la distance de l'objet en cm
long duration ;
Ultrasonic HCSR04(Trig,Echo);

//GPS+GPRS
TinyGPSPlus gps;

SoftwareSerial SerialAT(ARDUINO_GSM_RX, ARDUINO_GSM_TX); // RX, TX

SoftwareSerial gpSerial(ARDUINO_GPS_RX, ARDUINO_GPS_TX); // Create a SoftwareSerial for the GPS

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

PubSubClient mqtt(client);

double latitude, longitude;
static char lati[10], longi[10];

char data[50];

// Your GPRS credentials leave empty, if missing user or pass
// Vos informations d'identification GPRS laissez vide, s'il manque un utilisateur ou mot de passe

const char apn[]  = "";
const char user[] = "";
const char pass[] = "";

//const char* broker = "iot.eclipse.org" 
//const char* broker = "test.mosquitto.org"
const char* broker = "broker.hivemq.com";

const char* topic_GPS = "Tracker/coord";

long lastReconnectAttempt = 0;

//Fonctions
void reconnect(void);
void pubGPSInfo(float latitude, float longitude);
static void smartDelay(unsigned long ms);

 void setup()
{
  //INPUT/OUTPUT
  
  //ultrasonic+servo
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  servo.attach(4);
  
  //ultrasonic+LEDs
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
  
   //GPRS+GPS
  // port de communication avec le capteur GPS
  gpSerial.begin(GPS_BAUD);      
  delay(3000);

  // port de comunication avec le shield GPRS
  SerialAT.begin(GPRS_BAUD);    
  delay(3000);

  // initialisation du shield GPRS
  modem.restart();              
  
  // attendre la connexion au réseau de l'oprateur mobile
  while (!modem.waitForNetwork()) {
  }
  
  // wait for the Internet connection
  // attendre la connexion à Internet
  while (!modem.gprsConnect(apn, user, pass)) {
  }
  
  // Initialisation du broker MQTT
  mqtt.setServer(broker, 1883);

  
}

 void loop()
{
  //Ouverture automatique
int duration, distance;
digitalWrite(trigPin, HIGH); 
delay(1);
digitalWrite(trigPin, LOW);// Measure the pulse input in echo pin
duration = pulseIn(echoPin, HIGH);// Distance is half the duration devided by 29.1 (from datasheet)
distance = (duration/2) / 29.1;// if distance less than 0.5 meter and more than 0 (0 or less means over range) 
if (distance <= 50 && distance >= 0) 
{  
  servo.write(50);    
  delay(3000);
} 
else 
{   
  servo.write(160);
}

//Détection de niveau+envoie data au cas niveau

 //cm = HCSR04.convert(HCSR04.timing(), 1);
  /* duration = pulseIn(Echo, HIGH);
  cm = duration*0.034/2;   Autre solution*/
  cm = HCSR04.distanceRead();
  // Serial.print(distance);  Afficher la distance dans Serial Monitor
  if(cm > 0 && cm < 10)          // Il y a une présence  
  {    
  digitalWrite(green, HIGH); // Allumer la LED verte     
  delay(10); // Temps de traitement   
  }  
  else if(cm > 10 && cm < 20)          // Il y a une présence  
  {    
  digitalWrite(yellow, HIGH); // Allumer la LED jaune    
  delay(10); // Temps de traitement   
  }    
  else if(cm >= 20)  // Il n'y a pas de présence
  {
   digitalWrite(red, HIGH); // Allumer la LED rouge
   delay(10); // Temps de traitement
    
  gpSerial.listen();
  latitude = gps.location.lat();
  longitude = gps.location.lng();
  smartDelay(3000);

  SerialAT.listen();
  if (mqtt.connected()) {
    pubGPSInfo(latitude, longitude);
    mqtt.loop();
  } else {
    reconnect();
  }
  }
}

//Fonction pubGPSInfo()
void pubGPSInfo(double latitude, double longitude)
{
  dtostrf(latitude, 8, 6, lati);
  dtostrf(longitude, 8, 6, longi);
  sprintf(data, "%s,%s", lati, longi);
  mqtt.publish(topic_GPS, data);
}

//Fonction reconnect()
void reconnect(void) {
  // Loop until we're reconnected
  // Boucle jusqu'à ce que nous soyons reconnectés
  while (!mqtt.connected()) {
    // Attempt to connect
    if (mqtt.connect("oha")) {
    } else {
      // Wait 5 seconds before retrying
      // Attendez 5 secondes avant de réessayer
      delay(5000);
    }
  }
}

// This custom version of delay() ensures that the tinyGPS object
// is being "fed". From the TinyGPS++ examples.

// Cette version personnalisée de delay () garantit que l'objet tinyGPS
// est "nourri". A partir des exemples TinyGPS ++.

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    // If data has come in from the GPS module
    while (gpSerial .available())
      gps.encode(gpSerial .read()); // Send it to the encode function
    // tinyGPS.encode(char) continues to "load" the tinGPS object with new
    // data coming in from the GPS module. As full NMEA strings begin to come in
    // the tinyGPS library will be able to start parsing them for pertinent info
  } while (millis() - start < ms);
}
