/**
 * @file sw_uart.h
 *
 * @brief Software UART [Non-Blocking]
 *
 *
 */

#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <cstdint>

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <nrfx_timer.h>

namespace sw_comm {

#define UART_BAUD_RATE_5_BPS      5u
#define UART_BAUD_RATE_9600_BPS   9600u
#define UART_BAUD_RATE_115200_BPS 115200u

#define DATA_NOMINAL 0b01000001

#define UART_BAUD_RATE_PERIOD_US(x) ((1000u * 1000u) / x)

class uart_tx
{
  public:
    typedef enum class _uart_tx_state_t {
        UART_TX_START,
        UART_TX_DATA,
        UART_TX_PARITY,
        UART_TX_STOP,
    } uart_tx_state_t;

  private:
    /* Private data members */
    uint32_t _pin{};
    uint32_t _led{ 3u };

    nrfx_timer_t _timer{};
    bool _clk_state{};
    SemaphoreHandle_t _clk_sem{};

    uart_tx_state_t curr_state{ uart_tx_state_t::UART_TX_START };

    std::string _str = "ItsyBitsy SW UART!\r\n";
    size_t ptr{};
    uint8_t _data{};
    size_t _data_cnt{};

    TaskHandle_t _task_handle{};

    /* Private API */
    void _nrfx_timer_init();
    static void _handler(nrf_timer_event_t event_type, void* p_context);

    static void _run(void* _this);

  public:
    /* Public API */
    uart_tx(const uint32_t& tx_pin, const uint32_t& timer);
};

} // namespace sw_comm