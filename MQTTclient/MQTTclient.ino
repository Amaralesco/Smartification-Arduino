#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

#define D1 5
/*
#define WAKE_UP_KITCHEN_PIN 16 //D0
#define AUTOMATIC_MODE_PIN 5  //D1
#define MANUAL_MODE_PIN 4  //D2
#define OPEN_MODE_PIN 0  //D3
#define OPEN_PIN 2  //D4
#define CLOSE_PIN 14  //D5
*/
const char *wifi_ssid = "dlink_DWR-932_acordados";//"papamapa-trabalho";//"MEO-9AB8F0";
const char *wifi_password = "RMovel456#";//"pedro+miguel+clara+antonio";//"6390a3fe34";
const char* mqtt_server = "broker.hivemq.com";//"broker.mqtt-dashboard.com";
//const char* cloud_username = "voice_module";
//const char* cloud_password = "jdSi72J29da";
const char* topicStatus = "jdSi72J29da/voice_controller/status";
const char* acdTopicPub = "jdSi72J29da/automatic_cabinet_door/controls";
const char* eslTopicPub = "jdSi72J29da/embedded_smart_lighting/controls";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];


/***************************************************************************************************************************************/
/*************************************************************DECODER*******************************************************************/
/***************************************************************************************************************************************/
const char* decodeCommand[] = {"WAKE_UP_KITCHEN","AUTOMATIC_MODE","MANUAL_MODE", "OPEN_MODE","OPEN", "CLOSE", "WINE", "GIN", "COOKIES"};




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup_wifi() {

  randomSeed(micros());
  
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(F("\n\rWiFi connected!"));
}

/*
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}
*/

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if (client.connect(clientId.c_str(),cloud_username,cloud_password)) {
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(topicStatus, "Publisher ON");
      // ... and resubscribe
      //client.subscribe("jdSi72J29da/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(D1,INPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 100) {
    if(digitalRead(D1)==1){//1
      int command = 0;
      delay(100);
      if(digitalRead(D1)==1){//2
        delay(100);
        if(digitalRead(D1)==1){//3
          delay(100);
          command = 1;
          if(digitalRead(D1)==1){//4
            delay(100);
            if(digitalRead(D1)==1){//5
              delay(100);
              command = 2;
              if(digitalRead(D1)==1){//6
                delay(100);
                if(digitalRead(D1)==1){//7
                  delay(100);
                  command = 3;
                  if(digitalRead(D1)==1){//8
                    delay(100);
                    if(digitalRead(D1)==1){//9
                      delay(100);
                      command = 4;
                      if(digitalRead(D1)==1){//10
                        delay(100);
                        if(digitalRead(D1)==1){//11
                          delay(100);
                          command = 5;
                          if(digitalRead(D1)==1){//12
                            delay(100);
                            if(digitalRead(D1)==1){//13
                              delay(100);
                              command = 6;
                              if(digitalRead(D1)==1){//14
                                delay(100);
                                if(digitalRead(D1)==1){//15
                                  delay(100);
                                  command = 7;
                                  if(digitalRead(D1)==1){//16
                                    delay(100);
                                    if(digitalRead(D1)==1){//17
                                      delay(100);
                                      command = 8;
                                      delay(100);
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      Serial.println(decodeCommand[command]);
      if(command == 0)
        client.publish(topicStatus, decodeCommand[0]);
      else if(command <=5 )//Automatic Cabinet Door
        client.publish(acdTopicPub, decodeCommand[command]);
      else                 //Embedded Smart Lighting
        client.publish(eslTopicPub, decodeCommand[command]);
    }
    lastMsg = now;
    

  }
}
