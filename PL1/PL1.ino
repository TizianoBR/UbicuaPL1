#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

//Component pins
#define PEATONES_G 14
#define PEATONES_R 12
#define COCHES_G 4
#define COCHES_Y 16
#define COCHES_R 15
#define ESPERA 13
#define ZUM 33
#define BOTON 17
#define SERVO 32

// states
#define Gr 0
#define Yr 1
#define Rr1 2
#define Rg 3
#define Rr2 4

int cooldownTime=3000;
int stateTime[]={
  2000, //YrTime
  1000, //Rr1Time
  5000, //RgTime
  1000  //Rr2Time
};

unsigned long lastStateStart=0;
bool waiting=false;
int state=0;
unsigned long now=0;
bool emergencia=false;
int lastB = LOW;

Servo miServo;

#include <config.h>
#include <ESP32_Utils.hpp>
#include <MQTT.hpp>
#include <ESP32_Utils_MQTT.hpp>

void setup() {
  pinMode(COCHES_G, OUTPUT);
  pinMode(COCHES_Y, OUTPUT);
  pinMode(COCHES_R, OUTPUT);
  pinMode(PEATONES_G, OUTPUT);
  pinMode(PEATONES_R, OUTPUT);
  pinMode(ESPERA, OUTPUT);
  pinMode(ZUM, OUTPUT);
  pinMode(BOTON, INPUT);
  miServo.attach(SERVO);
  
  digitalWrite(COCHES_G, LOW);
  digitalWrite(COCHES_Y, LOW);
  digitalWrite(COCHES_R, LOW);
  digitalWrite(PEATONES_G, LOW);
  digitalWrite(PEATONES_R, LOW);
  digitalWrite(ESPERA, LOW);
  miServo.write(0);
  noTone(ZUM);

  Serial.begin(9600);
  Serial.println("Hola");

  ConnectWiFi_STA(false);
  InitMqtt();
  ConnectMqtt();
  Serial.println("Init");
  publishJson();
}

void loop() {
  HandleMqtt();
  now = millis();

  if (digitalRead(BOTON)==HIGH && lastB==LOW){
    publishJson();
  }

  //Check button
  if ((state==Gr || state==Rr2) && digitalRead(BOTON)==HIGH && !emergencia){
    waiting=true;
    digitalWrite(ESPERA, HIGH);
  }

  if (state==Gr && waiting && checkTime(now, lastStateStart, cooldownTime) && !emergencia){
    state++;
    waiting = false;
    lastStateStart=now;
    publishJson();
  }

  if (state>=Yr && (checkTime(now, lastStateStart, stateTime[state-1]))){
    state++;
    if (state>Rr2)
      state=Gr;
    lastStateStart=now;
    miServo.write(0);
    publishJson();
  }

  //Set lights
  switch(state){
    case Gr:
      lightCar('G');
      lightPasserby('R');
      break;
    case Yr:
      lightCar('Y');
      lightPasserby('R');
      break;
    case Rg:
      lightCar('R');
      lightPasserby('G');
      break;
    case Rr1:
    case Rr2:
      lightCar('R');
      lightPasserby('R');
      break;
    default:
      lightCar('Y');
      lightPasserby('G');
      break;
  }

  String str="";
  switch (state){
    case Gr:
      str="Gr";
      break;
    case Yr:
      str="Yr";
      break;
    case Rr1:
      str="Rr1";
      break;
    case Rg:
      str="Rg";
      break;
    case Rr2:
      str="Rr2";
      break;
    default:
      str="Error";
      break;
  }
  // Serial.println(str);
  // Serial.println(waiting);
  // Serial.println();
  // Serial.println(digitalRead(BOTON));
  // delay(100);

  // PublisMqtt(state);
  lastB = digitalRead(BOTON);
}

bool checkTime(unsigned long now, unsigned long base, int target){
  //Checks if now-base>=target while acounting for overflow
  unsigned long halfULong = pow(2,16);
  if (base >= halfULong){
    now-=halfULong;
    base-=halfULong;
  }
  return now-base>=(unsigned long)target;
}

void lightPasserby(char c){
  if (c=='G'){
    digitalWrite(PEATONES_G, HIGH);
    digitalWrite(PEATONES_R, LOW);
    digitalWrite(ESPERA, LOW);
    
    if (now%1000<500){
      tone(ZUM, 440);
    }
    else{
      tone(ZUM, 262);
    }

    handleServo();
  }
  else{
    digitalWrite(PEATONES_G, LOW);
    digitalWrite(PEATONES_R, HIGH);
    noTone(ZUM);
  }
}

void lightCar(char c){
  if (c=='G'){
    digitalWrite(COCHES_G, HIGH);
    digitalWrite(COCHES_Y, LOW);
    digitalWrite(COCHES_R, LOW);
  }
  else if (c=='Y'){
    if (now%1000<500){
      digitalWrite(COCHES_G, LOW);
      digitalWrite(COCHES_Y, HIGH);
      digitalWrite(COCHES_R, LOW);
    }
    else{
      digitalWrite(COCHES_Y, LOW);
      digitalWrite(COCHES_G, LOW);
      digitalWrite(COCHES_R, LOW);
    }
  }
  else{
    digitalWrite(COCHES_G, LOW);
    digitalWrite(COCHES_Y, LOW);
    digitalWrite(COCHES_R, HIGH);
  }
}

void handleServo(){
  unsigned long halfULong = pow(2,16);
  unsigned long n;
  unsigned long lss;
  if (lastStateStart>halfULong){
    n=now-halfULong;
    lss=lastStateStart-halfULong;
  } else{
    n=now;
    lss=lastStateStart;
  }

  miServo.write((double(n-lss)/stateTime[Rg-1])*180);
  // Serial.println((double(n-lss)/stateTime[Rg-1])*180);
}
