#if defined(ESP8266)
	#include <ESP8266WiFi.h>
	#include <Servo.h>
#elif defined(ESP32)
	#include <WiFi.h>
	#include "ESP32Servo.h"
#else
	#include <Servo.h>
#endif

const char* deviceName = "ESP32_1"; // Configure the Arduino name here

const char* ssid = "TPLINK01";
const char* password = "0652718161";

Servo* servos[32] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
int steppers[32] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

// Define the microstep sequence for A+ A- B+ B-
int microstepSequence[8][4] = {
  {1, 0, 0, 0},  // Step 0
  {1, 1, 0, 0},  // Step 1
  {0, 1, 0, 0},  // Step 2
  {0, 1, 1, 0},  // Step 3
  {0, 0, 1, 0},  // Step 4
  {0, 0, 1, 1},  // Step 5
  {0, 0, 0, 1},  // Step 6
  {1, 0, 0, 1},  // Step 7
};

#if defined(ESP8266) || defined(ESP32)

	WiFiServer server(8266); // Create a server instance on port 8266

  void receiveMessage(WiFiClient client, String& message) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Check for carriage return followed by newline
        if (c == '\r' && client.available() && client.peek() == '\n') {
          client.read(); // Consume the newline character
          break;
        } else {
          // Append valid characters
          message += c;
        }
      }
    }
  }

  void sendMessage(WiFiClient client, String& message) {
    if (message.length() > 0) {
      client.println(message);
    }
  }

	void setup() {
    Serial.begin(115200);

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
      String message_in = "";
      String message_out = "";

      receiveMessage(client, message_in);
      processMessage(message_in, message_out);
      sendMessage(client, message_out);
    }
  }
}
	
#else

void receiveMessage(char* message) {
  int index = 0;
  while (true) {
  if (Serial.available()) {
    char c = Serial.read();
    if (c < 32 || c > 126) {
    break;
    }
    message[index++] = c;
  }
  }
  message[index++] = 0;
}

void sendMessage(char* message) {
  if (message[0] != 0) {
  Serial.println(message);
  }
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  char message_in[50] = {0};
  char message_out[50] = {0};

  receiveMessage(message_in);
  processMessage(message_in, message_out);
  sendMessage(message_out);
}
#endif


void processMessage(String message_in, String& message_out) {
  message_out = "";  // Clear the output message

  String tokens[10];
  int numTokens = 0;
  
  while (message_in.length()) {
    int index = message_in.indexOf(",");
    if (index == -1) {
      tokens[numTokens++] = message_in;
      message_in = "";
    }
    else {
      String before = message_in.substring(0, index);
      String after = message_in.substring(index + 1);
      tokens[numTokens++] = before;
      message_in = after;
    }
  }

  Serial.println();
  Serial.println("=========== INCOMMING MESSAGE ===========");
  Serial.print("numTokens: ");
  Serial.println(numTokens);
  for (int i = 0; i < numTokens; i++) {
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(tokens[i]);  // Print each string with a newline
  }

  if (tokens[0] == nullptr) {
    message_out = "Error";
    return;
  }

  if (tokens[0] == "Handshake") {
    message_out = "Ok,";
    message_out += deviceName;
  }
  
  if (tokens[0] == "pinMode") {
    int pin = tokens[1].toInt();
    int value = tokens[2].toInt();
    pinMode(pin, value);
    
    message_out = "Ok";
  }
  
  if (tokens[0] == "digitalWrite") {
    int pin = tokens[1].toInt();
    int value = tokens[2].toInt();
    pinMode(pin, OUTPUT);
    digitalWrite(pin, value); 
    
    message_out = "Ok";
  }    

  if (tokens[0] == "digitalRead") {
    int pin = tokens[1].toInt();
    pinMode(pin, INPUT);
    int value = digitalRead(pin); 

    message_out = String(value);  // Convert int to String using String constructor
  }
      
  if (tokens[0] == "analogWrite") {
    int pin = tokens[1].toInt();
    int value = tokens[2].toInt();

    pinMode(pin, OUTPUT);
    analogWrite(pin, value); 
    
    message_out = "Ok";
  }    

  if (tokens[0] == "analogRead") {
    int pin = tokens[1].toInt();

    pinMode(pin, INPUT);
    int value = analogRead(pin); 

    message_out = String(value);  // Convert int to String using String constructor
  }

  if (tokens[0] == "servoWrite") {
    int pin = tokens[1].toInt();
    int value = tokens[2].toInt();

    if (servos[pin] == nullptr ) {
      servos[pin] = new Servo();
      servos[pin]->attach(pin);
    }

    servos[pin]->write(value);

    message_out = "Ok";
  }

  if (tokens[0] == "stepperWrite") {
    int pin1 = tokens[1].toInt();
    int pin2 = tokens[2].toInt();
    int pin3 = tokens[3].toInt();
    int pin4 = tokens[4].toInt();
    int steps = tokens[5].toInt();
    int sleep = tokens[6].toInt();

    if (steppers[pin1] == -1) {
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
      pinMode(pin3, OUTPUT);
      pinMode(pin4, OUTPUT);   

      steppers[pin1] = 0;
    }

    moveSteps(pin1, pin2, pin3, pin4, steps, sleep);

    message_out = "Ok";
  }
}

// Function to make a certain number of steps in a specified direction
void moveSteps(int pin1, int pin2, int pin3, int pin4, int steps, int sleep) {
  int sign = (steps > 0) - (steps < 0);
  // Loop for the desired number of microsteps
  for (int i = 0; i < abs(steps); i++) {
    // Get the microstep sequence index
    steppers[pin1] += sign;
    steppers[pin1] = steppers[pin1] % 16;
    if (steppers[pin1] < 0) steppers[pin1] += 16;

    // Set the coil pins based on the microstep sequence
    digitalWrite(pin1, microstepSequence[steppers[pin1]][0]);
    digitalWrite(pin2, microstepSequence[steppers[pin1]][1]);
    digitalWrite(pin3, microstepSequence[steppers[pin1]][2]);
    digitalWrite(pin4, microstepSequence[steppers[pin1]][3]);

    // Delay to control speed of rotation
    delayMicroseconds(sleep); // Adjust this delay as needed for your motor
  }
}