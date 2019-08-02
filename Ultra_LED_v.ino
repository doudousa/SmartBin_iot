#include <Servo.h>
#include <SoftwareSerial.h>


#define PIN_ECHO 2
#define PIN_TRIG 3
#define PIN_LED_ROUGE 10
#define PIN_LED_ORANGE  9
#define trigPin 6
#define echoPin 5
Servo servo;
int sound = 250;
long duration_hc, distance_hc;

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(51, 50); //SIM800L Tx & Rx is connected to Arduino #3 & #2

void setup() {
  Serial.begin (9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  pinMode(PIN_LED_ROUGE, OUTPUT);
  pinMode(PIN_LED_ORANGE, OUTPUT);

  //Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.attach(11);
   //communication GSM
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  
}

void loop() {
//ouverture automatique
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 50) {
 // Serial.println("the distance is less than 5");
  servo.write(90);
   delay(50);
 }
  else {
  servo.write(0);
 }
/* if (distance > 60 || distance <= 0){
   Serial.println("The distance is more than 60");
  }
 else {
   Serial.print(distance);
   Serial.println(" cm");
 } */
  delay(500);

//detection niveau
  mesureDistance();
 /* Serial.print("Distance ");
  Serial.print(distance_hc);
  Serial.println(" Cm ");*/

  if (distance_hc > 20) {
    // aucune led allumé
    digitalWrite(PIN_LED_ROUGE, LOW);
    digitalWrite(PIN_LED_ORANGE, LOW);
  }
  else if (distance_hc > 10) {
    digitalWrite(PIN_LED_ROUGE, LOW);
    digitalWrite(PIN_LED_ORANGE, HIGH);
  }
  else if (distance_hc > 5) {
    digitalWrite(PIN_LED_ROUGE, HIGH);
    digitalWrite(PIN_LED_ORANGE, LOW);   
 mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
   //mySerial.println("AT+CPIN=\"0000\""); // Configuring TEXT mode
  //updateSerial();
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+212665668208\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("Poubelle pleine :) test réussi"); //text content
  updateSerial();
  mySerial.write(26);
    updateSerial();
  }

}

void mesureDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  duration_hc = pulseIn(PIN_ECHO, HIGH);
  Serial.println(duration_hc);
  distance_hc = duration_hc / 58;
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
