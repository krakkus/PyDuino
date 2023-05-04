#include <ESP8266WiFi.h>

const char* deviceName = "ESP8266_1"; // Configure the Arduino name here

const char* ssid = "TPLINK01";
const char* password = "0652718161";

WiFiServer server(8266); // Create a server instance on port 8266

void receiveMessage(WiFiClient client, char* message) {
  int index = 0;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c < 32 || c > 126) {
        break;
      }
      message[index++] = c;
    }
  }
  message[index++] = 0;
}

void sendMessage(WiFiClient client, char* message) {
  if (message[0] != 0) {
    client.println(message);
  }
}

void setup() {
  WiFi.begin(ssid, password); // Connect to the WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  server.begin(); // Start the server
}

void loop() {
  WiFiClient client = server.available(); // Check for incoming client connections
  if (client) {
    while (client.connected()) {
      char message_in[50] = {0};
      char message_out[50] = {0};

      receiveMessage(client, message_in);
      processMessage(message_in, message_out);
      sendMessage(client, message_out);
    }
  }
}

void processMessage(char* message_in, char* message_out) {
  message_out[0] = 0;

  char token_buffer[50] = {0};
  strcpy(token_buffer, message_in);

  char *token = strtok(token_buffer, ",\n");

  if (token != nullptr) {
    strcpy(message_out, "Error");

    if (strcmp(token, "Handshake") == 0) {
      strcpy(message_out, "Ok,");
      strcat(message_out, deviceName);
    }
    
    if (strcmp(token, "digitalWrite") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      token = strtok(nullptr, ",\n");
      int value = atoi(token);

      pinMode(pin, OUTPUT); // Set D7 pin as an output
      digitalWrite(pin, value); // Turn D7 pin high
      
      strcpy(message_out, "Ok");
    }
  }
}
