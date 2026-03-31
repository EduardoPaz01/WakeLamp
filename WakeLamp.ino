#include "src/external-communication/serial/serial.hpp"
#include "src/external-communication/wifi/wifiController.hpp"
#include "src/lamp/lamp.hpp"
#include "src/time/timeController.hpp"
#include "src/utils/string/string-editions.hpp"
#include "src/time/timeClass.hpp"
#include "src/user/userController.hpp"
#include "Arduino.h"
#include "time.h"

timeController myTime;
wifiController myWifi; 
lamp myLamp;
serial mySerial(115200);
userController myUsers;

const unsigned long debounceDelay = 50; // 50 ms
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;

// ==================== SERVIDOR TCP PARA COMANDOS ====================
WiFiServer commandServer(22);      // Porta TCP
WiFiClient commandClient;          // Cliente conectado

// ==================== AUTENTICAÇÃO ====================
#define TCP_USERNAME "ukm1"
#define TCP_PASSWORD "bnmjhgtyu"
bool clientAuthenticated = false;  // Flag de autenticação
String tcpInputUser = "";          // Usuário TCP
String tcpInputPass = "";          // Senha TCP
int tcpAuthStep = 0;               // Passo de autenticação (0: user, 1: pass, 2: autenticado)

const String userController::ADMIN_USERNAME = "admin";
const String userController::ADMIN_PASSWORD = "00000000";

void setup() {
  pinMode(BUTTON_PIN, INPUT);   // já tem pull-up externo
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(OUTPUT_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Inicializar WiFi antes do servidor TCP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);  // Desliga e limpa memória anterior
  delay(100);

  // === LITTLEFS (2MB SPIFFS) ===
  Serial.println("\n=== INICIANDO LITTLEFS ===");
  if (!LittleFS.begin(true)) {  // true = formata automaticamente se falhar
    Serial.println("[ERRO] LittleFS falhou ao montar! Formatando...");
    if (!LittleFS.begin(true)) {
      Serial.println("[ERRO GRAVE] LittleFS não conseguiu formatar!");
    } else {
      Serial.println("[OK] LittleFS formatado e montado com sucesso.");
    }
  } else {
    Serial.println("[OK] LittleFS montado com sucesso (2MB SPIFFS).");
  }

  // Tenta conectar automaticamente em redes salvas
  myWifi.autoConnectToSavedNetworks();
  if(myWifi.isConnected()) {
    myTime.updateTime(myWifi.getTimeFromNTP());
    mySerial.print("HOUR:");
    mySerial.print(String(myTime.getTime().tm_hour));
    mySerial.print(" MINUTE:");
    mySerial.print(String(myTime.getTime().tm_min));
    mySerial.print(" SECOND:");
    mySerial.print(String(myTime.getTime().tm_sec));
    mySerial.print("\n");
  }
  
  // Carregar usuários TCP salvos
  myUsers.initUsers();
  
  Serial.println("[MEM] Count inicial no boot:");
  myWifi.printSavedNetworks(&commandClient); 

  commandServer.begin();
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  // Se mudou, reinicia o debounce
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // Após tempo de debounce
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    if (reading != buttonState) {
      buttonState = reading;

      // Detecta borda de descida (HIGH → LOW)
      if (buttonState == LOW) {
        myLamp.toggleLamp();
      }
    }
  }

  lastButtonState = reading;

  myTime.tick();
  int nowSec =
    myTime.getTime().tm_hour * 3600 +
    myTime.getTime().tm_min * 60 +
    myTime.getTime().tm_sec;
  int startSec =
    myTime.getWakeTime().HOUR * 3600 +
    myTime.getWakeTime().MINUTE * 60 +
    myTime.getWakeTime().SECOND;
  int intervalSec =
    myTime.getLampInterval().HOUR * 3600 +
    myTime.getLampInterval().MINUTE * 60 +
    myTime.getLampInterval().SECOND;
  int endSec = (startSec + intervalSec) % 86400;

  bool timeState;
  if (startSec < endSec) timeState = (nowSec >= startSec && nowSec < endSec);
  else timeState = (nowSec >= startSec || nowSec < endSec);

  static unsigned int lastSecond = 255; // Variável estática para guardar o segundo anterior
  unsigned int currentSecond = myTime.getTime().tm_sec;
  
  /*
  // Atualiza apenas quando os segundos mudam
  if (currentSecond != lastSecond) {
    Serial.print("HORA:");
    Serial.print(myTime.getTime().tm_hour);
    Serial.print(" MINUTO:");
    Serial.print(myTime.getTime().tm_min);
    Serial.print(" SEGUNDO:");
    Serial.println(myTime.getTime().tm_sec);
    
    lastSecond = currentSecond; // Atualiza o segundo anterior
  }
  */
  
  // Detecta mudança de estado (borda)
  if (timeState != myLamp.lastTimeState) {
    if (timeState) {
      myLamp.lampOn();   // só uma vez
      mySerial.print("\n[TIME] Hora de acordar! Lamp ON.");
      myTime.updateTime(myWifi.getTimeFromNTP());
      mySerial.print("HOUR:");
      mySerial.print(String(myTime.getTime().tm_hour));
      mySerial.print(" MINUTE:");
      mySerial.print(String(myTime.getTime().tm_min));
      mySerial.print(" SECOND:");
      mySerial.print(String(myTime.getTime().tm_sec));
      mySerial.print("\n");
    }
    else myLamp.lampOff();  // só uma vez
    myLamp.lastTimeState = timeState;
  }

  myLamp.updatedLamp();

  uint32_t command = mySerial.processSerial();
  if (command != UNKNOWN_CMD) {
    switch (command) {
      case GET_TIME: {
        mySerial.print("HOUR:");
        mySerial.print(String(myTime.getTime().tm_hour));
        mySerial.print(" MINUTE:");
        mySerial.print(String(myTime.getTime().tm_min));
        mySerial.print(" SECOND:");
        mySerial.print(String(myTime.getTime().tm_sec));
        mySerial.print("\n");
        break;
      }
      case UPDATE_TIME: {
        myTime.updateTime(myWifi.getTimeFromNTP());
        break;
      }
      case SET_WAKE_TIME: {
        String buffer = mySerial.getBuffer();
        
        timeClass newTime = parseTime(buffer);
        myTime.setWakeTime(newTime);

        mySerial.print("\nWake time updated!");
        break;
      }
      case GET_WAKE_TIME: {
        mySerial.print("HOUR:");
        mySerial.print(String(myTime.getWakeTime().HOUR));
        mySerial.print(" MINUTE:");
        mySerial.print(String(myTime.getWakeTime().MINUTE));
        mySerial.print(" SECOND:");
        mySerial.print(String(myTime.getWakeTime().SECOND));
        mySerial.print("\n");
        break;
      }
      case SET_LAMP_INTERVAL: {
        String buffer = mySerial.getBuffer();
        
        timeClass newTime = parseTime(buffer);
        myTime.setLampInterval(newTime);

        mySerial.print("\nLamp interval updated!");
        break;
      }
      case GET_LAMP_INTERVAL: {
        mySerial.print("HOUR:");
        mySerial.print(String(myTime.getLampInterval().HOUR));
        mySerial.print(" MINUTE:");
        mySerial.print(String(myTime.getLampInterval().MINUTE));
        mySerial.print(" SECOND:");
        mySerial.print(String(myTime.getLampInterval().SECOND));
        mySerial.print("\n");
        break;
      }
      case GET_ALL_SSIDS: {
        myWifi.getAllSSIDS();  
        break;
      }
      case CONNECT:{
        String current_buffer = mySerial.getBuffer();
        if (current_buffer.startsWith(String(CONNECT))) {
          current_buffer = removePrefix(current_buffer);
          int dotIndex = current_buffer.indexOf('.');
          String SSID = current_buffer.substring(0, dotIndex);
          String PASS = current_buffer.substring(dotIndex + 1);
          myWifi.connect(SSID, PASS);
        }
        break;
      }
      case DISCONNECT: {
        myWifi.disconnect();
        break;
      }
      case GET_IS_CONNECTED: {
        myWifi.isConnected();
        break;
      }
      case GET_IP_ADDRESS: {
        myWifi.getIPAddress();
        break;
      }
      case GET_SSID: {
        myWifi.getSSID();
        break;
      }
      case GET_STATUS: {
        myWifi.getStatus();
        break;
      }
      case GET_IP_INFO: {
        myWifi.getIPInfo();
        break;
      }
      case SET_IP_STATIC: {
        String buffer = mySerial.getBuffer();
        myWifi.setStaticIP(buffer);
        break;
      }
      case SET_DHCP: {
        myWifi.setDHCP();
        break;
      }
      case TCP_DISCONNECT_CLIENT: {
        if (commandClient && commandClient.connected()) {
          commandClient.println("Desconectado pelo servidor!");
          commandClient.stop();
          clientAuthenticated = false;
          mySerial.print("\n[TCP] Cliente desconectado!");
        } else {
          mySerial.print("\n[TCP] Nenhum cliente conectado.");
        }
        break;
      }
      case GET_SAVED_SSIDS: {
        myWifi.printSavedNetworks(&commandClient);
        break;
      }
      case CLEAR_SAVED_SSIDS: {
        myWifi.clearSavedNetworks();
        break;
      }
      case GET_HELP: {
        myWifi.printHelp(nullptr);
        break;
      }

      default: {
        mySerial.print("Unknown command.");
        break;
      }
    }
  }

  // Aceita novo cliente se não tiver nenhum conectado
  if (!commandClient || !commandClient.connected()) {
    commandClient = commandServer.available();
    if (commandClient) {
      Serial.println("[TCP] Cliente TCP conectado!");
      // Resetar variáveis de autenticação
      tcpInputUser = "";
      tcpInputPass = "";
      tcpAuthStep = 0;
      clientAuthenticated = false;
      
      commandClient.println("=== WakeLamp TCP Server ===");
      commandClient.println("Autenticacao requerida.");
      commandClient.print("Usuario: ");
    }
  }

  // Processa dados vindos do cliente TCP
  if (commandClient && commandClient.connected()) {
    static String netBuf = "";
    
    while (commandClient.available()) {
      char c = (char)commandClient.read();
      if (c == '\r') continue;
      if (c == '\n') {
        String CACHE_LINE_NET = netBuf;
        netBuf = "";
        CACHE_LINE_NET.trim();
        if (CACHE_LINE_NET.length() == 0) continue;

        // ========== SEÇÃO DE AUTENTICAÇÃO ==========
        if (!clientAuthenticated) {
          if (tcpAuthStep == 0) {
            // Recebendo usuário
            tcpInputUser = CACHE_LINE_NET;
            tcpAuthStep = 1;
            commandClient.print("Senha: ");
          } 
          else if (tcpAuthStep == 1) {
            // Recebendo senha
            tcpInputPass = CACHE_LINE_NET;
            
            // Verifica credenciais usando sistema de usuários
            if (myUsers.validateUser(tcpInputUser, tcpInputPass)) {
              clientAuthenticated = true;
              tcpAuthStep = 2;
              commandClient.println("\n*** AUTENTICADO COM SUCESSO ***");
              commandClient.println("Para comecar, tente por $HELP");
            } else {
              commandClient.println("ERRO: Usuario ou senha incorretos!");
              commandClient.print("Usuario: ");
              tcpAuthStep = 0;
              tcpInputUser = "";
              tcpInputPass = "";
            }
          }
          continue;
        }
        
        // ========== PROCESSAMENTO DE COMANDOS (SOMENTE SE AUTENTICADO) ==========
        uint32_t netCommand = UNKNOWN_CMD;
        String BUFFER_NET = "";

        if (CACHE_LINE_NET.startsWith("$")) {
          CACHE_LINE_NET = CACHE_LINE_NET.substring(1);

          if (CACHE_LINE_NET == "GET.TIME"){
            netCommand = GET_TIME;
          }
          else if (CACHE_LINE_NET == "UPDATE.TIME"){
            netCommand = UPDATE_TIME;
          }
          else if (CACHE_LINE_NET.startsWith("SET.WAKE.TIME.")) {
            CACHE_LINE_NET = CACHE_LINE_NET.substring(14);
            BUFFER_NET = String(SET_WAKE_TIME) + "." + CACHE_LINE_NET;
            netCommand = SET_WAKE_TIME;
          } 
          else if (CACHE_LINE_NET == "GET.WAKE.TIME") {
            netCommand = GET_WAKE_TIME;
          }
          else if (CACHE_LINE_NET.startsWith("SET.LAMP.INTERVAL.")) {
            CACHE_LINE_NET = CACHE_LINE_NET.substring(18);
            BUFFER_NET = String(SET_LAMP_INTERVAL) + "." + CACHE_LINE_NET;
            netCommand = SET_LAMP_INTERVAL;
          }         
          else if (CACHE_LINE_NET == "GET.LAMP.INTERVAL") {
            netCommand = GET_LAMP_INTERVAL;
          }
          else if (CACHE_LINE_NET == "GET.ALL.SSIDS") 
            netCommand = GET_ALL_SSIDS;
          else if (CACHE_LINE_NET.startsWith("CONNECT.")) {
            CACHE_LINE_NET = CACHE_LINE_NET.substring(8);
            BUFFER_NET = String(CONNECT) + "." + CACHE_LINE_NET;
            netCommand = CONNECT;
          } 
          else if(CACHE_LINE_NET == "DISCONNECT"){
            netCommand = DISCONNECT;
          }
          else if(CACHE_LINE_NET == "GET.IS.CONNECTED"){
            netCommand = GET_IS_CONNECTED;
          }
          else if(CACHE_LINE_NET == "GET.IP.ADDRESS"){
            netCommand = GET_IP_ADDRESS;
          }
          else if(CACHE_LINE_NET == "GET.SSID"){
            netCommand = GET_SSID;
          }
          else if(CACHE_LINE_NET == "GET.STATUS"){
            netCommand = GET_STATUS;
          }
          else if(CACHE_LINE_NET == "GET.IP.INFO"){
            netCommand = GET_IP_INFO;
          }
          else if(CACHE_LINE_NET.startsWith("SET.IP.STATIC.")){
            CACHE_LINE_NET = CACHE_LINE_NET.substring(14);
            BUFFER_NET = String(SET_IP_STATIC) + "." + CACHE_LINE_NET;
            netCommand = SET_IP_STATIC;
          }
          else if(CACHE_LINE_NET == "SET.DHCP"){
            netCommand = SET_DHCP;
          }
          else if(CACHE_LINE_NET == "TCP.LOGOUT"){
            netCommand = TCP_LOGOUT;
          }
          else if(CACHE_LINE_NET == "GET.SAVED.SSIDS"){
            netCommand = GET_SAVED_SSIDS;
          }
          else if(CACHE_LINE_NET == "CLEAR.SAVED.SSIDS"){
            netCommand = CLEAR_SAVED_SSIDS;
          }
          else if(CACHE_LINE_NET == "HELP"){
            netCommand = GET_HELP;
          }
          else if(CACHE_LINE_NET.startsWith("SET.TCP.USER.")){
            CACHE_LINE_NET = CACHE_LINE_NET.substring(13);
            BUFFER_NET = String(SET_TCP_USER) + "." + CACHE_LINE_NET;
            netCommand = SET_TCP_USER;
          }
          else if(CACHE_LINE_NET == "GET.TCP.USERS"){
            netCommand = GET_TCP_USERS;
          }
          else if(CACHE_LINE_NET.startsWith("REMOVE.TCP.USER.")){
            CACHE_LINE_NET = CACHE_LINE_NET.substring(16);
            BUFFER_NET = String(REMOVE_TCP_USER) + "." + CACHE_LINE_NET;
            netCommand = REMOVE_TCP_USER;
          }
          else if(CACHE_LINE_NET == "CLEAR.TCP.USERS"){
            netCommand = CLEAR_TCP_USERS;
          }
          else {
            netCommand = UNKNOWN_CMD;
          }
        } else {
          netCommand = UNKNOWN_CMD;
        }

        // Executa o comando (mesmo switch, mas respostas vão pelo TCP)
        if (netCommand != UNKNOWN_CMD) {
          switch (netCommand) {
            case GET_TIME: {
              commandClient.print("HOUR:");
              commandClient.print(String(myTime.getTime().tm_hour));
              commandClient.print(" MINUTE:");
              commandClient.print(String(myTime.getTime().tm_min));
              commandClient.print(" SECOND:");
              commandClient.println(String(myTime.getTime().tm_sec));
              break;
            }
            case UPDATE_TIME: {
              myTime.updateTime(myWifi.getTimeFromNTP());
              break;
            }
            case SET_WAKE_TIME: {
              String buffer = BUFFER_NET;
              timeClass newTime = parseTime(buffer);
              myTime.setWakeTime(newTime);
              commandClient.print("\nWake time updated!\n");
              break;
            }
            case GET_WAKE_TIME: {
              commandClient.print("HOUR:");
              commandClient.print(String(myTime.getWakeTime().HOUR));
              commandClient.print(" MINUTE:");
              commandClient.print(String(myTime.getWakeTime().MINUTE));
              commandClient.print(" SECOND:");
              commandClient.println(String(myTime.getWakeTime().SECOND));

              break;
            }
            case SET_LAMP_INTERVAL: {
              String buffer = BUFFER_NET;
              timeClass newTime = parseTime(buffer);
              myTime.setLampInterval(newTime);
              commandClient.print("\nLamp interval updated!\n");
              break;
            }
            case GET_LAMP_INTERVAL: {
              commandClient.print("HOUR:");
              commandClient.print(String(myTime.getLampInterval().HOUR));
              commandClient.print(" MINUTE:");
              commandClient.print(String(myTime.getLampInterval().MINUTE));
              commandClient.print(" SECOND:");
              commandClient.println(String(myTime.getLampInterval().SECOND));
              break;
            }
            // Comandos WiFi continuam imprimindo no Serial USB (para não complicar)
            case GET_ALL_SSIDS:    myWifi.getAllSSIDS(); break;
            case CONNECT: {
              String current_buffer = BUFFER_NET;
              if (current_buffer.startsWith(String(CONNECT))) {
                current_buffer = removePrefix(current_buffer);
                int dotIndex = current_buffer.indexOf('.');
                String SSID = current_buffer.substring(0, dotIndex);
                String PASS = current_buffer.substring(dotIndex + 1);
                myWifi.connect(SSID, PASS);
              }
              break;
            }
            case DISCONNECT:       myWifi.disconnect(); break;
            case GET_IS_CONNECTED: myWifi.isConnected(); break;
            case GET_IP_ADDRESS:   myWifi.getIPAddress(); break;
            case GET_SSID:         myWifi.getSSID(); break;
            case GET_STATUS:       myWifi.getStatus(); break;
            case GET_IP_INFO:      myWifi.getIPInfo(); break;
            case SET_IP_STATIC: {
              String buffer = BUFFER_NET;
              myWifi.setStaticIP(buffer);
              break;
            }
            case SET_DHCP:         myWifi.setDHCP(); break;
            case TCP_LOGOUT: {
              commandClient.println("[TCP] Desconectando...");
              commandClient.stop();
              clientAuthenticated = false;
              Serial.println("[TCP] Cliente TCP desconectado.");
              break;
            }
            case GET_SAVED_SSIDS: {
              myWifi.printSavedNetworks(&commandClient);
              break;
            }
            case CLEAR_SAVED_SSIDS: {
              myWifi.clearSavedNetworks();
              commandClient.println("\n[WiFi] Todas as redes salvas foram removidas!");
              break;
            }
            case GET_HELP: {
              myWifi.printHelp(&commandClient);
              break;
            }
            case SET_TCP_USER: {
              String buffer = BUFFER_NET;
              if (buffer.startsWith(String(SET_TCP_USER))) {
                buffer = removePrefix(buffer);
                int dotIndex = buffer.indexOf('.');
                String username = buffer.substring(0, dotIndex);
                String password = buffer.substring(dotIndex + 1);
                myUsers.createUser(username, password, &commandClient);
              }
              break;
            }
            case GET_TCP_USERS: {
              myUsers.printTCPUsers(&commandClient);
              break;
            }
            case REMOVE_TCP_USER: {
              String buffer = BUFFER_NET;
              if (buffer.startsWith(String(REMOVE_TCP_USER))) {
                buffer = removePrefix(buffer);
                myUsers.removeUser(buffer, &commandClient);
              }
              break;
            }
            case CLEAR_TCP_USERS: {
              myUsers.clearAllUsers(&commandClient);
              break;
            }

            default:
              commandClient.println("Unknown command.");
              break;
          }
        }
        continue;
      }
      netBuf += c;
      if (netBuf.length() > 1024) netBuf = netBuf.substring(0, 1024);
    }
  }

  // Fecha cliente desconectado
  if (commandClient && !commandClient.connected()) {
    Serial.println("[TCP] Cliente TCP desconectado.");
    // Resetar variáveis de autenticação
    tcpInputUser = "";
    tcpInputPass = "";
    tcpAuthStep = 0;
    clientAuthenticated = false;
    commandClient.stop();
  }
}