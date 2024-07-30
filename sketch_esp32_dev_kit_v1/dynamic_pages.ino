String getDynamicPage(WiFiClient client, String path, String parameters) {
  String content;

  if (path == "/dyn_feature_add.html") {
    content = R"(
      <!DOCTYPE html>
      <html>
      <head>
        <link rel="stylesheet" href="iframe.css">
      </head>
      <body>
      Feature has been added!
      <script>
        setTimeout(function(){
          window.location.href = "/index_setup.html";
        }, 2500);
      </script>
      </body>
      </html>
      )";

    String id = generateShortGuid();
    String fname = "noname";
    String type;

    type = getValueForKey(parameters, "type");

    String feat = id + " " + fname + " " + type;

    File file = LittleFS.open("/features.txt", "a");
    file.println(feat);
    file.close();

    return content;
  }

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
      <script>
        setTimeout(function(){
          window.location.href = "/dyn_cfg_wifi.html";
        }, 2500);
      </script>
      </html>
      )";

    id = getValueForKey(parameters, "name");
    ssid = getValueForKey(parameters, "ssid");
    password = getValueForKey(parameters, "password");

    String cred = id + " " + ssid + " " + password;

    File file = LittleFS.open("/credentials.txt", "w");
    file.println(cred);
    file.close();

    return content;
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

    return content;
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
      line.replace("\r", ""); // Remove carriage returns
      line.replace("\n", ""); 

      if (line.length() != 0) {
        String item = R"(
        {
          "id": "$ID",
          "fname": "$FNAME",
          "type": "$TYPE"
        }, $ITEM)";

        String id = getStringBefore(line, ' ');        line = getStringAfter(line, ' ');
        String fname = getStringBefore(line, ' ');     line = getStringAfter(line, ' ');
        String type = getStringBefore(line, ' ');

        item.replace("$ID", id);
        item.replace("$FNAME", fname);
        item.replace("$TYPE", type);

        content.replace("$ITEM", item);
      } else {
        file.close();

        content.replace(", $ITEM", "");
        content.replace("$ITEM", "");

        return content;
      }
    }
  }

  return "Dynamic page note here: " + path;
}
