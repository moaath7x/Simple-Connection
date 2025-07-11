#include <SPI.h>            
#include <LoRa.h>
#include <DHT.h>
#include <avr/wdt.h>

#define DHTPIN 5        
DHT dht(DHTPIN, DHT11);
#define ss 10
#define rst 9
#define dio0 2

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte MasterNode = 0xFF;
byte Node1 = 0xBB;

float temperature;
float humidity;
int mq4Pin = A4;
int mq4Value;
int ldrPin = A2;
int ldrValue;


String Mymessage = "";
String incoming = "";

void setup() {
  Serial.begin(115200);                 
  dht.begin();

  wdt_enable(WDTO_8S);

  while (!Serial);

  Serial.println("LoRa Node1");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  mq4Value = analogRead(mq4Pin);
  ldrValue = analogRead(ldrPin);
 // Serial.println(temperature);


  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  wdt_reset();
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    // Serial.println("error: message length does not match length");
    ;
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    //Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }
  Serial.println(incoming);
  int Val = incoming.toInt();
  if (Val == 10)
  {
    Mymessage = Mymessage + "DHT" + "," + temperature + "," + humidity;
    sendMessage(Mymessage, MasterNode, Node1);
    delay(100);
    sendMessage("MQ4,"+String(mq4Value), MasterNode, Node1);
    delay(100);
    float voltage = (ldrValue / 1023.0) * 5.0;
    float percentage = (voltage / 5.0) * 100;
    sendMessage("LDR,"+String(percentage), MasterNode, Node1);
    delay(100);
    Mymessage = "";
  }
}

void sendMessage(String outgoing, byte MasterNode, byte Node1) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(MasterNode);              // add destination address
  LoRa.write(Node1);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;
  Serial.println("Message sending...");                           // increment message ID
}
