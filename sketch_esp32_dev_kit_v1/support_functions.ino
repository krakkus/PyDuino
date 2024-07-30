
String getStringBefore(String s, char c) {
  int idx = s.indexOf(c);
  if (idx != -1) {
    return s.substring(0, idx);
  } else {
    return s;
  }
}

String getStringAfter(String s, char c) {
  int idx = s.indexOf(c);
  if (idx != -1) {
    return s.substring(idx + 1);
  } else {
    return "";
  }
}

void respond(WiFiClient client, int status, String contentType, String content) {
  client.println("HTTP/1.1 " + String(status) + " Internal Server Error");
  client.println("Content-Type: " + contentType);
  client.println();
  client.println(content);
}

String generateShortGuid() {
  randomSeed(millis());
  char guid[9]; // 8 characters + null terminator
  guid[8] = '\0'; // Ensure null termination

  for (int i = 0; i < 8; i++) {
    guid[i] = 'a' + random(26); // Generate lowercase letter (a-z)
  }

  return String(guid); // Convert character array to String for return
}

String getValueForKey(String parameters, String key) {
  while (parameters.length() > 0) {
    String kv = getStringBefore(parameters, '&');
    parameters = getStringAfter(parameters, '&');

    String k = getStringBefore(kv, '=');
    String v = getStringAfter(kv, '=');

    if (k == key) return v;
  }

  return "";
}

String urlDecode(const String& input) {
  String output;
  for (size_t i = 0; i < input.length(); i++) {
    if (input[i] == '%') {
      if (i + 2 < input.length()) {
        char hexChar[3] = {input[i + 1], input[i + 2], '\0'};
        char decodedChar = (char)strtol(hexChar, NULL, 16);
        output += decodedChar;
        i += 2;
      }
    } else if (input[i] == '+') {
      output += ' ';
    } else {
      output += input[i];
    }
  }
  return output;
}

void processFile(const char* filePath, String searchString, String replacementString) {
  File input = LittleFS.open(filePath, "r");
  if (!input) {
    Serial.println("Error opening file for reading");
    return;
  }

  String newContent; // Use a String to store the modified content

  String line;
  while (input.available()) {
    line = input.readStringUntil('\n');
    // Process line and append to newContent
    if (line.startsWith(searchString)) {
      newContent += replacementString + "\n";
    } else {
      newContent += line + "\n";
    }
  }

  input.close();

  File output = LittleFS.open(filePath, "w");
  if (!output) {
    Serial.println("Error opening file for writing");
    return;
  }

  output.print(newContent);
  output.close();
}

String getFeatureLineFor(String fid) {
  File file = LittleFS.open("/features.txt", "r");
  while (true) {
    String line = file.readStringUntil('\n');
    line.replace("\r", ""); // Remove carriage returns
    line.replace("\n", "");

    if (line.length() != 0) {
      String lid = getStringBefore(line, ' ');
      if (lid == fid) {
        file.close();
        return line;
      }
    } else {
      file.close();
      return "";
    }
  }
}
