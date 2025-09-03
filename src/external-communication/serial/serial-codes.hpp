// Clock commands
#define GET_TIME    000
#define UPDATE_TIME 001

// Wake time commands
#define SET_WAKE_TIME     010
#define GET_WAKE_TIME     011
#define SET_LAMP_INTERVAL 012
#define GET_LAMP_INTERVAL 013

// Wi-Fi commands
#define GET_ALL_SSIDS    020
#define CONNECT          021
#define DISCONNECT       022
#define GET_IS_CONNECTED 023
#define GET_IP_ADDRESS   024
#define GET_SSID         025
#define GET_STATUS       026

// Error commands
#define ERROR_INCOMPLETE 990

// Unknown command
#define UNKNOWN_CMD 999