#include <WiFiManager.h>
#include <ArduinoJson.h>

void setPinState(int pin, int state) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, state);
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Initialize WiFiManager
  WiFiManager wifiManager;
  wifiManager.autoConnect("LineHup");  // Create an access point name

  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client;

  Serial.print("connecting to ino.moshft.shop");
  if (!client.connect("ino.moshft.shop", 80)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  // Send HTTP request
  client.print("GET /script.php HTTP/1.1\r\n");
  client.print("Host: ino.moshft.shop\r\n");
  client.print("Connection: close\r\n\r\n");

  // Wait for the response
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Read the response
  String response = client.readStringUntil('\r');
  Serial.println(response);

  // Parse JSON
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);
  
  JsonObject pins = doc[0];
  for (JsonPair pin : pins) {
    String pin_number_str = pin.key().c_str();
    int pin_number = pin_number_str.toInt();
    int pin_state = pin.value().as<int>();
    Serial.print("Pin ");
    Serial.print(pin_number);
    Serial.print(" state: ");
    Serial.println(pin_state);
    setPinState(pin_number, pin_state);
  }

  Serial.println();
  Serial.println("closing connection");
}
