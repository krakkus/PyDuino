String getDynamicPage(WiFiClient client, String path, String parameters) {
  String content;

  if (path == "/dyn_cfg_wifi_save.html") {
    content = R"(
      <!DOCTYPE html>
      <html>
      <head>
        <link rel="stylesheet" href="iframe.css">
      </head>
      <body>
      WiFi credentials have been saved!
      </body>
      </html>
      )";

    while (parameters.length() > 0) {
      String kv = getStringBefore(parameters, '&');
      parameters = getStringAfter(parameters, '&');

      String k = getStringBefore(kv, '=');
      String v = getStringAfter(kv, '=');

      if (k == "name") id = v;
      if (k == "ssid") ssid = v;
      if (k == "password") password = v;
    }

    String cred = id + " " + ssid + " " + password;

    File file = LittleFS.open("/credentials.txt", "w");
    file.println(cred);
    file.close();
  }

  if (path == "/dyn_cfg_wifi.html") {
    content = R"(
      <!DOCTYPE html>
      <html>
      <head>
        <link rel="stylesheet" href="iframe.css">
      </head>
      <body>
          <form action="/dyn_cfg_wifi_save.html">
              <h3>Devicename</h3>
              <label for="name">Name:</label>
              <input type="text" id="name" name="name" value="$ID"><br><br>
              <br>
              <h3>WiFi credentials</h3>
              <label for="ssid">SSID:</label>
              <input type="text" id="ssid" name="ssid" value="$SSID"><br><br>
              <label for="password">Password:</label>
              <input type="password" id="password" name="password" value="$PWD"><br><br><br>
              <input type="submit" value="Save" class="action-button">
          </form>
        <br>
      </body>
      
      </html>
      )";

    content.replace("$ID", id);
    content.replace("$SSID", ssid);
    content.replace("$PWD", password);
  }


  if (path == "/dyn_get_features.json") {
    String content = R"(
      [
        $ITEM
      ]
      )";

    File file = LittleFS.open("/features.txt", "r");
    while (true) {
      String line = file.readStringUntil('\n');
      if (line.length() == 0) {
        file.close(); // Close the file when done
        break; // Exit the loop
      } else {
        String item = R"(
        {
          "id": "$ID",
          "type": "$TYPE"
        }, $ITEM)";

        String id = getStringBefore(line, ' ');
        line = getStringAfter(line, ' ');
        String type = getStringBefore(line, ' ');

        item.replace("$ID", id);
        item.replace("$TYPE", type);

        content.replace("$ITEM", item);
      }
    }
    file.close();

    content.replace("$ITEM", "");
    content.replace(", $ITEM", "");
  }

  // Return the final content
  return content;
}
