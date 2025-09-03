#ifndef WIFICONTROLLER_HPP
#define WIFICONTROLLER_HPP

#include <WiFi.h>
#include <Arduino.h>
#include "../../utils/string/string-editions.hpp"

/**
 * Class representing a WiFi connection
 */
class wifiController {

private:
  const static int MAX_SCAN = 64;
  String scan_ssids[MAX_SCAN];
  int scan_rssis[MAX_SCAN];
  int scan_auths[MAX_SCAN];
  int scan_count = 0;

public:
  /**
   * Constructor for the wifi class
   */
  wifiController(void);

  /**
   * Get the SSID of the connected WiFi
   */
  void getAllSSIDS(void);

  /**
   * Begin the WiFi connection
   */
  void connect(const char* ssid, const char* password);

  /**
   * Disconnect from the WiFi network
   */
  void disconnect(void);

  /**
   * Check if the WiFi is connected
   */
  bool isConnected(void);

  /**
   * Get the IP address of the WiFi connection
   */
  String getIPAddress(void);

  /**
   * Get the SSID of the connected WiFi
   */
  String getSSID(void);

  /**
   * Get the status of the WiFi connection
   */
  String getStatus(void);
};

#endif // WIFICONTROLLER_HPP
