#include "wifiController.hpp"

wifiController::wifiController(void) {
  isStatic = false;
  Serial.println("[WiFi] LittleFS (2MB SPIFFS) ativado para salvar redes");
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

  if (scan_count <= 0) {
    Serial.println("$WIFI:0"); 
    return;
  }
  for (int i = 0; i < scan_count && i < MAX_SCAN; ++i) {
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
  for (int i = 0; i < scan_count; ++i) if (scan_ssids[i] == ssid) idx = i;
  
  bool flag = false;
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
    saveWiFiCredentials(ssid, password);  // Salva credenciais
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
  Serial.println("[WIFI] DESCONECTADO");
}

bool wifiController::isConnected(void) {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[WIFI] CONECTADO");
    return true;
  } else {
    Serial.println("[WIFI] NAO CONECTADO");
    return false;
  }
}

String wifiController::getIPAddress(void){
  if(isConnected()){
    IP = WiFi.localIP().toString();
    Serial.println("$IP");
    Serial.print(IP);
    return IP;
  }
  else{
    return "";
  }
}

String wifiController::getSSID(void) {
  if (isConnected()) {
    String ssid = WiFi.SSID();
    Serial.println("$SSID");
    Serial.print(ssid);
    return ssid;
  } else {
    return "";
  }
}

String wifiController::getStatus(void) {
  wl_status_t status = WiFi.status();
  String statusStr;

  switch (status) {
    case WL_CONNECTED:        statusStr = "CONNECTED"; break;
    case WL_IDLE_STATUS:      statusStr = "IDLE"; break;
    case WL_NO_SSID_AVAIL:    statusStr = "NO_SSID"; break;
    case WL_CONNECT_FAILED:   statusStr = "FAIL"; break;
    case WL_CONNECTION_LOST:  statusStr = "LOST"; break;
    case WL_DISCONNECTED:     statusStr = "DISCONNECTED"; break;
    default:                  statusStr = "UNKNOWN"; break;
  }

  Serial.println("$STATUS");
  Serial.print(statusStr);

  return statusStr;
}

struct tm wifiController::getTimeFromNTP(void) {
  configTime(0, 0, "pool.ntp.org");  

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("[TIME:ERROR] Falha ao obter hora do NTP!");
    return {};
  }

  Serial.println("[TIME] Hora obtida do NTP:");
  Serial.print("HOUR:");
  Serial.print(timeinfo.tm_hour);
  Serial.print(" MINUTE:");
  Serial.print(timeinfo.tm_min);
  Serial.print(" SECOND:");
  Serial.println(timeinfo.tm_sec);

  return timeinfo;
}

void wifiController::getIPInfo(void) {
  Serial.println("\n=== IP CONFIGURATION INFO ===");
  
  if (!isConnected()) {
    Serial.println("WiFi nao conectado!");
    return;
  }

  // Obter IP e gateway
  IPAddress localIP = WiFi.localIP();
  IPAddress gateway = WiFi.gatewayIP();
  IPAddress subnet = WiFi.subnetMask();
  IPAddress dns = WiFi.dnsIP(0);

  Serial.print("IP Address: ");
  Serial.println(localIP);
  Serial.print("Gateway: ");
  Serial.println(gateway);
  Serial.print("Subnet Mask: ");
  Serial.println(subnet);
  Serial.print("DNS: ");
  Serial.println(dns);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  // Modo atual (agora com nossa flag)
  if (isStatic) {
    Serial.println("Modo: IP ESTATICO");
  } else {
    Serial.println("Modo: DHCP");
  }
}

void wifiController::setStaticIP(String ipConfig) {
  // === CORREÇÃO DO BUG: remove o prefixo "28." ===
  ipConfig = removePrefix(ipConfig);

  // Contar pontos e validar
  int dotCount = 0;
  for (int i = 0; i < ipConfig.length(); i++) {
    if (ipConfig[i] == '.') dotCount++;
  }

  if (dotCount < 12) {
    Serial.println("\n[ERRO] Formato invalido!");
    Serial.println("Use: $SET.IP.STATIC.192.168.1.100.255.255.255.0.192.168.1.1.8.8.8.8");
    Serial.println("Formato: IP.Subnet.Gateway.DNS");
    return;
  }

  // === Parsing dos 4 campos ===
  int idx1 = ipConfig.indexOf('.');
  int idx2 = ipConfig.indexOf('.', idx1 + 1);
  int idx3 = ipConfig.indexOf('.', idx2 + 1);
  int idx4 = ipConfig.indexOf('.', idx3 + 1);
  
  String ipStr = ipConfig.substring(0, idx4);
  String remaining = ipConfig.substring(idx4 + 1);

  IPAddress staticIP;
  if (!staticIP.fromString(ipStr)) {
    Serial.println("\n[ERRO] IP invalido!");
    return;
  }

  // Subnet
  idx1 = remaining.indexOf('.');
  idx2 = remaining.indexOf('.', idx1 + 1);
  idx3 = remaining.indexOf('.', idx2 + 1);
  idx4 = remaining.indexOf('.', idx3 + 1);
  
  String subnetStr = remaining.substring(0, idx4);
  remaining = remaining.substring(idx4 + 1);

  IPAddress subnet;
  if (!subnet.fromString(subnetStr)) {
    Serial.println("\n[ERRO] Subnet mask invalida!");
    return;
  }

  // Gateway
  idx1 = remaining.indexOf('.');
  idx2 = remaining.indexOf('.', idx1 + 1);
  idx3 = remaining.indexOf('.', idx2 + 1);
  idx4 = remaining.indexOf('.', idx3 + 1);
  
  String gatewayStr = remaining.substring(0, idx4);
  String dnsStr = remaining.substring(idx4 + 1);

  IPAddress gateway;
  if (!gateway.fromString(gatewayStr)) {
    Serial.println("\n[ERRO] Gateway invalido!");
    return;
  }

  // DNS - correção automática se for 0.0.0.0
  if (dnsStr == "0.0.0.0") {
    dnsStr = "8.8.8.8";
    Serial.println("[INFO] DNS 0.0.0.0 detectado → usando 8.8.8.8 automaticamente");
  }

  IPAddress dns;
  if (!dns.fromString(dnsStr)) {
    Serial.println("\n[ERRO] DNS invalido!");
    return;
  }

  // Aplicar configuração estática
  if (!WiFi.config(staticIP, gateway, subnet, dns)) {
    Serial.println("\n[ERRO] Falha ao configurar IP estatico!");
    return;
  }

  isStatic = true;   // ← já tínhamos adicionado antes

  // Reconectar
  WiFi.disconnect();
  delay(100);
  WiFi.reconnect();
  delay(2000);

  Serial.println("\n=== IP ESTATICO CONFIGURADO COM SUCESSO ===");
  Serial.print("IP: "); Serial.println(staticIP);
  Serial.print("Gateway: "); Serial.println(gateway);
  Serial.print("Subnet: "); Serial.println(subnet);
  Serial.print("DNS: "); Serial.println(dns);
}

void wifiController::setDHCP(void) {
  // Reabilitar DHCP
  WiFi.disconnect();
  delay(100);
  
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);   // só isso existe no ESP32

  isStatic = false;   // ← ADICIONADO

  Serial.println("\n=== DHCP ATIVADO ===");
  Serial.println("Reconectando com DHCP...");
  
  WiFi.reconnect();
}

int wifiController::loadSavedNetworks(String ssidList[10], String passList[10]) {
  int count = 0;
  File file = LittleFS.open("/saved_nets.txt", "r");
  if (!file) {
    Serial.println("[MEM] Arquivo /saved_nets.txt não encontrado ou não pode ser lido");
    return 0;
  }

  Serial.print("[MEM] Tamanho do arquivo: ");
  Serial.print(file.size());
  Serial.println(" bytes");

  while (file.available() && count < 10) {
    String line = file.readStringUntil('\n');
    
    // Remove \r se existir (comum em ESP32)
    if (line.endsWith("\r")) {
      line = line.substring(0, line.length() - 1);
    }
    
    line.trim();
    
    if (line.length() == 0) {
      continue;
    }

    int sep = line.indexOf('|');
    if (sep == -1) {
      Serial.print("[MEM LOAD] Linha sem separador |: ");
      Serial.println(line);
      continue;
    }

    ssidList[count] = line.substring(0, sep);
    passList[count] = line.substring(sep + 1);
    
    Serial.print("[MEM LOAD] [");
    Serial.print(count);
    Serial.print("] SSID: '");
    Serial.print(ssidList[count]);
    //Serial.print("' PASS: '");
    //Serial.print(passList[count]);
    Serial.println("'");
    
    count++;
  }
  
  file.close();
  Serial.print("[DEBUG] Total de redes carregadas: ");
  Serial.println(count);
  return count;
}

bool wifiController::saveSavedNetworks(String ssidList[10], String passList[10], int count) {
  File file = LittleFS.open("/saved_nets.txt", "w");
  if (!file) {
    Serial.println("[ERRO] Não conseguiu abrir arquivo para escrita");
    return false;
  }

  for (int i = 0; i < count; i++) {
    file.print(ssidList[i]);
    file.print('|');
    file.println(passList[i]);
  }
  
  file.flush();  // Força a escrita no armazenamento
  file.close();
  
  // Verifica se o arquivo foi criado/atualizado
  File checkFile = LittleFS.open("/saved_nets.txt", "r");
  if (checkFile) {
    Serial.print("[MEM] Arquivo salvo com sucesso. Tamanho: ");
    Serial.print(checkFile.size());
    Serial.println(" bytes");
    checkFile.close();
  }
  
  return true;
}

void wifiController::saveWiFiCredentials(String ssid, String password) {
  String ssidList[10];
  String passList[10];
  int count = loadSavedNetworks(ssidList, passList);

  // Atualiza se já existe
  bool exists = false;
  for (int i = 0; i < count; i++) {
    if (ssidList[i] == ssid) {
      passList[i] = password;
      exists = true;
      break;
    }
  }

  if (!exists) {
    if (count >= 10) {
      Serial.println("[WiFi] Limite de 10 redes atingido!");
      return;
    }
    ssidList[count] = ssid;
    passList[count] = password;
    count++;
  }

  if (saveSavedNetworks(ssidList, passList, count)) {
    Serial.println("[WiFi] Credenciais salvas com sucesso no LittleFS!");
  } else {
    Serial.println("[ERRO] Falha ao gravar arquivo no LittleFS!");
  }
}

void wifiController::autoConnectToSavedNetworks(void) {
  String ssidList[10];
  String passList[10];
  int count = loadSavedNetworks(ssidList, passList);

  if (count == 0) {
    Serial.println("[WiFi] Nenhuma rede salva no LittleFS.");
    return;
  }

  Serial.println("[WiFi] Tentando conectar em redes salvas (LittleFS)...");
  updateSSIDS();

  for (int i = 0; i < count; i++) {
    String savedSSID = ssidList[i];
    String savedPass = passList[i];
    if (savedSSID.length() == 0) continue;

    bool found = false;
    for (int j = 0; j < scan_count; j++) {
      if (scan_ssids[j] == savedSSID) { found = true; break; }
    }
    if (!found) continue;

    Serial.print("[WiFi] Tentando '"); Serial.print(savedSSID); Serial.println("'...");

    disconnect();
    delay(100);
    if (savedPass.length() == 0) WiFi.begin(savedSSID.c_str());
    else WiFi.begin(savedSSID.c_str(), savedPass.c_str());

    unsigned long start = millis();
    while (millis() - start < CONNECT_TIMEOUT_MS) {
      if (WiFi.status() == WL_CONNECTED) {
        IP = WiFi.localIP().toString();
        Serial.print("[WiFi] Conectado! IP: "); Serial.println(IP);
        return;
      }
      delay(200);
    }
  }
  Serial.println("[WiFi] Nenhuma rede salva conseguiu conectar.");
}

void wifiController::printSavedNetworks(WiFiClient* client = nullptr) {
  String ssidList[10];
  String passList[10];
  int count = loadSavedNetworks(ssidList, passList);

  if (client == nullptr) {  // Serial
    Serial.println("\n=== REDES SALVAS ===");
    Serial.print("Total: ");
    Serial.println(count);
    
    if (count == 0) {
      Serial.println("Nenhuma rede salva.");
      return;
    }
    
    for (int i = 0; i < count; i++) {
      Serial.print("  [");
      Serial.print(i);
      Serial.print("] SSID: ");
      Serial.print(ssidList[i]);
      //Serial.print(" | SENHA: ");
      //Serial.println(passList[i]);
    }
  } else {  // TCP
    client->println("\n=== REDES SALVAS ===");
    client->print("Total: ");
    client->println(count);
    
    if (count == 0) {
      client->println("Nenhuma rede salva.");
      return;
    }
    
    for (int i = 0; i < count; i++) {
      client->print("  [");
      client->print(i);
      client->print("] SSID: ");
      client->print(ssidList[i]);
      //client->print(" | SENHA: ");
      //client->println(passList[i]);
    }
  }
}

void wifiController::clearSavedNetworks(void) {
  if (LittleFS.remove("/saved_nets.txt")) {
    Serial.println("\n[WiFi] Todas as redes salvas foram removidas (LittleFS)!");
  } else {
    Serial.println("\n[WiFi] Nenhuma rede salva ou arquivo já removido.");
  }
}

void wifiController::printHelp(WiFiClient* client) {
  if (client == nullptr) {  // Serial
    Serial.println("\n\n========================================");
    Serial.println("           WAKELAMP - COMMAND HELP");
    Serial.println("========================================\n");

    Serial.println("=== CLOCK COMMANDS (Relogio) ===");
    Serial.println("$GET.TIME");
    Serial.println("  - Gets current time (HH:MM:SS)");
    Serial.println("  - Example: $GET.TIME\n");

    Serial.println("$UPDATE.TIME");
    Serial.println("  - Updates time from NTP server");
    Serial.println("  - Example: $UPDATE.TIME\n");

    Serial.println("\n=== WAKE TIME COMMANDS ===");
    Serial.println("$GET.WAKE.TIME");
    Serial.println("  - Gets alarm wake time");
    Serial.println("  - Example: $GET.WAKE.TIME\n");

    Serial.println("$SET.WAKE.TIME.HH.MM.SS");
    Serial.println("  - Sets alarm wake time");
    Serial.println("  - Example: $SET.WAKE.TIME.05.30.00 (5:30 AM)\n");

    Serial.println("$GET.LAMP.INTERVAL");
    Serial.println("  - Gets lamp on duration");
    Serial.println("  - Example: $GET.LAMP.INTERVAL\n");

    Serial.println("$SET.LAMP.INTERVAL.HH.MM.SS");
    Serial.println("  - Sets lamp on duration");
    Serial.println("  - Example: $SET.LAMP.INTERVAL.01.00.00 (1 hour)\n");

    Serial.println("\n=== WIFI COMMANDS ===");
    Serial.println("$GET.ALL.SSIDS");
    Serial.println("  - Scans and lists all available WiFi networks");
    Serial.println("  - Example: $GET.ALL.SSIDS\n");

    Serial.println("$CONNECT.SSID.PASSWORD");
    Serial.println("  - Connects to a WiFi network");
    Serial.println("  - Example: $CONNECT.MyNetwork.MyPassword123\n");

    Serial.println("$DISCONNECT");
    Serial.println("  - Disconnects from WiFi");
    Serial.println("  - Example: $DISCONNECT\n");

    Serial.println("$GET.IS.CONNECTED");
    Serial.println("  - Checks WiFi connection status");
    Serial.println("  - Example: $GET.IS.CONNECTED\n");

    Serial.println("$GET.SSID");
    Serial.println("  - Gets connected network name");
    Serial.println("  - Example: $GET.SSID\n");

    Serial.println("$GET.IP.ADDRESS");
    Serial.println("  - Gets current IP address");
    Serial.println("  - Example: $GET.IP.ADDRESS\n");

    Serial.println("$GET.STATUS");
    Serial.println("  - Gets WiFi connection status (CONNECTED/DISCONNECTED/etc)");
    Serial.println("  - Example: $GET.STATUS\n");

    Serial.println("\n=== IP CONFIGURATION COMMANDS ===");
    Serial.println("$GET.IP.INFO");
    Serial.println("  - Gets full IP configuration (IP, Gateway, Subnet, DNS)");
    Serial.println("  - Example: $GET.IP.INFO\n");

    Serial.println("$SET.IP.STATIC.IP.SUBNET.GATEWAY.DNS");
    Serial.println("  - Sets static IP configuration");
    Serial.println("  - Example: $SET.IP.STATIC.192.168.1.100.255.255.255.0.192.168.1.1.8.8.8.8\n");

    Serial.println("$SET.DHCP");
    Serial.println("  - Enables DHCP (automatic IP assignment)");
    Serial.println("  - Example: $SET.DHCP\n");

    Serial.println("\n=== SAVED NETWORKS COMMANDS ===");
    Serial.println("$GET.SAVED.SSIDS");
    Serial.println("  - Lists all saved WiFi networks");
    Serial.println("  - Example: $GET.SAVED.SSIDS\n");

    Serial.println("$CLEAR.SAVED.SSIDS");
    Serial.println("  - Clears all saved WiFi networks");
    Serial.println("  - Example: $CLEAR.SAVED.SSIDS\n");

    Serial.println("\n=== TCP USER MANAGEMENT COMMANDS ===");
    Serial.println("$SET.TCP.USER.username.password");
    Serial.println("  - Creates or updates a TCP user");
    Serial.println("  - Example: $SET.TCP.USER.john.pass123\n");

    Serial.println("$GET.TCP.USERS");
    Serial.println("  - Lists all registered TCP users (without passwords)");
    Serial.println("  - Example: $GET.TCP.USERS\n");

    Serial.println("$REMOVE.TCP.USER.username");
    Serial.println("  - Removes a TCP user (cannot remove ukm1/admin)");
    Serial.println("  - Example: $REMOVE.TCP.USER.john\n");

    Serial.println("$CLEAR.TCP.USERS");
    Serial.println("  - Clears all TCP users (except admin)");
    Serial.println("  - Example: $CLEAR.TCP.USERS\n");

    Serial.println("\n=== TCP CONNECTION COMMANDS ===");
    Serial.println("$TCP.LOGOUT");
    Serial.println("  - Disconnects TCP client (Serial only)");
    Serial.println("  - Example: $TCP.LOGOUT\n");

    Serial.println("$TCP.DISCONNECT.CLIENT");
    Serial.println("  - Forces TCP client disconnect from Serial");
    Serial.println("  - Example: $TCP.DISCONNECT.CLIENT\n");

    Serial.println("========================================");
    Serial.println("        For TCP, use same commands");
    Serial.println("     (connect with: telnet <ip> 22)");
    Serial.println("========================================\n");

  } else {  // TCP
    client->println("\n\n========================================");
    client->println("           WAKELAMP - COMMAND HELP");
    client->println("========================================\n");

    client->println("=== CLOCK COMMANDS (Relogio) ===");
    client->println("$GET.TIME");
    client->println("  - Gets current time (HH:MM:SS)");
    client->println("  - Example: $GET.TIME\n");

    client->println("$UPDATE.TIME");
    client->println("  - Updates time from NTP server");
    client->println("  - Example: $UPDATE.TIME\n");

    client->println("\n=== WAKE TIME COMMANDS ===");
    client->println("$GET.WAKE.TIME");
    client->println("  - Gets alarm wake time");
    client->println("  - Example: $GET.WAKE.TIME\n");

    client->println("$SET.WAKE.TIME.HH.MM.SS");
    client->println("  - Sets alarm wake time");
    client->println("  - Example: $SET.WAKE.TIME.05.30.00 (5:30 AM)\n");

    client->println("$GET.LAMP.INTERVAL");
    client->println("  - Gets lamp on duration");
    client->println("  - Example: $GET.LAMP.INTERVAL\n");

    client->println("$SET.LAMP.INTERVAL.HH.MM.SS");
    client->println("  - Sets lamp on duration");
    client->println("  - Example: $SET.LAMP.INTERVAL.01.00.00 (1 hour)\n");

    client->println("\n=== WIFI COMMANDS ===");
    client->println("$GET.ALL.SSIDS");
    client->println("  - Scans and lists all available WiFi networks");
    client->println("  - Example: $GET.ALL.SSIDS\n");

    client->println("$CONNECT.SSID.PASSWORD");
    client->println("  - Connects to a WiFi network");
    client->println("  - Example: $CONNECT.MyNetwork.MyPassword123\n");

    client->println("$DISCONNECT");
    client->println("  - Disconnects from WiFi");
    client->println("  - Example: $DISCONNECT\n");

    client->println("$GET.IS.CONNECTED");
    client->println("  - Checks WiFi connection status");
    client->println("  - Example: $GET.IS.CONNECTED\n");

    client->println("$GET.SSID");
    client->println("  - Gets connected network name");
    client->println("  - Example: $GET.SSID\n");

    client->println("$GET.IP.ADDRESS");
    client->println("  - Gets current IP address");
    client->println("  - Example: $GET.IP.ADDRESS\n");

    client->println("$GET.STATUS");
    client->println("  - Gets WiFi connection status (CONNECTED/DISCONNECTED/etc)");
    client->println("  - Example: $GET.STATUS\n");

    client->println("\n=== IP CONFIGURATION COMMANDS ===");
    client->println("$GET.IP.INFO");
    client->println("  - Gets full IP configuration (IP, Gateway, Subnet, DNS)");
    client->println("  - Example: $GET.IP.INFO\n");

    client->println("$SET.IP.STATIC.IP.SUBNET.GATEWAY.DNS");
    client->println("  - Sets static IP configuration");
    client->println("  - Example: $SET.IP.STATIC.192.168.1.100.255.255.255.0.192.168.1.1.8.8.8.8\n");

    client->println("$SET.DHCP");
    client->println("  - Enables DHCP (automatic IP assignment)");
    client->println("  - Example: $SET.DHCP\n");

    client->println("\n=== SAVED NETWORKS COMMANDS ===");
    client->println("$GET.SAVED.SSIDS");
    client->println("  - Lists all saved WiFi networks");
    client->println("  - Example: $GET.SAVED.SSIDS\n");

    client->println("$CLEAR.SAVED.SSIDS");
    client->println("  - Clears all saved WiFi networks");
    client->println("  - Example: $CLEAR.SAVED.SSIDS\n");

    client->println("\n=== TCP USER MANAGEMENT COMMANDS ===");
    client->println("$SET.TCP.USER.username.password");
    client->println("  - Creates or updates a TCP user");
    client->println("  - Example: $SET.TCP.USER.john.pass123\n");

    client->println("$GET.TCP.USERS");
    client->println("  - Lists all registered TCP users (without passwords)");
    client->println("  - Example: $GET.TCP.USERS\n");

    client->println("$REMOVE.TCP.USER.username");
    client->println("  - Removes a TCP user (cannot remove ukm1/admin)");
    client->println("  - Example: $REMOVE.TCP.USER.john\n");

    client->println("$CLEAR.TCP.USERS");
    client->println("  - Clears all TCP users (except admin)");
    client->println("  - Example: $CLEAR.TCP.USERS\n");

    client->println("\n=== TCP CONNECTION COMMANDS ===");
    client->println("$TCP.LOGOUT");
    client->println("  - Disconnects your TCP session");
    client->println("  - Example: $TCP.LOGOUT\n");

    client->println("========================================");
    client->println("        All commands use $ prefix");
    client->println("      Commands are case-sensitive!");
    client->println("========================================\n");
  }
}