
String getAPIPage(WiFiClient client, String path, String parameters) {
  String content;

  if (path == "/api_digital_output_high.html") {
    String fname = getValueForKey(parameters, "name");

    String line = getFeatureLineForName(fname);
    line = getStringAfter(line, ' ');
    String x = urlDecode(getStringBefore(line, ' '));
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
    String fname = getValueForKey(parameters, "name");

    String line = getFeatureLineForName(fname);
    line = getStringAfter(line, ' ');
    String x = urlDecode(getStringBefore(line, ' '));
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
