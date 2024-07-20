// START OF EEPROM

int log_level = 2;    // 0 = None, 1 = Error, 2 = All
int log_serial = 1;   // Debug to serial or not

char* deviceName = "ESP32_HATCH"; // Configure the Arduino name here
char* ssid = "TPLINK01";
char* password = "0652718161";

// END OF EEPROM

#if defined(ESP8266)
	#include <ESP8266WiFi.h>
#elif defined(ESP32)
	#include <WiFi.h>
#endif

int steppers[48] = {};
String pinOwner[48];

#if defined(ESP8266) || defined(ESP32)

	WiFiServer server(8266); // Create a server instance on port 8266
  IPAddress ip;

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

  void sendMessage(WiFiClient client, String message, String log) {
    if (message.length() > 0) {
      client.println(message);
    }

    if (log_serial == 1) {
      Serial.println(log);
    }
  }

	void setup() {
    for (int i = 0; i < 48; i++) {
      steppers[i] = -1;
      pinOwner[i] = "none";
    }

    Serial.begin(115200);

    WiFi.begin(ssid, password); // Replace with your WiFi credentials
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the WiFi network");
    ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    server.begin();
	}

void loop() {
  WiFiClient client = server.available(); // Check for incoming client connections
  if (client) {
      // Print basic connection information
      Serial.print("Client connected from IP: ");
      Serial.println(client.remoteIP());
    
      // Optionally print additional details (may require library-specific functions)
      #ifdef USE_ETHERNET_LIBRARY // Check for Ethernet library availability
      if (client.connected()) {
        // Example with Ethernet library (replace with specific functions if using WiFi library)
        uint16_t remotePort = client.remotePort();
        Serial.print("Client remote port: ");
        Serial.println(remotePort);
      }
      #endif
    while (client.connected()) {
      String message_in = "";
      String message_log = "";
      String message_out = "";

      receiveMessage(client, message_in);
      processMessage(message_in, message_out, message_log);
      sendMessage(client, message_out, message_log);
    }
  }
}
	
#else

void receiveMessage(String& message) {
  while (true) {
  if (Serial.available()) {
    char c = Serial.read();
    if (c < 32 || c > 126) {
    break;
    }
    message += c;
  }
  }
}

void sendMessage(String message, String log) {
  if (message != "") {
    Serial.println(message);
  }

  if (log_serial == 1) {
      Serial.println(dbg);
  }
}

void setup() {
  for (int i = 0; i < 48; i++) {
    //servos[i] = nullptr;
    steppers[i] = -1;
    pinOwner[i] = "none";
  }

  Serial.begin(115200);
}

void loop() {
  String message_in = "";
  String message_log = "";
  String message_out = "";

  receiveMessage(message_in);
  processMessage(message_in, message_out, message_log);
  sendMessage(message_out, message_log);
}
#endif


void processMessage(String message_in, String& message_out, String& message_log) {
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

  if (log_level > 1) {
    message_log += "\nIncomming message:\n";
    message_log += ("numTokens: " + String(numTokens) + "\n");
    for (int i = 0; i < numTokens; i++) {
      message_log += (String(i) + " : " + tokens[i] + "\n");
    }
  }

  if (tokens[0] == "") {
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

    String id = (tokens[0] + "_" + String(pin));
    if (pinOwner[pin] != id) {
      if (log_level > 0) {
        message_log += "Pin " + String(pin) + " owner changed from " + pinOwner[pin] + " to " + id + "\n";
      }
      pinOwner[pin] = id;
    }

    pinMode(pin, value);
    
    message_out = "Ok";
  }
  
  if (tokens[0] == "digitalWrite") {
    int pin = tokens[1].toInt();
    int value = tokens[2].toInt();

    String id = (tokens[0] + "_" + String(pin));
    if (pinOwner[pin] != id) {
      if (log_level > 0) {
        message_log += "Pin " + String(pin) + " owner changed from " + pinOwner[pin] + " to " + id + "\n";
      }
      pinOwner[pin] = id;
      pinMode(pin, OUTPUT);
    }

    digitalWrite(pin, value); 
    
    message_out = "Ok";
  }    

  if (tokens[0] == "digitalRead") {
    int pin = tokens[1].toInt();

    String id = (tokens[0] + "_" + String(pin));
    if (pinOwner[pin] != id) {
      if (log_level > 0) {
        message_log += "Pin " + String(pin) + " owner changed from " + pinOwner[pin] + " to " + id + "\n";
      }
      pinOwner[pin] = id;
      pinMode(pin, INPUT);
    }

    int value = digitalRead(pin); 

    message_out = String(value);  // Convert int to String using String constructor
  }
      
  if (tokens[0] == "analogWrite") {
    int pin = tokens[1].toInt();
    int value = tokens[2].toInt();

    String id = (tokens[0] + "_" + String(pin));
    if (pinOwner[pin] != id) {
      if (log_level > 0) {
        message_log += "Pin " + String(pin) + " owner changed from " + pinOwner[pin] + " to " + id + "\n";
      }
      pinOwner[pin] = id;
      pinMode(pin, OUTPUT);
    }

    analogWrite(pin, value); 
    
    message_out = "Ok";
  }    

  if (tokens[0] == "analogRead") {
    int pin = tokens[1].toInt();
    
    String id = (tokens[0] + "_" + String(pin));
    if (pinOwner[pin] != id) {
      if (log_level > 0) {
        message_log += "Pin " + String(pin) + " owner changed from " + pinOwner[pin] + " to " + id + "\n";
      }
      pinOwner[pin] = id;
      pinMode(pin, INPUT);
    }

    int value = analogRead(pin); 

    message_out = String(value);  // Convert int to String using String constructor
  }
/*
  if (tokens[0] == "servoWrite") {
    int pin = tokens[1].toInt();
    int value = tokens[2].toInt();

    if (servos[pin] == nullptr ) {
      servos[pin] = new Servo();
    }

    String id = (tokens[0] + "_" + String(pin));
    if (pinOwner[pin] != id) {
      if (log_level > 0) {
        message_log += "Pin " + String(pin) + " owner changed from " + pinOwner[pin] + " to " + id + "\n";
      }
      pinOwner[pin] = id;
      servos[pin]->attach(pin);
    }

    servos[pin]->write(value);

    message_out = "Ok";
  }
*/
  if (tokens[0] == "stepperWrite_1" || tokens[0] == "stepperWrite_2") {
    int pin[4] = {};

    for (int i=0; i<4; i++) {
      pin[i] = tokens[i+1].toInt();
    }

    int steps = tokens[5].toInt();
    int sleep = tokens[6].toInt();

    for (int i=0; i<4; i++) {
      String id = (tokens[0] + "_" + String(pin[0]) + "_" + String(pin[1]) + "_" + String(pin[2]) + "_" + String(pin[3]));
      if (pinOwner[pin[i]] != id) {
        if (log_level > 0) {
          message_log += "Pin " + String(pin[i]) + " owner changed from " + pinOwner[pin[i]] + " to " + id + "\n";
        }
        pinOwner[pin[i]] = id;
        pinMode(pin[i], OUTPUT);

        steppers[pin[0]] = 0;
      }
    }

    if (tokens[0] == "stepperWrite_1") stepperWrite_1(pin[0], pin[1], pin[2], pin[3], steps, sleep);
    if (tokens[0] == "stepperWrite_2") stepperWrite_2(pin[0], pin[1], pin[2], pin[3], steps, sleep);

    message_out = "Ok";
  }
}

// Bipolar motor, 4 wires
//              pin1      pin2      pin3      pin4
// GENERIC      A+        A-        B+        B-
int stepperSequence_1[4][4] = {
  {1, 0, 0, 0},  // Step 0
  {0, 1, 0, 0},  // Step 1
  {0, 0, 1, 0},  // Step 2
  {0, 0, 0, 1},  // Step 3
};

// Function to make a certain number of steps in a specified direction
void stepperWrite_1(int pin1, int pin2, int pin3, int pin4, int steps, int sleep) {
  int sign = (steps > 0) - (steps < 0);
  // Loop for the desired number of microsteps
  for (int i = 0; i < abs(steps); i++) {
    // Get the microstep sequence index
    steppers[pin1] += sign;
    steppers[pin1] = steppers[pin1] % 4;
    if (steppers[pin1] < 0) steppers[pin1] += 4;

    // Set the coil pins based on the microstep sequence
    digitalWrite(pin1, stepperSequence_1[steppers[pin1]][0]);
    digitalWrite(pin2, stepperSequence_1[steppers[pin1]][1]);
    digitalWrite(pin3, stepperSequence_1[steppers[pin1]][2]);
    digitalWrite(pin4, stepperSequence_1[steppers[pin1]][3]);

    // Delay to control speed of rotation
    delayMicroseconds(sleep); // Adjust this delay as needed for your motor

    //Serial.print(".");
  }
}

// Unipolar, 5 wires
//                              pin1      pin2      pin3      pin4 
// GENERIC              Center  Coil1     Coil3     Coil2     Coil4
// ULN2003A     GND     VIN     IN1       IN2       IN3       IN4
// 28BYJ-48             Red     Orange    Yellow    Pink      Blue
int stepperSequence_2[4][4] = {
  {1, 1, 1, 0},  // Step 0
  {1, 1, 0, 1},  // Step 1
  {1, 0, 1, 1},  // Step 2
  {0, 1, 1, 1},  // Step 3
};

// Function to make a certain number of steps in a specified direction
void stepperWrite_2(int pin1, int pin2, int pin3, int pin4, int steps, int sleep) {
  int sign = (steps > 0) - (steps < 0);
  // Loop for the desired number of microsteps
  for (int i = 0; i < abs(steps); i++) {
    // Get the microstep sequence index
    steppers[pin1] += sign;
    steppers[pin1] = steppers[pin1] % 4;
    if (steppers[pin1] < 0) steppers[pin1] += 4;

    // Set the coil pins based on the microstep sequence
    digitalWrite(pin1, stepperSequence_2[steppers[pin1]][0]);
    digitalWrite(pin2, stepperSequence_2[steppers[pin1]][1]);
    digitalWrite(pin3, stepperSequence_2[steppers[pin1]][2]);
    digitalWrite(pin4, stepperSequence_2[steppers[pin1]][3]);

    // Delay to control speed of rotation
    delayMicroseconds(sleep); // Adjust this delay as needed for your motor

    //Serial.print(".");
  }
}
