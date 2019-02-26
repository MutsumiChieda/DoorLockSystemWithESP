#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>

#define SW 4
#define MO 2

#define HTML_HEADER "<!doctype html>"\
  "<html><head><meta charset=\"UTF-8\"/>"\
  "<meta name=\"viewport\" content=\"width=device-width\"/>"\
  "</head><body>"
#define HTML_FOOTER "</body></html>"

Servo sv;
const char* state;
const char* ssid = "iPhone6";
const char* password = "s2fbhkht1pko7";

//必要ならBasic認証 (If you need authentication.)
const char* www_username = "USERNAME";
const char* www_password = "PASSWORD";

int sw;
ESP8266WebServer server(80);
WiFiClient client;

void handleNotFound() {
  String message = "File Not Found\n\n";
  server.send(404, "text/plain", message);
}

void keystate() {
  String html = HTML_HEADER "<input type=\"button\" onclick=\"location.href=\'lock\'\"value=\"LOCK\">" HTML_FOOTER;
  
  server.send(200,"POST",state);
  server.send(200,"text/html",html);
}

void unlock() {
  delay(500);
  sv.write(180);
  delay(1000);
  Serial.println("");
  Serial.println("UNLOCKED");
  state = "UNLOCK";
  server.send ( 200, "POST", state);
}

void lock() {
  delay(500);
  sv.write(90);
  delay(1000);
  Serial.println("");
  Serial.println("LOCKED");
  state = "LOCK";
  server.send ( 200, "POST", state);
}

void ufn() {
  if (!server.authenticate(www_username, www_password))
    return server.requestAuthentication();
  server.send(200, "text/plain", "Login OK");
} 

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", keystate);
  server.on("/unlock", unlock);
  server.on("/lock", lock);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  sv.attach(2, 800, 2300);

  pinMode(SW, INPUT);
}

void loop(void) {
  server.handleClient();

  //スイッチの状態を調べる
  if (digitalRead(SW) == 0) {
    Serial.write("SW ON");
    if (state == "UNLOCK") {
      lock();
    } else {
      unlock();
    }
  }
}
