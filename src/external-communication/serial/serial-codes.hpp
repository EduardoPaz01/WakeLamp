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

// IP Configuration commands
#define GET_IP_INFO           27
#define SET_IP_STATIC         28
#define SET_DHCP              29

// TCP Connection commands
#define TCP_LOGOUT              30
#define TCP_DISCONNECT_CLIENT   31

// Saved Networks commands
#define GET_SAVED_SSIDS    40
#define CLEAR_SAVED_SSIDS  41

// TCP User Management commands
#define SET_TCP_USER       60
#define GET_TCP_USERS      61
#define REMOVE_TCP_USER    62
#define CLEAR_TCP_USERS    63

// Help command
#define GET_HELP           50

// Error commands
#define ERROR_INCOMPLETE 90

// Unknown command
#define UNKNOWN_CMD 99