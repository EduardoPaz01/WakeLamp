#include <WiFi.h>

/**
 * Class representing a WiFi connection
 */
class wifi {

private:
  /** 
   * SSID of the WiFi network
   */
  const char* ssid;

  /** 
   * Password of the WiFi network
   */
  const char* password;

public:
  /**
   * Constructor for the wifi class
   */
  wifi(void);

  /**
   * Get the SSID of the connected WiFi
   */
  String getAllSSID();

  /**
   * Begin the WiFi connection
   */
  void connect(const char* ssid, const char* password);

  /**
   * Disconnect from the WiFi network
   */
  void disconnect();

  /**
   * Check if the WiFi is connected
   */
  bool isConnected();

  /**
   * Get the IP address of the WiFi connection
   */
  String getIPAddress();

  /**
   * Get the SSID of the connected WiFi
   */
  String getSSID();

  /**
   * Get the status of the WiFi connection
   */
  String getStatus();
};