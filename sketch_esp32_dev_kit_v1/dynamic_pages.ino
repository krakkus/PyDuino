String getDynamicPage(WiFiClient client, String path, String parameters) {
  String content;

  if (path == "/dyn_cfg_wifi.html") {
    content = R"(
      <!DOCTYPE html>
      <html>
      <head>
        <link rel="stylesheet" href="iframe.css">
      </head>
      <body>
          <h3>Devicename</h3>
          <form>
              <label for="name">Name:</label>
              <input type="text" id="name" name="name"><br><br>
              <input type="submit" value="Save" class="action-button">
          </form>
          <br><br><br>
          <h3>WiFi credentials</h3>
          <form>
              <label for="ssid">SSID:</label>
              <input type="text" id="ssid" name="ssid"><br><br>
              <label for="password">Password:</label>
              <input type="password" id="password" name="password"><br><br>
              <input type="submit" value="Save" class="action-button">
          </form>
        <br>
      </body>
      
      </html>
      )";
  }
  
  if (path == "/dyn_get_features.json") {
    content = R"(
      [
        {
          "id": "BuiltInLED",
          "type": "digital_output"
        },
        {
          "id": "Door sensor",
          "type": "digital_input"
        }
      ]
      )";
  }

  return content;
}
