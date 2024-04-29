#ifndef WIFIPROVISIONING_H
#include "WifiProvisioning.h"
#include "mqttclient.h"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "mqtt_client.h"

bool ok_to_run = false;

static const char *TAG = "MQTT";

esp_mqtt_client_handle_t mqtt_client;

mqttConfig mConfig;

bool validate_mqtt_host_port(const char *mqtt_host, int mqtt_port) {
    // Check if MQTT host is empty or exceeds maximum length
    if (mqtt_host == NULL || strlen(mqtt_host) == 0 || strlen(mqtt_host) >= 64) {
        return false;
    }
    
    // Check if MQTT port is within valid range
    if (mqtt_port <= 0 || mqtt_port > 65535) {
        return false;
    }
    
    return true;
}

char* build_mqtt_uri(const char *mqtt_host, int mqtt_port) {
    // Perform basic sanity check on MQTT host and port
    if (!validate_mqtt_host_port(mqtt_host, mqtt_port)) {
        return NULL;
    }

    // Allocate memory for the MQTT URI (mqtt://<host>:<port>)
    char *mqtt_uri = malloc(strlen("mqtt://") + strlen(mqtt_host) + 6); // 6 for ":<port>" and null terminator
    if (mqtt_uri == NULL) {
        return NULL; // Memory allocation failed
    }

    // Build the MQTT URI
    sprintf(mqtt_uri, "mqtt://%s:%d", mqtt_host, mqtt_port);

    return mqtt_uri;
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
    }
    return ESP_OK;
}

void set_mqtt_ok_to_run(bool oktorun)
{
    ok_to_run  = set_mqtt_ok_to_run;
}

void set_mqtt_runtime_config(mqttConfig *config) {
    printf("MQTT Host Name: %s, Port: %d\n", config->mqtt_host, config->mqtt_port);
    // Further processing with config->mqtt_host and config->mqtt_port
}

static void mqtt_app_start()
{
    char *mqtt_uri = build_mqtt_uri(mConfig.mqtt_host, mConfig.mqtt_port);
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = mqtt_uri
        .event_handle = mqtt_event_handler_cb,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
}
