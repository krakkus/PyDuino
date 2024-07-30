
String getDynamicPage(WiFiClient client, String path, String parameters) {
  String content;

  if (path == "/dyn_ctrl_digital_output.html") {
    content = R"(
      <!DOCTYPE html>
      <html>
      <head>
        <link rel="stylesheet" href="iframe.css">
      </head>
      <body>
        <h3>$FNAME</h3>
        <button id="button-high" class="api-button">HIGH</button>
        <button id="button-low" class="api-button">LOW</button>
        <div id="result">no results yet..</div>
      </body>
      <script>
        const button1 = document.getElementById('button-high');
        const button2 = document.getElementById('button-low');
        const resultDiv = document.getElementById('result');
        
        button1.addEventListener('click', () => {
          const url = "/api_digital_output_high.html?id=$FID";
          
          fetch(url)
            .then(response => response.text())
            .then(data => {
              resultDiv.textContent = data;
            })
            .catch(error => {
              console.error('Error:', error);              
            });
        });
        
        button2.addEventListener('click', () => {
          const url = "/api_digital_output_low.html?id=$FID";
          
          fetch(url)
            .then(response => response.text())
            .then(data => {
              resultDiv.textContent = data;
            })
            .catch(error => {
              console.error('Error:', error);
            });
        });
        
      </script>
      </html>
      )";

    String fid = getValueForKey(parameters, "id");

    String line = getFeatureLineFor(fid);
    line = getStringAfter(line, ' ');
    String fname = urlDecode(getStringBefore(line, ' '));
    line = getStringAfter(line, ' ');
    // Type
    line = getStringAfter(line, ' ');
    String fpin = urlDecode(getStringBefore(line, ' '));
    line = getStringAfter(line, ' ');
    String fstate = urlDecode(getStringBefore(line, ' '));

    content.replace("$FID", fid);
    content.replace("$FNAME", fname);
    return content;
  }

  if (path == "/dyn_cfg_digital_output_save.html") {
    content = R"(
      <!DOCTYPE html>
      <html>
      <head>
        <link rel="stylesheet" href="iframe.css">
      </head>
      <body>
      Feature parameters have been saved!
      </body>
      <script>
        setTimeout(function(){
          window.location.href = "/dyn_cfg_digital_output.html?id=$FID";
        }, 2500);
      </script>
      </html>
      )";

    String fid = getValueForKey(parameters, "id");
    String fname = getValueForKey(parameters, "name");
    String fpin = getValueForKey(parameters, "pin");
    String fstate = getValueForKey(parameters, "state");

    String feature = fid + " " + fname + " " + "digital_output" + " " + fpin + " " + fstate;

    processFile("/features.txt", fid, feature);

    content.replace("$FID", fid);

    return content;
  }

  if (path == "/dyn_cfg_digital_output.html") {
    content = R"(
      <!DOCTYPE html>
      <html>
      <head>
        <link rel="stylesheet" href="iframe.css">
      </head>
      <body>
          <form action="/dyn_cfg_digital_output_save.html">
              <h3>Digital output</h3>
              <input type="hidden" id="id" name="id" value="$FID">
              <label for="name">Name:</label>
              <input type="text" id="name" name="name" value="$FNAME"><br><br>
              <label for="pin">pin:</label>
              <input type="text" id="pin" name="pin" value="$FPIN"><br><br> 
              <label for="state">state:</label>             
              <input type="text" id="state" name="state" value="$FSTATE"><br><br>
              <input type="submit" value="Save" class="action-button">
          </form>
        <br>
      </body>
      
      </html>
      )";

    String fid = getValueForKey(parameters, "id");
    String fname;
    String fpin;
    String fstate;

    String line = getFeatureLineFor(fid);
    line = getStringAfter(line, ' ');
    fname = urlDecode(getStringBefore(line, ' '));
    line = getStringAfter(line, ' ');
    // Type
    line = getStringAfter(line, ' ');
    fpin = urlDecode(getStringBefore(line, ' '));
    line = getStringAfter(line, ' ');
    fstate = urlDecode(getStringBefore(line, ' '));

    content.replace("$FID", fid);
    content.replace("$FNAME", fname);
    content.replace("$FPIN", fpin);
    content.replace("$FSTATE", fstate);

    return content;
  }

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
              <input type="text" id="name" name="name" value="$FID"><br><br>
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
          "id": "$FID",
          "fname": "$FNAME",
          "type": "$TYPE"
        }, $ITEM)";

        String fid = getStringBefore(line, ' ');       line = getStringAfter(line, ' ');
        String fname = getStringBefore(line, ' ');     line = getStringAfter(line, ' ');
        String type = getStringBefore(line, ' ');

        item.replace("$FID", fid);
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

  return "Dynamic page not here: " + path;
}
