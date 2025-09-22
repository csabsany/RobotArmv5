#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <SPIFFS.h>
#include <FastLED.h>

#define LED_PIN 21
#define NUM_LEDS 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// AP beállítások
const char* ssid = "Robotarm";
const char* password = "u6x48nmgu";

// Stepper motorok 
const int dirPins[3] = {2, 13, 11};
const int stepPins[3] = {1, 14, 12};
const int MOTOR_RESET = 39;

// Végálláskapcsolók
const int LSwitch1 = 42; 
const int LSwitch2 = 41; 
const int LSwitch3 = 40; 

// Szervó motor (gripper)
const int gripperPin = 38;

Servo gripperServo;
WebServer server(80);
String motorDirection = "stop";

// LED állapotfrissítés
void LED_Update(String state, int motorIndex = -1) 
{
  for (int i = 0; i < NUM_LEDS; ++i) 
  {
    leds[i] = CRGB::White; //készenlét
  }

  if (state == "moving" && motorIndex >= 0 && motorIndex < NUM_LEDS) 
  {
    leds[motorIndex] = CRGB::Green; //Az éppen mozgó motor színe
  }

  FastLED.show();
}

// Webes részek kezelése - START
void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(500, "text/plain", "index.html not found");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

void handleScript() {
  File file = SPIFFS.open("/script.js", "r");
  if (!file) {
    server.send(500, "text/plain", "script.js not found");
    return;
  }
  server.streamFile(file, "application/javascript");
  file.close();
}

void handleStyle() {
  File file = SPIFFS.open("/style.css", "r");
  if (!file) {
    server.send(500, "text/plain", "style.css not found");
    return;
  }
  server.streamFile(file, "text/css");
  file.close();
}
// Webes részek kezelése - END

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
      gripperServo.write(90); // Nyitás
    } 
    else if (motorDirection == close) 
    {
      gripperServo.write(0);  // Zárás
    }
  }
  server.send(200, "text/plain", "OK");
}

void moveMotor(int motorIndex, bool forward) {
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
  FastLED.setBrightness(10); //LED fényerő csökkentés
  FastLED.clear();
  FastLED.show();
}

void setup() {
  Serial.begin(115200);

  pinMode(LSwitch1, INPUT_PULLUP);
  pinMode(LSwitch2, INPUT_PULLUP);
  pinMode(LSwitch3, INPUT_PULLUP);

  for (int i = 0; i < 3; i++) {
    pinMode(stepPins[i], OUTPUT);
    pinMode(dirPins[i], OUTPUT);
  }

  gripperServo.attach(gripperPin);
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

  

if (motorDirection == "m1_left") 
{
  if (digitalRead(LSwitch2) == HIGH) // NINCS benyomva, lehet léptetni
  {
    moveMotor(0, false);
    LED_Update("moving", 0);
    isMoving = true;
  }
  else
  {
    LED_Update("ready");
    //motorDirection = "stop";
  }
} 
else if (motorDirection == "m1_right") 
{
  moveMotor(0, true);
  LED_Update("moving", 0);
  isMoving = true;
} 
else if (motorDirection == "m2_forward") 
{
  if (digitalRead(LSwitch1) == HIGH) // NINCS benyomva, lehet léptetni
  {
    moveMotor(1, true);
    LED_Update("moving", 1);
    isMoving = true;
  }
  else
  {
    LED_Update("ready");
    //motorDirection = "stop";
  }
} 
else if (motorDirection == "m2_backward") 
{
  moveMotor(1, false);
  LED_Update("moving", 1);
  isMoving = true;
} 
else if (motorDirection == "m3_diag1") 
{
  if (digitalRead(LSwitch3) == HIGH) // NINCS benyomva, lehet léptetni
  {
    moveMotor(2, false);
    LED_Update("moving", 2);
    isMoving = true;
  }
  else
  {
    LED_Update("ready");
    //motorDirection = "stop";
  }
} 
else if (motorDirection == "m3_diag2") 
{
  moveMotor(2, true);
  LED_Update("moving", 2);
  isMoving = true;
} 
else 
{
  LED_Update("ready");
}


}