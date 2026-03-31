#ifndef WIFICONTROLLER_HPP
#define WIFICONTROLLER_HPP

#include <WiFi.h>
#include <time.h>
#include <Arduino.h>
#include <LittleFS.h>
#include "../../utils/string/string-editions.hpp"

/**
 * @class wifiController
 * @brief Controlador de conectividade WiFi para o ESP32
 * @details Gerencia conexões WiFi, obtém tempo NTP, configura IP estático/DHCP
 * @note Utiliza LittleFS para armazenar redes salvas e credenciais
 */
class wifiController {

  private:
    /**
     * @brief Timeout para conexão WiFi em milissegundos
     * @note 20 segundos de espera máxima
     */
    const long CONNECT_TIMEOUT_MS = 20000;

    /**
     * @brief Número máximo de redes a armazenar em um scan
     */
    const static int MAX_SCAN = 64;

    /**
     * @brief Número máximo de redes salvas no arquivo LittleFS
     */
    const static int MAX_SAVED_NETWORKS = 10;
    
    /**
     * @brief Array com SSIDs encontrados no último scan
     */
    String scan_ssids[MAX_SCAN];

    /**
     * @brief Array com força de sinal (RSSI) de cada SSID
     * @details Valores negativos, quanto mais próximo de 0 melhor
     */
    int scan_rssis[MAX_SCAN];

    /**
     * @brief Array com tipos de autenticação de cada SSID
     * @details Valores como WIFI_AUTH_OPEN, WIFI_AUTH_WPA2_PSK, etc
     */
    int scan_auths[MAX_SCAN];

    /**
     * @brief Quantidade de redes encontradas no último scan
     */
    int scan_count = 0;

    /**
     * @brief Endereço IP atual do módulo WiFi
     */
    String IP;

    /**
     * @brief Flag indicando se está em modo IP estático
     * @details true = IP estático, false = DHCP dinâmico
     */
    bool isStatic;

    /**
     * @brief Carrega redes WiFi salvas do arquivo LittleFS
     * @details Lê o arquivo /saved_nets.txt com formato SSID|PASSWORD
     * @param ssidList Array de String para armazenar até 10 SSIDs
     * @param passList Array de String para armazenar até 10 senhas
     * @return int Quantidade de redes carregadas (0-10)
     * @note Arquivo pode não existir na primeira execução
     */
    int  loadSavedNetworks(String ssidList[10], String passList[10]);

    /**
     * @brief Salva redes WiFi no arquivo LittleFS
     * @details Escreve no arquivo /saved_nets.txt no formato SSID|PASSWORD
     * @param ssidList Array de String contendo SSIDs a salvar
     * @param passList Array de String contendo senhas correspondentes
     * @param count Quantidade de redes a salvar (máximo 10)
     * @return bool true se salvou com sucesso, false caso contrário
     * @note Sobrescreve o arquivo anterior completamente
     */
    bool saveSavedNetworks(String ssidList[10], String passList[10], int count);
  
  public:
    /**
     * @brief Construtor da classe wifiController
     * @details Inicializa o controlador WiFi e informa inicialização do LittleFS
     * @return Nenhum (construtor)
     * @note Deve ser chamado na inicialização do sistema
     */
    wifiController(void);

    /**
     * @brief Atualiza a lista de redes disponíveis
     * @details Realiza um scan de redes WiFi disponíveis nas proximidades
     * @return Nenhum (void)
     * @note Os resultados são armazenados em scan_ssids, scan_rssis, scan_auths
     * @note A função WiFi.scanDelete() é chamada automaticamente ao fim
     * @example myWifi.updateSSIDS();
     */
    void updateSSIDS(void);

    /**
     * @brief Obtém e imprime lista de todos os SSIDs disponíveis
     * @details Realiza scan e exibe resultado na Serial com SSID, RSSI e autenticação
     * @return Nenhum (void)
     * @note Chamada interna de updateSSIDS()
     * @note Saída: $WIFI: SSID,RSSI,AUTH_TYPE
     * @example myWifi.getAllSSIDS();
     */
    void getAllSSIDS(void);

    /**
     * @brief Conecta a uma rede WiFi específica
     * @details Realiza conexão com timeout de 20 segundos
     * @param ssid Nome da rede (SSID) a conectar
     * @param password Senha da rede
     * @return Nenhum (void)
     * @note Se bem-sucedido, salva as credenciais usando saveWiFiCredentials()
     * @note Saída Serial: $CNT (conectado) ou $ERR (erro)
     * @example myWifi.connect("MyNetwork", "password123");
     */
    void connect(String ssid, String password);

    /**
     * @brief Desconecta da rede WiFi atual
     * @details Finaliza a conexão WiFi e limpa o status
     * @return Nenhum (void)
     * @example myWifi.disconnect();
     */
    void disconnect(void);

    /**
     * @brief Verifica se está conectado à rede WiFi
     * @details Consulta o status da conexão WiFi
     * @return bool true se conectado, false caso contrário
     * @note Imprime status na Serial
     * @example if (myWifi.isConnected()) { ... }
     */
    bool isConnected(void);

    /**
     * @brief Obtém o endereço IP atual
     * @details Retorna o IP configurado (estático ou dinâmico via DHCP)
     * @return String Endereço IP no formato "192.168.1.100"
     * @note Retorna string vazia se não estiver conectado
     * @note Imprime na Serial com prefixo $IP
     * @example String ipAddr = myWifi.getIPAddress();
     */
    String getIPAddress(void);

    /**
     * @brief Obtém o nome da rede conectada
     * @details Retorna o SSID da rede WiFi atual
     * @return String Nome do SSID, ou string vazia se desconectado
     * @note Imprime na Serial com prefixo $SSID
     * @example String networkName = myWifi.getSSID();
     */
    String getSSID(void);

    /**
     * @brief Obtém o status atual da conexão WiFi
     * @details Retorna estado da conexão em formato textual
     * @return String Status como "CONNECTED", "DISCONNECTED", "IDLE", etc
     * @note Imprime na Serial com prefixo $STATUS
     * @example String status = myWifi.getStatus();
     */
    String getStatus(void);

    /**
     * @brief Sincroniza hora com servidor NTP
     * @details Obtém hora UTC do servidor NTP pool.ntp.org
     * @return struct tm Estrutura com hora, minuto, segundo do servidor
     * @note Não realiza ajuste de fuso horário (fica para timeController)
     * @note Imprime resultado na Serial
     * @example struct tm ntpTime = myWifi.getTimeFromNTP();
     */
    struct tm getTimeFromNTP(void);

    /**
     * @brief Obtém e exibe informações completas de configuração IP
     * @details Mostra IP, Gateway, Subnet Mask, DNS e modo (DHCP/Estático)
     * @return Nenhum (void)
     * @note Requer conexão WiFi ativa para funcionar
     * @note Saída apenas na Serial (não em TCP)
     * @example myWifi.getIPInfo();
     */
    void getIPInfo(void);

    /**
     * @brief Define configuração de IP estático
     * @details Configura IP, Subnet Mask, Gateway e DNS manualmente
     * @param ipConfig String no formato: IP.Subnet.Gateway.DNS
     * @return Nenhum (void)
     * @note Exemplo: "192.168.1.100.255.255.255.0.192.168.1.1.8.8.8.8"
     * @note Se DNS for 0.0.0.0, será substituído por 8.8.8.8 automaticamente
     * @note A conexão é reestablecida após a configuração
     * @example myWifi.setStaticIP("192.168.1.100.255.255.255.0.192.168.1.1.8.8.8.8");
     */
    void setStaticIP(String ipConfig);

    /**
     * @brief Ativa modo DHCP de configuração dinâmica de IP
     * @details Desativa IP estático e permite que roteador atribua IP automaticamente
     * @return Nenhum (void)
     * @note A conexão será reestablecida em seguida
     * @example myWifi.setDHCP();
     */
    void setDHCP(void);

    /**
     * @brief Salva credenciais de rede no arquivo LittleFS
     * @details Armazena SSID e senha para reconexão automática futura
     * @param ssid Nome da rede (SSID)
     * @param password Senha da rede
     * @return Nenhum (void)
     * @note Se rede já existe, a senha é atualizada
     * @note Limite de 10 redes salvas
     * @note Arquivo: /saved_nets.txt
     * @example myWifi.saveWiFiCredentials("MyNetwork", "password123");
     */
    void saveWiFiCredentials(String ssid, String password);

    /**
     * @brief Conecta automaticamente em redes salvas
     * @details Tenta conectar em cada rede salva até conseguir conexão
     * @return Nenhum (void)
     * @note Chamado automaticamente na inicialização (setup)
     * @note Realiza scan antes de tentar conexões
     * @note Timeout de 20 segundos por rede
     * @example myWifi.autoConnectToSavedNetworks();
     */
    void autoConnectToSavedNetworks(void);

    /**
     * @brief Exibe lista de redes salvas
     * @details Mostra todas as redes salvas no LittleFS
     * @param client Ponteiro para WiFiClient para saída TCP (nullptr = Serial)
     * @return Nenhum (void)
     * @note Se client for nullptr, exibe na Serial USB
     * @note Se client for válido, exibe no cliente TCP conectado
     * @example myWifi.printSavedNetworks(nullptr);  // Serial
     * @example myWifi.printSavedNetworks(&client);  // TCP
     */
    void printSavedNetworks(WiFiClient* client);

    /**
     * @brief Remove todas as redes salvas
     * @details Deleta o arquivo /saved_nets.txt do LittleFS
     * @return Nenhum (void)
     * @note Confirmação é enviada para Serial
     * @example myWifi.clearSavedNetworks();
     */
    void clearSavedNetworks(void);

    /**
     * @brief Exibe mensagem de ajuda com todos os comandos disponíveis
     * @details Mostra lista completa de comandos WiFi, clock, lamp, etc
     * @param client Ponteiro para WiFiClient para saída TCP (nullptr = Serial)
     * @return Nenhum (void)
     * @note Conteúdo grande - recomenda-se capturar a saída completa
     * @note Se client for nullptr, exibe na Serial USB
     * @note Se client for válido, exibe no cliente TCP conectado
     * @example myWifi.printHelp(nullptr);    // Serial
     * @example myWifi.printHelp(&client);    // TCP
     */
    void printHelp(WiFiClient* client);
};

#endif // WIFICONTROLLER_HPP
