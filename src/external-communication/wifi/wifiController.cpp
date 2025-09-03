#include "wifiController.hpp"

wifiController::wifiController(void) {
  // Constructor implementation
}

void wifiController::getAllSSIDS(void) {
  scan_count = 0;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int n = WiFi.scanNetworks();
  if (n <= 0) {
    Serial.println("$WIFI:0"); // Nenhuma rede encontrada
    return;
  }

  for (int i = 0; i < n && i < MAX_SCAN; ++i) {
    scan_ssids[i] = WiFi.SSID(i);
    scan_rssis[i] = WiFi.RSSI(i);
    scan_auths[i] = WiFi.encryptionType(i);
    scan_count++;

    Serial.print("\n$WIFI: ");
    Serial.print(scan_ssids[i]);
    Serial.print(",");
    Serial.print(scan_rssis[i]);
    Serial.print(",");
    Serial.println(authToString(scan_auths[i])); 
  }

  WiFi.scanDelete(); // Limpa os resultados do scan
}