#include <TinyGPS++.h>
char data;
 
double latitude;
double longitude;
double alt; //altitude
double vitesse;
unsigned long nbre_sat;
 
TinyGPSPlus gps;
 
void setup() {
  //Initialize serial and wait for port to open:
  Serial1.begin(9600);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}
 
void loop() {
  while (Serial1.available()) {
    data = Serial1.read();
    //  Serial.print(data);
    gps.encode(data);
    if (gps.location.isUpdated())
    {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      alt = gps.altitude.meters();
      vitesse = gps.speed.kmph();
      nbre_sat = gps.satellites.value();
 
      Serial.println("-------- FIX GPS ------------");
      Serial.print("LATITUDE="); Serial.println(latitude);
      Serial.print("LONGITUDE="); Serial.println(longitude);
      Serial.print("ALTITUDE (m) ="); Serial.println(alt);
      Serial.print("VITESSE (km/h)="); Serial.println(vitesse);
      Serial.print("NOMBRE SATTELLITES="); Serial.println(nbre_sat);
    }
  }
}
