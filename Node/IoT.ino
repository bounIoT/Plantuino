#include <SPI.h>
#include <YunClient.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#define MS_PROXY "quickstart.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_MAX_PACKET_SIZE 100

//#include will be needed depending on your Ethernet shield type
// Update these with values suitable for your network.
byte mac[] = { 0x00, 0xFF, 0xBB, 0xCC, 0xDE, 0x02 };
//The convention to be followed is d:quickstart:iotsample-arduino:<MAC Address>

#define MQTT_CLIENTID "d:quickstart:iotsample-arduino:00ffbbccde02"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"

YunClient c;
IPStack ipstack(c);

MQTT::Client<IPStack, Countdown, 100, 1> client = MQTT::Client<IPStack, Countdown, 100, 1>(ipstack);

String deviceEvent;

void setup() {
  Bridge.begin();
  Console.begin(); 
  delay(2000);
}

void loop() {
  int rc = -1;
  if (!client.isConnected()) {
    Serial.println("Connecting to IoT Foundation for publishing Moisture");
    rc = ipstack.connect(( (char *) MS_PROXY), MQTT_PORT);
    
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)MQTT_CLIENTID;    
    rc = -1;
    while ((rc = client.connect(data)) != 0)
    ;
    Serial.println("Connected successfully\n");
    Serial.println("Temperature(in C)\tDevice Event (JSON)");
    Serial.println("____________________________________________________________________________");
  }

  MQTT::Message message;
  message.qos = MQTT::QOS0;
  message.retained = false;
  deviceEvent = String("{\"d\":{\"myName\":\"Arduino Yun\",\"moisture\":");
  char buffer[60];
  dtostrf(getMoisture(),1,2, buffer);
  deviceEvent += buffer;
  deviceEvent += "}}";
  Serial.print("\t");
  Serial.print(buffer);
  Serial.print("\t\t");
  deviceEvent.toCharArray(buffer, 60);
  Serial.println(buffer);
  message.payload = buffer;
  message.payloadlen = strlen(buffer);
  rc = client.publish(MQTT_TOPIC, message);

  if (rc != 0) {
    Serial.print("return code from publish was ");
    Serial.println(rc);
  }
  client.yield(1000);
}

double getMoisture(void) {
  double t;
  t = analogRead(A1);
  t = (1000-t)/10;
  return (t);
}
