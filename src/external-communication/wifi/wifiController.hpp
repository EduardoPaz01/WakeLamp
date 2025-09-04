#ifndef WIFICONTROLLER_HPP
#define WIFICONTROLLER_HPP

#include <WiFi.h>
#include <time.h>
#include <Arduino.h>
#include "../../utils/string/string-editions.hpp"

/**
 * Class representing a WiFi connection
 */
class wifiController {

private:
  const long CONNECT_TIMEOUT_MS = 20000;
  const static int MAX_SCAN = 64;
  String scan_ssids[MAX_SCAN];
  int scan_rssis[MAX_SCAN];
  int scan_auths[MAX_SCAN];
  int scan_count = 0;
  String IP;

public:
  /**
   * Constructor for the wifi class
   */
  wifiController(void);

  /**
   * Update the list of saved SSIDs
   */
  void updateSSIDS(void);

  /**
   * Get the SSID of the connected WiFi
   */
  void getAllSSIDS(void);

  /**
   * Begin the WiFi connection
   */
  void connect(String ssid, String password);

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

  /**
   * Get the schedule online
   */
  struct tm getTimeFromNTP(void);
};

#endif // WIFICONTROLLER_HPP
