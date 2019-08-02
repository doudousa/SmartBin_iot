#include <SoftwareSerial.h>
#include <String.h>
SoftwareSerial mySerial(7, 8);               //your pins to serial communication

//-------------------------------------------------------------
//---------------------Ubidots Configuration-------------------
//-------------------------------------------------------------
String token = "YRWsXoUOptheqaDFNT58o65hyHe4m";      //your token to post a value
String idvariable = "5408256b76254210e3acafbf";      //ID of your variable
void setup()
{

  mySerial.begin(19200);                                                            //the GPRS baud rate
  Serial.begin(19200);                                                              //the serial communication baud rate
   Serial.println("Starting");
  delay(2000);
}

void loop()
{
    int value = analogRead(A0);                                                     //read pin A0 of the Arduino
    Serial.println("Sending to Ubidots Value="+ value);
    save_value(String(value));                                                      //call the save_value function
    if (mySerial.available())
    Serial.write(mySerial.read());
}
//this function is to send the sensor data to Ubidots, you can see the new value in Ubidots after running this function

void save_value(String value)
{
  int num;
  String le;
  String var;
  var="{\"value\":"+ value + "}";
  num=var.length();
  le=String(num);
  for(int i = 0;i<7;i++)
  {
    mySerial.println("AT+CGATT?");                                                   //this is better made repeatedly because it is unstable
    delay(2000);
    ShowSerialData();
  }
  mySerial.println("AT+CSTT=\"web.vmc.net.co\"");                                    //start task and set the APN
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+CIICR");                                                      //bring up wireless connection
  delay(3000);
  ShowSerialData();
  mySerial.println("AT+CIFSR");                                                      //get local IP adress
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+CIPSPRT=0");
  delay(3000);
  ShowSerialData();
  mySerial.println("AT+CIPSTART=\"tcp\",\"things.ubidots.com\",\"80\"");             //start up connection
  delay(3000);
  ShowSerialData();
  mySerial.println("AT+CIPSEND");                                                    //begin sendiing data to the remote server
  delay(3000);
  ShowSerialData();
  mySerial.print("POST /api/v1.6/variables/"+idvariable);
  delay(100);
  ShowSerialData();
  mySerial.println("/values HTTP/1.1");
  delay(100);
  ShowSerialData();
  mySerial.println("Content-Type: application/json");
  delay(100);
  ShowSerialData();
  mySerial.println("Content-Length: "+le);
  delay(100);
  ShowSerialData();
  mySerial.print("X-Auth-Token: ");
  delay(100);
  ShowSerialData();
  mySerial.println(token);
  delay(100);
  ShowSerialData();
  mySerial.println("Host: things.ubidots.com");
  delay(100);
  ShowSerialData();
  mySerial.println();
  delay(100);
  ShowSerialData();
  mySerial.println(var);
  delay(100);
  ShowSerialData();
  mySerial.println();
  delay(100);
  ShowSerialData();
  mySerial.println((char)26);
  delay(7000);
  mySerial.println();
  ShowSerialData();
  mySerial.println("AT+CIPCLOSE");                                                //close communication
  delay(1000);
  ShowSerialData();
}

void ShowSerialData()
{
  while(mySerial.available()!=0)
  Serial.write(mySerial.read());
}
