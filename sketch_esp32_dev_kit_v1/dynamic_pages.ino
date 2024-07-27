
String getDynamicPage(WiFiClient client, String filePath) {
  String content;

  if (filePath == "/dyn_sh_netw_conf.html") {
    content = R"(
    <h3>WiFi Configuration</h3>
    <form>
        <label for="name">Name:</label>
        <input type="text" id="name" name="name"><br><br>
        <label for="ssid">SSID:</label>
        <input type="text" id="ssid" name="ssid"><br><br>
        <label for="password">Password:</label>
        <input type="password" id="password" name="password"><br><br>
        <input type="submit" value="Save" class="action-button">
    </form>)";
  }

  return content;
}
