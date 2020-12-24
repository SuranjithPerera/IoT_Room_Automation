#include <ESP8266WiFi.h> // esp8266 library
#include <FirebaseArduino.h> // firebase library


#define FIREBASE_HOST "" // the project name address from firebase id
#define FIREBASE_AUTH "" // the secret key generated from firebase
#define WIFI_SSID ""   // input your home or public wifi name 
#define WIFI_PASSWORD "" //password of wifi ssid

#include "DHT.h"






#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
String fireStatus = ""; // led status received from firebase
String lightstatus;
String exsousestatus;
String waterpumpstatus;
String automode;
float h ;

float t;


int rellay1 = D7; //light
int rellay2 = D2; //fan2
int rellay3 = D5; //fan3
int rellay4 = D6; //moter
int rellay5 = D1; //fan1
int rellay6 = D0; //exsouse





void setup() {
  dht.begin();
  pinMode(rellay1, OUTPUT);
  pinMode(rellay2, OUTPUT);
  pinMode(rellay3, OUTPUT);
  pinMode(rellay4, OUTPUT);
  pinMode(rellay5, OUTPUT);
  pinMode(rellay6, OUTPUT);

  digitalWrite(rellay1, HIGH);
  digitalWrite(rellay2, HIGH);
  digitalWrite(rellay3, HIGH);
  digitalWrite(rellay4, HIGH);
  digitalWrite(rellay5, HIGH);
  digitalWrite(rellay6, HIGH);

  Serial.begin(9600);
  delay(1000);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP()); //print local IP address
  //  WiFi.mode(WIFI_STA);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // connect to firebase

  Firebase.setString("rellay_STATUS", "fanoff");
  Firebase.setString("light_STATUS", "lightoff");
  Firebase.setFloat("temp", 0.0);
  Firebase.setFloat("hum", 0.0);
  Firebase.setString("exsouse_STATUS", "off");
   Firebase.setString("waterpump_STATUS", "off");





}

void loop() {

  
  automatic();
  lightcontrol();
 
  yield();

}
void temp() {

  delay(2000);
  h = dht.readHumidity();

//  h= 96;

    t = dht.readTemperature();
//  t = 30;

  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }


  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

    Firebase.setFloat("temp", t);
    Firebase.setFloat("hum", h);




}

void automatic() {
  automode = Firebase.getString("automode");

  if (automode == "on") {
    temp();
    Serial.println("Automode On");
    Serial.println(t);

    if (t <= 25) {
      Serial.println("t<=25");
      fanoff();
    }

    if ((t >= 25.01) && (t <= 26)) {
      speed1();
      Serial.println("t >=25.01) && (t<= 26");

    }
    if ((t >= 26.01) && (t <= 28)) {
      speed2();
      exsouse_on();
      Serial.println(F("(t >=27.01) && (t<= 28)"));
      

    }
    if ((t >= 28.01) && (t <= 40)) {
      speed3();
      exsouse_on();
      Serial.println(F("(t >=28.01) && (t<= 40)"));

    }


     if ((h >= 97) && (h <= 100) && (t >= 28.01) && (t <= 40)) {
      speed3();
      waterpump_on();
      Serial.println(F("(((h >= 95) && (h <= 100))"));
      

    }else{
      waterpump_off();
      
      }




  } else if (automode == "off") {
   
    temp();
    Serial.println(F("Automode Off"));

    fireStatus = Firebase.getString("rellay_STATUS");
    exsousestatus= Firebase.getString("exsouse_STATUS");
    waterpumpstatus = Firebase.getString("waterpump_STATUS");


    if (fireStatus == "fanspeed1") {
      speed1();
      
    }

    else if (fireStatus == "fanspeed2") {
      speed2();
      
    }

    else if (fireStatus == "fanspeed3") {
      speed3();
    }

    else if (fireStatus == "fanoff") {
      fanoff();
    }
    else if (fireStatus == "ALLOFF") {
      offall();
    }

    if (exsousestatus == "on") {
      exsouse_on();
      
    }
    if (exsousestatus == "off") {
     exsouse_off();
      
    }

    if(waterpumpstatus == "on"){
       waterpump_on();
      }

    if(waterpumpstatus == "off"){
       waterpump_off();
      }  

  }

}

void lightcontrol() {
  lightstatus = Firebase.getString("light_STATUS");

  if (lightstatus == "lighton") {
    digitalWrite(rellay1, LOW);


  } else if (lightstatus == "lightoff") {
    digitalWrite(rellay1, HIGH);

  }
}

void speed1() {
  digitalWrite(rellay3, HIGH);
  digitalWrite(rellay4, HIGH);
  digitalWrite(rellay2, LOW);
}

void speed2() {
  digitalWrite(rellay3, LOW);
  digitalWrite(rellay4, HIGH);
  digitalWrite(rellay2, HIGH);
}

void speed3() {
  digitalWrite(rellay3, HIGH);
  digitalWrite(rellay4, LOW);
  digitalWrite(rellay2, HIGH);
}

void fanoff() {
  digitalWrite(rellay3, HIGH);
  digitalWrite(rellay4, HIGH);
  digitalWrite(rellay2, HIGH);
}

void offall() {
  digitalWrite(rellay3, HIGH);
  digitalWrite(rellay4, HIGH);
  digitalWrite(rellay2, HIGH);
  digitalWrite(rellay1, HIGH);
  digitalWrite(rellay5, HIGH);
}

void exsouse_on(){
  digitalWrite(rellay6, LOW);
  }

void exsouse_off(){
  digitalWrite(rellay6, HIGH);
  }  

  void waterpump_on(){
  digitalWrite(rellay5, LOW);
  }

void waterpump_off(){
  digitalWrite(rellay5, HIGH);
  }  

 
