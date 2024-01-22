#include <iostream>
#include <memory>

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "sw_uart.h"

#define UART_TX_PIN  7u
#define UART_LED_PIN 3u

std::shared_ptr<sw_comm::uart_tx> _uart{};

void setup()
{
    Serial.begin(115200u);
    vTaskStartScheduler();
    _uart = std::make_shared<sw_comm::uart_tx>(UART_TX_PIN, 2);
}

void loop()
{
    /* do nothing */
    __WFI();
}
