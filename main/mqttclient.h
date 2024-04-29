#ifndef WIFIPROVISIONING_H
#include "WifiProvisioning.h"

// config.h


// Contents of mqttclient.h go here

// Define the configuration struct
typedef struct {
    char mqtt_host[64];
    int mqtt_port;
} mqttConfig;

// Function prototypes (if any)
void initialize_config(mqttConfig *config);
void set_mqtt_runtime_config(mqttConfig *config);
void set_mqtt_ok_to_run(bool oktorun);
static void mqtt_app_start();

#endif  // CONFIG_H