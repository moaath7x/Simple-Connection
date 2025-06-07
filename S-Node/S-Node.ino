#include <SPI.h>            
#include <LoRa.h>
#include <avr/wdt.h>

#define ss 10
#define rst 9
#define dio0 2

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte MasterNode = 0xFF;
byte Node2 = 0xCC;

int Signal;

String Mymessage = "";
String incoming = "";

void setup() {
  Serial.begin(115200);                 

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(A7, OUTPUT);

  wdt_enable(WDTO_8S);

  while (!Serial);

  Serial.println("LoRa Node2");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {

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
  if (recipient != Node2 && recipient != MasterNode) {
    //Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }
  Serial.println(incoming);
  int Val = incoming.toInt();
  if (Val == 10)
  {
    Mymessage = Mymessage;
    sendMessage(Mymessage, MasterNode, Node2);
    delay(100);
    Mymessage = "";
  }
  Signal = incoming.toInt();
  (Signal == 0) ? digitalWrite(4, LOW) :
  (Signal == 1) ? digitalWrite(4, HIGH) :
  (Signal == 2) ? digitalWrite(3, LOW) :
  (Signal == 3) ? digitalWrite(3, HIGH) :
  (Signal == 4) ? digitalWrite(A7, LOW) :
  (Signal == 5) ? digitalWrite(A7, HIGH) :
  (Signal == 6) ? digitalWrite(A6, LOW) :
  (Signal == 7) ? digitalWrite(A6, HIGH) : void();
}

void sendMessage(String outgoing, byte MasterNode, byte Node2) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(MasterNode);              // add destination address
  LoRa.write(Node2);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;
  Serial.println("Message sending...");                           // increment message ID
}
