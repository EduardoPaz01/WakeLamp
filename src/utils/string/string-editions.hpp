#include <Arduino.h> 
#include <WiFi.h>

/**
 * Convert authentication type to string
 */
String authToString(int auth);

/**
 * Trim whitespace from string
 */
String trimStr(const String &s);

/**
 * Remove the prefix up to the first dot
 */
String removePrefix(String str);