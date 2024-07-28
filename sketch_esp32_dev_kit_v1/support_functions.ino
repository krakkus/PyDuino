
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
