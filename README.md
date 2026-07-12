# devops_easy_connect

A small ESP-IDF component for simplifying Wi-Fi setup in ESP32 applications.

It provides a simple interface to initialize Wi-Fi, connect to a station, and run a callback when the connection is established.

## Features
- Initialize Wi-Fi and network stack
- Connect to a configured SSID/password
- Trigger a callback after a successful connection

## Usage
Call `app_wifi_init()` to configure the connection and `app_wifi_connect()` to start the connection process.
