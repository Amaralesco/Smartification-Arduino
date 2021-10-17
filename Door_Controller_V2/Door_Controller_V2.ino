#include <Bridge.h>
#include <BridgeClient.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>
/////////////ACTUATOR///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Servo.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DOOR 4
#define ACTUATOR_PIN 9

const char* mqtt_server = "broker.hivemq.com";//"broker.mqtt-dashboard.com";
const char* topicStatus = "jdSi72J29da/automatic_cabinet_door/status";
const char* topicSub = "jdSi72J29da/automatic_cabinet_door/controls";

BridgeClient yunClient;
PubSubClient client(yunClient);
Ultrasonic ultrasonic(10, 11);
int distance;
bool doorOpen=false;
int zoneTime=0;
const int delayTime = 100;//1 read every 100 milliseconds
const int openZone = 130;//130 cm
int mode = 1; //1-AUTOMATIC_MODE; 2-MANUAL_MODE; 3-OPEN_MODE;
int lastMode = 1;
bool canOpen = false;
/////////////ACTUATOR///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Servo myActuator;
int actuatorPos;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long lastMsg = 0;


/////////////ACTUATOR///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void actuatorOpen(){
  for(actuatorPos = 0; actuatorPos <= 360; actuatorPos+=1){
    myActuator.write(actuatorPos);
    delay(30); //15 ms to reach position
  }
}

void actuatorClose(){
  for(actuatorPos = 360; actuatorPos >= 0; actuatorPos-=1){
    myActuator.write(actuatorPos);
    delay(30); //15 ms to reach position
  }
}
  /*for(actuatorPos = 180; actuatorPos >= 0; actuatorPos-=1){
    myActuator.write(actuatorPos);
    delay(30); //15 ms to reach position
  }*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  char payloadChar[length+1];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  int i=0;
  for (; i < length; i++) {
    payloadChar[i] = (char)payload[i]; 
    Serial.print(payloadChar[i]);
  }
  payloadChar[i] = '\0';
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if (strcmp(payloadChar, "AUTOMATIC_MODE")==0) {
    //Serial.println("Automatic Mode Successful!");
    lastMode = mode;
    mode = 1;
  } else if(strcmp(payloadChar, "MANUAL_MODE")==0){
    lastMode = mode;
    mode = 2;
  } else if(strcmp(payloadChar, "OPEN_MODE")==0){
    lastMode = mode;
    mode = 3;
    digitalWrite(DOOR, HIGH);
    actuatorOpen();
  } else if(strcmp(payloadChar, "OPEN")==0 && mode==2 && canOpen){
    digitalWrite(DOOR, HIGH);
    actuatorOpen();
  } else if(strcmp(payloadChar, "CLOSE")==0 && mode==3){
    digitalWrite(DOOR, LOW);
    Serial.println("closing");
    actuatorClose();
    Serial.println("Closed-----");
    mode = lastMode;
  }
  canOpen = false;
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);

}

void reconnectMQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("jdSi72J29da/automatic_cabinet_door/status", "Subscriber ON");
      // ... and resubscribe
      client.subscribe("jdSi72J29da/automatic_cabinet_door/controls");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {    // Initialize the BUILTIN_LED pin as an output
   // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized

/////////////ACTUATOR///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  myActuator.attach(ACTUATOR_PIN);
  actuatorClose();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(DOOR, OUTPUT);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();

  //SerialUSB.begin(9600);

  //while (!SerialUSB); // wait for a serial connection
  
  Serial.begin(9600);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callbackMQTT);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  distance = ultrasonic.read();
  Serial.println(distance);

  
  if(mode == 1 || mode == 2){
    if(doorOpen){
      if(distance > openZone)
        zoneTime += delayTime;
      else
        zoneTime = 0;
      if(zoneTime >= 4000){
        Serial.println("Close");
        doorOpen = false;
        zoneTime = 0;
        if(mode == 2)
          canOpen = false;
        digitalWrite(4,LOW);
/////////////ACTUATOR///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        actuatorClose();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      }
    }
    else{
      if(distance < openZone)
        zoneTime += delayTime;
      else
        zoneTime = 0;
      if(zoneTime >= 1000){
        Serial.println("Open");
        doorOpen = true;
        zoneTime = 0;
        if(mode == 2)
          canOpen = true;
        else{
          digitalWrite(4,HIGH);
/////////////ACTUATOR///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          actuatorOpen();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }
      }
    }
  }

  delay(delayTime);
  /*
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("jdSi72J29da/outTopic", msg);
  }
  */
}
