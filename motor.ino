#include <Servo.h>
#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>

#define FIREBASE_Host "https://smarthouse-database-bc26d-default-rtdb.firebaseio.com/"  // replace with your Firebase Host
#define FIREBASE_authorization_key "JWt0XnE2t0fTPFdZVfHSr15jNcjtp5VamC1ZEOZV"           // replace with your secret key
const char* ssid = "Redmi Note 10S";
const char* password =  "qwerasdf";

Servo myservo; // переменная под мотор
String fireStatus = "";  // LED State

void setup()
{
  myservo.attach(15);
  
  pinMode(17, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected to WIFI!");
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  //send initial string of led status

  Firebase.begin(FIREBASE_Host, FIREBASE_authorization_key);
  myservo.write(0);
}

int pos = 0;

void loop()
{
  // for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
  //   // in steps of 1 degree
  //   myservo.write(pos);              // tell servo to go to position in variable 'pos'
  //   delay(15);                       // waits 15ms for the servo to reach the position
  // }
  // for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
  //   myservo.write(pos);              // tell servo to go to position in variable 'pos'
  //   delay(15);                       // waits 15ms for the servo to reach the position
  // }


 fireStatus = Firebase.getString("WindowPower");
  if (fireStatus == "false" && pos != 0){
    for (int i = pos; i >= 0; i--){
      myservo.write(i);
      delay(10);
    }
    // myservo.write(0);
    pos = 0;
  } else if (fireStatus == "true"){
    fireStatus = Firebase.getString("WindowOpenMode");
    if (fireStatus == "100" && pos != 140){
      for (int i = pos; i <= 140; i++){
      myservo.write(i);
      delay(10);
      }
      pos = 140;
    }
    if (fireStatus == "50" && pos == 0){
      for (int i = pos; i <= 70; i++){
      myservo.write(i);
      delay(10);
      }
      pos = 70;
    }
    if (fireStatus == "50" && pos == 140){
      for (int i = pos; i >= 70; i--){
      myservo.write(i);
      delay(10);
      }
      pos = 70;
    }
  }
}