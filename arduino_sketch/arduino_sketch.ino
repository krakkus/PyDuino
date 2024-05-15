#if defined(ESP8266)
	#include <ESP8266WiFi.h>
	#include <Servo.h>
#elif defined(ESP32)
	#include <WiFi.h>
	#include "ESP32Servo.h"
#else
	#include <Servo.h>
#endif

const char* deviceName = "NONAME_1"; // Configure the Arduino name here

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
    
    if (strcmp(token, "pinMode") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      token = strtok(nullptr, ",\n");
      int value = atoi(token);

      pinMode(pin, value);
      
      strcpy(message_out, "Ok");
    }
    
    if (strcmp(token, "digitalWrite") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      token = strtok(nullptr, ",\n");
      int value = atoi(token);

      pinMode(pin, OUTPUT);
      digitalWrite(pin, value); 
      
      strcpy(message_out, "Ok");
    }    

    if (strcmp(token, "digitalRead") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      pinMode(pin, INPUT);
      int value = digitalRead(pin); 

      itoa(value, message_out, 10);  // convert the integer to a string with a base of 10
    }
        
    if (strcmp(token, "analogWrite") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      token = strtok(nullptr, ",\n");
      int value = atoi(token);

      pinMode(pin, OUTPUT);
      analogWrite(pin, value); 
      
      strcpy(message_out, "Ok");
    }    

    if (strcmp(token, "analogRead") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      int value = analogRead(pin); 

      itoa(value, message_out, 10);  // convert the integer to a string with a base of 10
    }

    if (strcmp(token, "servoWrite") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      token = strtok(nullptr, ",\n");
      int value = atoi(token);

      if (servos[pin] == nullptr ) {
        servos[pin] = new Servo();
        servos[pin]->attach(pin);
      }

      servos[pin]->write(value);

      strcpy(message_out, "Ok");
    }

    if (strcmp(token, "stepperWrite") == 0) {
      token = strtok(nullptr, ",\n");
      int pin1 = atoi(token);

      token = strtok(nullptr, ",\n");
      int pin2 = atoi(token);

      token = strtok(nullptr, ",\n");
      int pin3 = atoi(token);

      token = strtok(nullptr, ",\n");
      int pin4 = atoi(token);

      token = strtok(nullptr, ",\n");
      int steps = atoi(token);

      token = strtok(nullptr, ",\n");
      int sleep = atoi(token);

      if (steppers[pin1] == -1) {
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
        pinMode(pin3, OUTPUT);
        pinMode(pin4, OUTPUT);   

        steppers[pin1] = 0;
      }

      moveSteps(pin1, pin2, pin3, pin4, steps, sleep);

      strcpy(message_out, "Ok");
    }
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