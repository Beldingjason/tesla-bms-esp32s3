#ifndef SECRETS_H_
#define SECRETS_H_

// Copy this file to include/secrets.h and fill in real values.
// secrets.h is gitignored.
//
// To run BMS-only with no WiFi or MQTT: leave WIFI_SSID empty.
// To run with WiFi but no MQTT: set WIFI_SSID, leave MQTT_HOST empty.
// To enable Home Assistant MQTT integration: set both.
//
// Multiple packs on the same network: each pack auto-derives a unique id
// from the ESP32's MAC address, so flashing the same secrets.h to several
// boards just works. Optionally set MQTT_NODE_NAME to give each pack a
// friendly label in HA (e.g. "Tesla BMS Garage", "Tesla BMS Shed").

#define WIFI_SSID      ""
#define WIFI_PASSWORD  ""

#define MQTT_HOST      ""
#define MQTT_PORT      1883
// Leave MQTT_USER / MQTT_PASSWORD empty for anonymous auth (broker must
// allow it — e.g. Mosquitto with `allow_anonymous true`).
#define MQTT_USER      ""
#define MQTT_PASSWORD  ""

// Friendly name shown in Home Assistant. Leave as default if you only have
// one pack; override per-board for multi-pack setups.
#define MQTT_NODE_NAME "Tesla BMS"

#endif // SECRETS_H_
