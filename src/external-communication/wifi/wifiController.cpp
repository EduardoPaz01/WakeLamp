#include "wifiController.hpp"

wifiController::wifiController(void) {
  // Constructor implementation
}

void wifiController::updateSSIDS(void) {
  scan_count = 0;
  WiFi.mode(WIFI_STA);
  delay(100);

  int n = WiFi.scanNetworks();
  if (n <= 0) {
    Serial.println("$WIFI:0"); 
    return;
  }

  for (int i = 0; i < n && i < MAX_SCAN; ++i) {
    scan_ssids[i] = WiFi.SSID(i);
    scan_rssis[i] = WiFi.RSSI(i);
    scan_auths[i] = WiFi.encryptionType(i);
    scan_count++;
  }

  WiFi.scanDelete();
}

void wifiController::getAllSSIDS(void) {
  updateSSIDS();

  int n = WiFi.scanNetworks();
  if (n <= 0) {
    Serial.println("$WIFI:0"); 
    return;
  }
  for (int i = 0; i < n && i < MAX_SCAN; ++i) {
    Serial.print("\n$WIFI: ");
    Serial.print(scan_ssids[i]);
    Serial.print(",");
    Serial.print(scan_rssis[i]);
    Serial.print(",");
    Serial.println(authToString(scan_auths[i])); 
  }
}

void wifiController::connect(String ssid, String password){
  updateSSIDS();

  int idx = -1;
  for (int i = 0; i < scan_count; ++i) if (scan_ssids[i] == ssid) idx = 1;
  
  bool flag = false;
  WiFi.mode(WIFI_STA);
  disconnect();
  delay(100);
  if (password.length() == 0) WiFi.begin(ssid.c_str());
  else WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long start = millis();
  while (millis() - start < CONNECT_TIMEOUT_MS) {
    if (WiFi.status() == WL_CONNECTED) flag = true;
    delay(200);
  }

  if (flag) {
    IP = WiFi.localIP().toString();
    Serial.println("$CNT");
    Serial.println(IP);
    return;
  }

  if (idx != -1) {
    int auth = scan_auths[idx];
    if (auth != WIFI_AUTH_OPEN && password.length() > 0) {
      Serial.println("$ERR.KEY");
      return;
    }
    Serial.println("$ERR");
    return;
  }

  Serial.println("$ERR");
}

void wifiController::disconnect(void){
  WiFi.disconnect();
  Serial.println("$DCNT");
}