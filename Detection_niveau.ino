#define PIN_ECHO 2
#define PIN_TRIG 3
#define PIN_LED_ROUGE 10
#define PIN_LED_VERTE  9

int sound = 250;
long duration_hc, distance_hc;

void setup() {
  Serial.begin (9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  pinMode(PIN_LED_ROUGE, OUTPUT);
  pinMode(PIN_LED_VERTE, OUTPUT);
}

void loop() {
  mesureDistance();
 Serial.print("Distance ");
  Serial.print(distance_hc);
  Serial.println(" Cm ");

  if (distance_hc > 10) {
    // aucune led allumé
    digitalWrite(PIN_LED_ROUGE, LOW);
    digitalWrite(PIN_LED_VERTE, LOW);
  }
  else if (distance_hc < 10  && distance_hc > 5 ) {
    digitalWrite(PIN_LED_ROUGE, LOW);
    digitalWrite(PIN_LED_VERTE, HIGH);
  }
  else if (distance_hc < 5 && distance_hc > 0) {
    digitalWrite(PIN_LED_ROUGE, HIGH);
    digitalWrite(PIN_LED_VERTE, LOW);   
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
