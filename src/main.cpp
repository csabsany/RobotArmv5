#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <SPIFFS.h>
#include <FastLED.h>
#include <pins.h>

CRGB leds[NUM_LEDS];

// AP beállítások
const char* ssid = "Robotarm";
const char* password = "u6x48nmgu";

Servo gripperServo;
WebServer server(80);
String motorDirection = "stop";

// LED állapotfrissítés
void LED_Update(String state, int motorIndex = -1) 
{
  for (int i = 0; i < NUM_LEDS; ++i) 
  {
    leds[i] = CRGB::White; // készenlét
  }

  if (state == "moving" && motorIndex >= 0 && motorIndex < NUM_LEDS) 
  {
    leds[motorIndex] = CRGB::Green; // mozgás
  }
  else if (state == "limit" && motorIndex >= 0 && motorIndex < NUM_LEDS)
  {
    leds[motorIndex] = CRGB::Red; // végállás
  }

  FastLED.show();
}

// SPIFFS handler
void handleFile(const char* path, const char* mime) {
  File file = SPIFFS.open(path, "r");
  if (!file) {
    server.send(500, "text/plain", String(path) + " not found");
    return;
  }
  server.streamFile(file, mime);
  file.close();
}

// Webes handler-ek
void handleRoot()    { handleFile("/index.html", "text/html"); }
void handleScript()  { handleFile("/script.js", "application/javascript"); }
void handleStyle()   { handleFile("/style.css", "text/css"); }

// Motorok kezelése - START
void handleServo() {
  if (server.hasArg("dir")) 
  {
    motorDirection = server.arg("dir");
    Serial.println("[MOVE] " + motorDirection);
    String open = "grip_open";
    String close = "grip_close";

    if (motorDirection == open) 
    {
      gripperServo.write(70); // Nyitás
    } 
    else if (motorDirection == close) 
    {
      gripperServo.write(0);  // Zárás
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleStepper(int motorIndex, bool forward) {
  digitalWrite(dirPins[motorIndex], forward ? HIGH : LOW);
  digitalWrite(stepPins[motorIndex], HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPins[motorIndex], LOW);
  delayMicroseconds(1000);
}
// Motorok kezelése - END

// Ledek kezelése
void InitLeds()
{
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(10); // LED fényerő csökkentés
  FastLED.clear();
  FastLED.show();
}

void setup() {
  Serial.begin(115200);

  pinMode(LSwitch[1], INPUT_PULLUP);
  pinMode(LSwitch[2], INPUT_PULLUP);
  pinMode(LSwitch[0], INPUT_PULLUP);

  for (int i = 0; i < 3; i++) {
    pinMode(stepPins[i], OUTPUT);
    pinMode(dirPins[i], OUTPUT);
  }

  gripperServo.attach(GRIPPER_PIN);
  gripperServo.write(0);
  delay(1000);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.println("[SYSTEM] Access Point létrehozva");
  Serial.print("[SYSTEM] IP cím: ");
  Serial.println(IP);

  if (!SPIFFS.begin(true)) {
    Serial.println("[ERROR] SPIFFS inicializálása sikertelen");
    return;
  }

  // LED inicializálás
  InitLeds();
  
  server.on("/", handleRoot);
  server.on("/script.js", handleScript);
  server.on("/style.css", handleStyle);
  server.on("/move", handleServo);
  server.begin();
  Serial.println("[SYSTEM] Webszerver elindítva");
}

void loop() {
  server.handleClient();

  bool isMoving = false;

// TEST CODE

// Serial.print("LSwitch[3]: ");
// Serial.println(digitalRead(LSwitch[3]));
// delay(1000);


  if (motorDirection == "m1_left") 
  {
    if (digitalRead(LSwitch[2]) == HIGH) 
    {
      handleStepper(0, false);
      LED_Update("moving", 0);
      isMoving = true;
    }
    else
    {
      LED_Update("limit", 0);
    }
  } 
  else if (motorDirection == "m1_right") 
  {
    handleStepper(0, true);
    LED_Update("moving", 0);
    isMoving = true;
  } 
  else if (motorDirection == "m2_backward") 
  {
    handleStepper(1, false);
    LED_Update("moving", 1);
    isMoving = true;
  } 
  else if (motorDirection == "m2_forward") 
  {
    if (digitalRead(LSwitch[0]) == HIGH)
    {
      handleStepper(1, true);
      LED_Update("moving", 1);
      isMoving = true;
    }
    else
    {
      LED_Update("limit", 1);
    }
  }
  else if (motorDirection == "m3_diag2") 
  {
    if (digitalRead(LSwitch[1]) == HIGH) 
    {
      handleStepper(2, false);
      LED_Update("moving", 2);
      isMoving = true;
    }
    else
    {
      LED_Update("limit", 2);
    }
  }
  else if (motorDirection == "m3_diag1") 
  {
    handleStepper(2, true);
    LED_Update("moving", 2);
    isMoving = true;
  }
}