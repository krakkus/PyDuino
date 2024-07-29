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
          <form>
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

    File file = SPIFFS.open("/features.txt", "r");
    String line = file.readStringUntil('\n');
    file.close();

    String id = getStringBefore(line, ' ');
    content.replace("$ID", id);
    line = getStringAfter(line, ' ');

    String ssid = getStringBefore(line, ' ');
    content.replace("$SSID", ssid);
    line = getStringAfter(line, ' ');

    String pwd = getStringBefore(line, ' ');
    content.replace("$PWD", pwd);

  }


  if (path == "/dyn_get_features.json") {
    String content = R"(
      [
        $ITEM
      ]
      )";

    File file = SPIFFS.open("/features.txt", "r");
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
