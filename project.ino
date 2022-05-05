#include <ESP8266WiFi.h>
#include <PubSubClient.h> // A client library for MQTT messaging
#include <Servo.h> //this arduino library is used when we deal with motors, this lib can accomodate 12 motors and extend upto 48 motors in arduino mega 
Servo pan;  //motor1
Servo tilt; //motor 2

//WiFi, MQTT parameters

const char* ssid = "realme XT";                // WiFi SSID
const char* password = "12345678";        // WiFi Password
const char* mqtt_server = "broker.mqttdashboard.com";  // IP Broker MQTT
   
#define topic_pan "servo/pan publish"
#define topic_tilt "servo/tilt publish"

   
WiFiClient espClient; //Creates a client that can connect to to a specified internet IP address and port as defined in client.connect().
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int val = 0;

void setup() {

  Serial.begin(115200); //baud rate (transfer data rate)
  pan.attach(D3);
  tilt.attach(D4);
  setup_wifi(); //wifi is connected by passing ssid and password to the begin function
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe(topic_tilt);
  client.subscribe(topic_pan);
  
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); //connection requested 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("ok "); 
}

void callback(char* topic, byte* payload, unsigned int length) {
 String string;
 //display incoming Topic
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] \n");
 //Serial.println("");
 //decode payload message
 for (int i = 0; i < length; i++) {
 string+=((char)payload[i]);
  //Serial.print((char)payload[i]);
 }
 Serial.println("");
 
  //display incoming message
 //Serial.print(string);
 Serial.print("Rotating to ");
 //convert position as an Integer
 int pos = 0;
 pos= string.toInt(); 
 Serial.print(pos);
 Serial.println(" Degree ");

 //Determines which servo should be moved
 if ( strcmp(topic, topic_pan) == 0 ) {
 Serial.print("Move Pan to ");
 Serial.println("");
 Serial.println(pos);
 pan.write(pos); 
 }
 if ( strcmp(topic, topic_tilt) == 0 ) {
 Serial.print("Move Tilt to ");
 Serial.println(pos);
 tilt.write(pos); 
 }
 delay(15); 
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected\n");
      // Once connected, publish an announcement...
      client.publish(topic_tilt, "Connected");
      client.publish(topic_pan, "Connected");
      // ... and resubscribe
      client.subscribe(topic_tilt);
      client.subscribe(topic_pan);
    }

      else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() { 

   
  
  if (!client.connected()) {
 
    reconnect();
  }
  client.loop();

}
