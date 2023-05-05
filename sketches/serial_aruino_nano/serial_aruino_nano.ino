const char* deviceName = "NANO_1"; // Configure the Arduino name here

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

      digitalWrite(pin, value); 
      
      strcpy(message_out, "Ok");
    }    

    if (strcmp(token, "digitalRead") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      int value = digitalRead(pin); 

      itoa(value, message_out, 10);  // convert the integer to a string with a base of 10
    }
        
    if (strcmp(token, "analogWrite") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      token = strtok(nullptr, ",\n");
      int value = atoi(token);

      analogWrite(pin, value); 
      
      strcpy(message_out, "Ok");
    }    

    if (strcmp(token, "analogRead") == 0) {
      token = strtok(nullptr, ",\n");
      int pin = atoi(token);

      int value = analogRead(pin); 

      itoa(value, message_out, 10);  // convert the integer to a string with a base of 10
    }
  }
}