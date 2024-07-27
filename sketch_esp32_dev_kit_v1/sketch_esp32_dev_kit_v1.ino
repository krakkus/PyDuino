#include <WiFi.h>
#include <regex.h>
#include <SPIFFS.h>

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
  }
  else
  {
    Serial.println("Could not connect to network, SSID: " + String(ssid));
    Serial.println("setting up access point");
    WiFi.mode(WIFI_AP); // Set mode to access point
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("Access point started, SSID: " + String(ap_ssid));
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  } else {
    Serial.println("SPIFFS mounted succesfully!");
  }
}

String requestLine;

void loop() {
  WiFiClient client = server.available(); // Check for incoming client connections
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Build a string to store the request line
        if (c != '\n') {
          requestLine += c;
        }

        // Check if the request line is complete (ends with \r\n)
        if (c == '\n') {
          Serial.println(requestLine);

          String tokens[2];
          int idx_from = 0;
          int idx_to = 0;

          idx_to = requestLine.indexOf(' ', idx_from);
          tokens[0] = requestLine.substring(idx_from, idx_to);
          //Serial.println(tokens[0]);

          idx_from = idx_to + 1;

          idx_to = requestLine.indexOf(' ', idx_from);
          tokens[1] = requestLine.substring(idx_from, idx_to);
          //Serial.println(tokens[1]);

          if (tokens[0] == "GET") {
            servePage(client, tokens[1]);
          }
          else {
            client.println("HTTP/1.1 500 Internal Server Error");
            client.println("Content-Type: text/plain");
            client.println();
            client.println("An internal server error occurred.");
          }

          // Clear the request line for the next request
          client.stop();
          requestLine = "";
        }
      }
    }
  }
}

const size_t chunkSize = 1024; // Adjust chunk size as needed
char buffer[chunkSize + 1]; // +1 for null terminator

void servePage(WiFiClient client, String filePath) {
  if (filePath == "/") filePath = "/index.html";

  String prefix = filePath.substring(1, 4);

  if (prefix == "dyn") {
    String content = getDynamicPage(client, filePath);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.print("Content-Length: ");
    client.println(content.length());
    client.println(); // Extra blank line
    client.print(content);
    return;
  }

  if (prefix == "api") {
    return;
  }

  File file = SPIFFS.open(filePath, "r");
  if (file.size() == 0) {
    Serial.println("Failed to open file: " + filePath + " for reading");

    client.println("HTTP/1.1 404 File not found");
    client.println("Content-Type: text/plain");
    client.println();
    client.println("File not found error for: " + filePath);
    return;
  }

  String ctt = "text/html";
  if (filePath.endsWith(".jpg")) ctt = "image/jpeg";
  if (filePath.endsWith(".js")) ctt = "text/javascript";
  if (filePath.endsWith(".css")) ctt = "text/css";

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
