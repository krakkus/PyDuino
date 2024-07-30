
String getAPIPage(WiFiClient client, String path, String parameters) {
  String content;

  if (path == "/api_digital_output_high.html") {
    String fid = getValueForKey(parameters, "id");

    String line = getFeatureLineFor(fid);
    line = getStringAfter(line, ' ');
    String fname = urlDecode(getStringBefore(line, ' '));
    line = getStringAfter(line, ' ');
    // Type
    line = getStringAfter(line, ' ');
    String fpin = urlDecode(getStringBefore(line, ' '));

    pinMode(fpin.toInt(), OUTPUT);
    digitalWrite(fpin.toInt(), 1);

    content = "OK, Pin " + fpin + " set to HIGH";
    return content;
  }

  if (path == "/api_digital_output_low.html") {
    String fid = getValueForKey(parameters, "id");

    String line = getFeatureLineFor(fid);
    line = getStringAfter(line, ' ');
    String fname = urlDecode(getStringBefore(line, ' '));
    line = getStringAfter(line, ' ');
    // Type
    line = getStringAfter(line, ' ');
    String fpin = urlDecode(getStringBefore(line, ' '));

    pinMode(fpin.toInt(), OUTPUT);
    digitalWrite(fpin.toInt(), 0);

    content = "OK, Pin " + fpin + " set to LOW";
    return content;
  }

  return "API page not here: " + path;
}
