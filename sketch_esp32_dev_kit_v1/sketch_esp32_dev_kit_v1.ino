#include <WiFi.h>
//#include <SPIFFS.h>
#include <LittleFS.h>

const char* ssid = "TPLINK01";//"your_ssid"; // Replace with your SSID
const char* password = "Welkom01!";//your_password"; // Replace with your password

const char* ap_ssid = "MY_ESP32"; // Access point SSID
const char* ap_password = "password"; // Access point password

String wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // Set mode to station (connect to an existing network)
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() == WL_DISCONNECTED || WiFi.status() == WL_IDLE_STATUS) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi Status code: " + wl_status_to_string(WiFi.status()));

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to network, SSID: " + String(ssid));
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Could not connect to network, SSID: " + String(ssid));
    Serial.println("setting up access point");
    WiFi.mode(WIFI_AP); // Set mode to access point
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("Access point started, SSID: " + String(ap_ssid));
    Serial.print("Access point IP Address: ");
    Serial.println(WiFi.softAPIP());
  }

  server.begin();

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
  } else {
    Serial.println("SPIFFS mounted succesfully!");
  }
}

String request;

void loop() {
  WiFiClient client = server.available(); // Check for incoming client connections
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Build a string to store the request line
        if (c != '\n') {
          request += c;
        }

        // Check if the request line is complete (ends with \r\n)
        if (c == '\n') {
          Serial.println(request);

          String method = getStringBefore(request, ' ');
          request = getStringAfter(request, ' ');
          String url = getStringBefore(request, ' ');

          if (method == "GET" || method == "POST") {
            servePage(client, url);
          }
          else {
            respond(client, 500, "text/plain", "An internal server error occurred.");
          }

          // Clear the request line for the next request
          client.stop();
          request = "";
        }
      }
    }
  }
}

const size_t chunkSize = 1024; // Adjust chunk size as needed
char buffer[chunkSize + 1]; // +1 for null terminator

void servePage(WiFiClient client, String url) {
  String path = getStringBefore(url, '?');
  String parameters = getStringAfter(url, '?');

  if (path == "/") path = "/index.html";

  String ctt = "text/html";
  if (path.endsWith(".jpg")) ctt = "image/jpeg";
  if (path.endsWith(".js")) ctt = "text/javascript";
  if (path.endsWith(".css")) ctt = "text/css";
  if (path.endsWith(".json")) ctt = "application/json";
  if (path.endsWith(".ico")) ctt = "image/x-icon";

  String prefix = path.substring(1, 4);
  if (prefix == "dyn") {
    String content = getDynamicPage(client, path, parameters);
    respond(client, 200, ctt, content);
    return;
  }

  if (prefix == "api") {
    return;
  }

  File file = LittleFS.open(path, "r");
  if (file.size() == 0) {
    Serial.println("Failed to open file: " + path + " for reading");
    String content = "File not found error for: " + path;
    respond(client, 404, "text/plain", content);
    return;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: " + ctt);

  // Get file size for Content-Length header
  size_t fileSize = file.size();
  client.print("Content-Length: ");
  client.println(fileSize);
  client.println(); // Extra blank line

  const size_t chunkSize = 1024;
  char buffer[chunkSize];

  while (file.available()) {
    size_t bytesRead = file.readBytes(buffer, chunkSize);
    client.write(buffer, bytesRead);
  }

  file.close();
}
