// Clock commands
#define GET_TIME    00
#define UPDATE_TIME 01

// Wake time commands
#define SET_WAKE_TIME     10
#define GET_WAKE_TIME     11
#define SET_LAMP_INTERVAL 12
#define GET_LAMP_INTERVAL 13

// Wi-Fi commands
#define GET_ALL_SSIDS    20
#define CONNECT          21
#define DISCONNECT       22
#define GET_IS_CONNECTED 23
#define GET_IP_ADDRESS   24
#define GET_SSID         25
#define GET_STATUS       26

// Error commands
#define ERROR_INCOMPLETE 90

// Unknown command
#define UNKNOWN_CMD 99