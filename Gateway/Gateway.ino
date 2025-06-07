#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#define ss 5
#define rst 14
#define dio0 2

byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;
byte Node1IsHere = false;
byte Node2IsHere = false;

String SenderNode = "";
String outgoing;
byte msgCount = 0;
String incoming = "";

unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 1;
int Secs = 0;

byte sender = LoRa.read();

int SV1;
int SV2;
int SV3;
int SV4;
int S1;
int S2;
int S3;
int S4;

const char* ssid = "ESP32_AP";
const char* password = "12345678";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Master Node");
  
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin("justmoaath")) {
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error starting mDNS");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head>";
    html += "<style>";
    html += "body { font-family: 'Courier New', monospace; background-color: #2E2E2E; color: #00FF00; text-align: center; padding: 50px; }";
    html += "h1 { font-size: 36px; margin-bottom: 20px; color: #00FF00; text-shadow: 0px 0px 10px #00FF00; }";
    html += "p { font-size: 22px; margin: 10px 0; }";
    html += "button { border: none; padding: 10px 20px; font-size: 18px; border-radius: 5px; cursor: pointer; transition: all 0.3s; }";
    html += ".on { background-color: #00FF00; color: #2E2E2E; }"; // لون أخضر عند التشغيل
    html += ".off { background-color: #FF0000; color: #FFFFFF; }"; // لون أحمر عند الإيقاف
    html += "button:hover { opacity: 0.8; }";
    html += ".status { font-weight: bold; }";
    html += ".card { background-color: #1A1A1A; border-radius: 15px; padding: 25px; box-shadow: 0px 5px 15px rgba(0, 0, 0, 0.5); max-width: 700px; margin: 40px auto; text-align: left; }";
    
    // ✅ **تنظيم الأزرار في شبكة باستخدام grid**
    html += ".button-container { display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; }"; // grid مع عمودين
    
    html += "</style>";
    
    // ✅ **جافاسكريبت لتحديث الزر والحالة ديناميكيًا**
    html += "<script>";
    html += "function toggleRelay(node) {";
    html += "  fetch('/toggle?node=' + node)";
    html += "    .then(response => response.text())";
    html += "    .then(state => {";
    html += "      let button = document.getElementById('SignalButtonS' + node);";
    html += "      let status = document.getElementById('SignalStatusS' + node);";
    html += "      status.innerText = state;";  // تحديث نص الحالة
    html += "      button.innerText = state === 'ON' ? 'Turn OFF S' + node : 'Turn ON S' + node;"; // تغيير النص
    html += "      button.className = state === 'ON' ? 'on' : 'off';"; // تغيير اللون
    html += "    });";
    html += "}";  
    html += "</script>";
    
    html += "</head><body>";

    // ✅ **Node 1**
    if(Node1IsHere) {

      html += "<div class='card'>";
      html += "<h1>Node 1</h1>";
      if(SV1 && SV2) {
        html += "<p>Temperature: <span class='status'>" + String(SV1) + "C</span></p>";
        html += "<p>Humidity: <span class='status'>" + String(SV2) + "%</span></p>";
      } if(SV3) {
        html += "<p>Methane gas ratio: <span class='status'>" + String(SV3) + "PPM</span></p>";
      } if(SV4) {
        html += "<p>Light Intensity: <span class='status'>" + String(SV4) + "%</span></p>";
      }
      html += "</div>";
    }

    // ✅ **Node 2**
    if(Node2IsHere) {
      html += "<div class='card'>";
      html += "<h1>Node 2</h1>";
  
      // **أزرار التحكم بالأجهزة الأربعة في شبكة (Grid)**
      html += "<div class='button-container'>";
      
      // **S1**
      html += "<div>";
      html += "<p>S1(Pin D4) State: <span id='SignalStatusS1' class='status'>" + String(S1 ? "ON" : "OFF") + "</span></p>";
      html += "<button id='SignalButtonS1' class='" + String(S1 ? "on" : "off") + "' onclick='toggleRelay(1)'>"
              + (S1 ? "Turn OFF S1" : "Turn ON S1") + "</button>";
      html += "</div>";
      
      // **S2**
      html += "<div>";
      html += "<p>S2(Pin D3) State: <span id='SignalStatusS2' class='status'>" + String(S2 ? "ON" : "OFF") + "</span></p>";
      html += "<button id='SignalButtonS2' class='" + String(S2 ? "on" : "off") + "' onclick='toggleRelay(2)'>"
              + (S2 ? "Turn OFF S2" : "Turn ON S2") + "</button>";
      html += "</div>";

      // **S3**
      html += "<div>";
      html += "<p>S3(Pin A7) State: <span id='SignalStatusS3' class='status'>" + String(S3 ? "ON" : "OFF") + "</span></p>";
      html += "<button id='SignalButtonS3' class='" + String(S3 ? "on" : "off") + "' onclick='toggleRelay(3)'>"
              + (S3 ? "Turn OFF S3" : "Turn ON S3") + "</button>";
      html += "</div>";
      
      // **S4**
      html += "<div>";
      html += "<p>S4(Pin A6) State: <span id='SignalStatusS4' class='status'>" + String(S4 ? "ON" : "OFF") + "</span></p>";
      html += "<button id='SignalButtonS4' class='" + String(S4 ? "on" : "off") + "' onclick='toggleRelay(4)'>"
              + (S4 ? "Turn OFF S4" : "Turn ON S4") + "</button>";
      html += "</div>";

      html += "</div>"; // إغلاق الـ button-container
      html += "</div>";
    }

    html += "</body></html>";
    request->send(200, "text/html", html);
  });


  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("node")) {
      int node = request->getParam("node")->value().toInt();
      if (node == 1) {
        S1 = !S1;
        request->send(200, "text/plain", S1 ? "ON" : "OFF");
      } else if (node == 2) {
        S2 = !S2;  
        request->send(200, "text/plain", S2 ? "ON" : "OFF");
      } else if (node == 3) {
        S3 = !S3;  
        request->send(200, "text/plain", S3 ? "ON" : "OFF");
      } else if (node == 4) {
        S4 = !S4;  
        request->send(200, "text/plain", S4 ? "ON" : "OFF");
      }
    }
  });

  server.begin();

}

void loop() {
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    Secs = Secs + 1;
    if (Secs >= 21) Secs = 0;
    if ((Secs >= 1) && (Secs <= 2)) sendMessage("10", MasterNode, Node1);
    if ((Secs >= 3) && (Secs <= 4)) sendMessage("10", MasterNode, Node2);
    if ((Secs >= 5) && (Secs <= 6) && (S1 == false)) sendMessage("0", MasterNode, Node2);
    if ((Secs >= 7) && (Secs <= 8) && (S1 == true)) sendMessage("1", MasterNode, Node2);    
    if ((Secs >= 9) && (Secs <= 10) && (S2 == false)) sendMessage("2", MasterNode, Node2);
    if ((Secs >= 11) && (Secs <= 12) && (S2 == true)) sendMessage("3", MasterNode, Node2);
    if ((Secs >= 13) && (Secs <= 14) && (S3 == false)) sendMessage("4", MasterNode, Node2);
    if ((Secs >= 15) && (Secs <= 16) && (S3 == true)) sendMessage("5", MasterNode, Node2);
    if ((Secs >= 17) && (Secs <= 18) && (S4 == false)) sendMessage("6", MasterNode, Node2);
    if ((Secs >= 19) && (Secs <= 20) && (S4 == true)) sendMessage("7", MasterNode, Node2);
    previoussecs = currentsecs;
  }
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();
  LoRa.write(otherNode);
  LoRa.write(MasterNode);
  LoRa.write(msgCount);
  LoRa.write(outgoing.length());
  LoRa.print(outgoing);
  LoRa.endPacket();
  msgCount++;
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;
  int recipient = LoRa.read();
  byte sender = LoRa.read();
  if (sender == 0XBB) SenderNode = "Node1:";
  if (sender == 0XCC) SenderNode = "Node2:";
  byte incomingMsgId = LoRa.read();
  byte incomingLength = LoRa.read();
  while (LoRa.available()) incoming += (char)LoRa.read();
  if (incomingLength != incoming.length()) return;
  if (recipient != Node1 && recipient != MasterNode) return;
  
  if (sender == 0XCC) {
    Serial.println("0XCC is sending");
    incoming = "";
    Node2IsHere = true;
  }
  if (sender == 0XBB) {
    Serial.println("0XBB is sending");
    String s = getValue(incoming, ',', 0);
    if(s == "DHT") {
      String t = getValue(incoming, ',', 1);
      String h = getValue(incoming, ',', 2);
      SV1 = t.toInt();     
      SV2 = h.toInt();
    } if(s == "MQ4") {
      String g = getValue(incoming, ',', 1);
      SV3 = g.toInt();     
    } if(s == "LDR") {
      String l = getValue(incoming, ',', 1);
      SV4 = l.toInt();     
    }
    incoming = "";
    Node1IsHere = true;
  }
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
