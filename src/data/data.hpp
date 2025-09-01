#include <Arduino.h>
#include <EEPROM.h>

/**
 * @brief
 */
class data {

private:
  const int EEPROM_SIZE = 1024;
  const int MAX_SAVED = 5;          // máximo de redes salvas
  const int SSID_MAX = 32;          // bytes para SSID (inclui '\0')
  const int PASS_MAX = 64;          // bytes para senha (inclui '\0')
  const int ENTRY_SIZE = 1 + SSID_MAX + PASS_MAX; // 1 byte used flag + ssid + pass
  const char* NTP_SERVER = "pool.ntp.org";
  const long CONNECT_TIMEOUT_MS = 20000;
  const long SCAN_TIMEOUT_MS = 10000;
  const long NTP_WAIT_MS = 10000;
  const long SYNC_INTERVAL_MS = 3600000UL; // 1 hora
  const long TZ_OFFSET_SECONDS = -3 * 3600; // fuso horário (ex: -3h para America/Sao_Paulo)

public:
  data(void);

  ~data(void);

  void saveNewNetwork(const String &ssid, const String &pass);

};