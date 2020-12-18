#include <SoftwareSerial.h>

#define TX_Pin 7
#define RX_Pin 8
SoftwareSerial LoRa(RX_Pin, TX_Pin); 
#define LoRaBaud 115200

String SendString = "";    // string to hold input
byte indikator = 13;

byte sensorInt = 0; 
byte flowsensor = 2;

float konstanta = 0.69; //konstanta flow meter

volatile byte pulseCount;

float debit;
unsigned int flowmlt;
unsigned long totalmlt;
String pesan="";
unsigned long oldTime;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(indikator, OUTPUT);
digitalWrite(indikator, HIGH); 

pinMode(flowsensor, INPUT);
digitalWrite(flowsensor, HIGH);

pulseCount = 0;
debit = 0.0;
flowmlt = 0;
totalmlt = 0;
oldTime = 0;

attachInterrupt(sensorInt, pulseCounter, FALLING);
  LoRa_init();
  LoRa_parameter();
  delay(10000);
}

void loop() {
  // put your main code here, to run repeatedly:

    if((millis() - oldTime) > 1000) 
    { 
    detachInterrupt(sensorInt);
    debit = ((1000.0 / (millis() - oldTime)) * pulseCount) / konstanta;
    oldTime = millis();
    flowmlt = (debit / 60) * 1000;
    totalmlt += flowmlt;
    
    unsigned int frac;
    
    int ccc= (int)debit;
String aaa=String(ccc);
aaa+=",";
aaa+=String(totalmlt);

char coba[100];
  aaa.toCharArray(coba,100);
  for(int i=0;i<aaa.length();i++){
   String bbb = String((int)coba[i],HEX);
   pesan+=bbb;
  }
//  Serial.println(pesan);
  delay(500);
 

    pulseCount = 0;
    
    attachInterrupt(sensorInt, pulseCounter, FALLING);
    }
    SendString = "mac tx ucnf 15 "+pesan;
     pesan="";
    Serial.println(SendString);
    Serial.print(sendLoRacmd(SendString , 300));
    delay(5000);

}

void LoRa_init() {
  LoRa.begin(LoRaBaud);
}

void LoRa_parameter() {

    SendString = "mac join otaa";
    Serial.print("join otaa:");
    Serial.print(sendLoRacmd(SendString ,300));

}

void pulseCounter()
{
// Increment the pulse counter
pulseCount++;
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
