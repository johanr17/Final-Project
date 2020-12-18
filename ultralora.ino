// ---------------------------------------------------------------- //
// Arduino Ultrasoninc Sensor HC-SR04
// Re-writed by Arbi Abdul Jabbaar
// Using Arduino IDE 1.8.7
// Using HC-SR04 Module
// Tested on 17 September 2019
// ---------------------------------------------------------------- //
#include <SoftwareSerial.h>
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
#define TX_Pin 7
#define RX_Pin 8
SoftwareSerial LoRa(RX_Pin, TX_Pin); 
#define LoRaBaud 115200

String SendString = "";    // string to hold input
String pesan="";
// defines variables
long duration; // variable for the duration of sound wave travel
long int distance; // variable for the distance measurement
long int p;
long int l;
long int t;
long int hasil;
long int delta;
int debit;
int vol;
void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  p = 7;
  l = 9;
  t = 75;
  debit=9;
  vol=999;
  LoRa_init();
  LoRa_parameter();
  delay(10000);
}
void loop() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  delta = t-distance;
  hasil = delta*p*l;
String aaa=String(debit);
aaa+=",";
aaa+=String(vol);

char coba[100];
  aaa.toCharArray(coba,100);
  for(int i=0;i<aaa.length();i++){
   String bbb = String((int)coba[i],HEX);
   pesan+=bbb;
  }
  //  Serial.println(pesan);
  delay(500);
    SendString = "mac tx ucnf 17 "+pesan;
     pesan="";
    Serial.println(SendString);
    Serial.print(sendLoRacmd(SendString , 300));
    delay(5000);

//  Serial.print("Distance: ");
//Serial.print(distance);
//  Serial.print(" cm");
//  Serial.print("\t");
//Serial.print("\t");
//  Serial.print("Delta t: ");
//Serial.print(delta);
//  Serial.print(" cm");
//Serial.print("\t");
//  Serial.print("\t");
//  Serial.print("Volume: ");
//Serial.print(debit);
//Serial.print(",");
//Serial.print(hasil);
//  Serial.println(" mL");
//  delay(5000);
////  Serial.println(" cm3");
//  
}

void LoRa_init() {
  LoRa.begin(LoRaBaud);
}

void LoRa_parameter() {

    SendString = "mac join otaa";
    Serial.print("join otaa:");
    Serial.print(sendLoRacmd(SendString ,300));

}

String sendLoRacmd(String cmd, unsigned int Dutytime) {
  String response = "";  
  LoRa.print(cmd); 
  unsigned long timeout = Dutytime + millis();
  while (LoRa.available() || millis() < timeout) {
    while (LoRa.available()) {
      //  if(millis() > timeout) break;
      char c = LoRa.read();
      response += c;
    }
  }

  //Serial.print(response);  
  return (response);
}
