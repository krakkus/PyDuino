
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
