#include <Arduino.h>
#include <FreeRTOS.h>

#include <semphr.h>

#include <nrfx.h>
#include <nrfx_my_config.h>
#include <nrfx_timer.h>

#include "sw_uart.h"

namespace sw_comm {

/**
 * @brief Thread for UART FIFO
 */
void uart_tx::_run(void* args)
{
    uart_tx* _this = (uart_tx*)args;

    bool fresh = false;
    printf("UART TX task started!\n");

    size_t str_sz = _this->_str.size();
    _this->_data = _this->_str[_this->ptr++];

    while (1) {
        /* when data available start timer */
        if (_this->_data) {
            nrfx_timer_enable(&_this->_timer);
            _this->_data_cnt = 8u;
            fresh = true;
        }

        /* consume all data */
        while (fresh) {
            uart_tx_state_t next_state = _this->curr_state;

            if (xSemaphoreTake(_this->_clk_sem, portMAX_DELAY)) {
                /* lightweight FSM */
                uint8_t data = 0u;
                switch (_this->curr_state) {
                    case (uart_tx_state_t::UART_TX_START):
                        /* start bit */
                        data = 0u;
                        next_state = uart_tx_state_t::UART_TX_DATA;
                        break;

                    case (uart_tx_state_t::UART_TX_DATA):
                        // fix for zero-index
                        if (_this->_data_cnt == 1u) {
                            next_state = uart_tx_state_t::UART_TX_STOP;
                        }
                        data = _this->_data & 1u;
                        _this->_data = _this->_data >> 1u;
                        _this->_data_cnt--;
                        break;

                    case (uart_tx_state_t::UART_TX_PARITY):
                        /* @todo parity bit */
                        data = 1u;
                        next_state = uart_tx_state_t::UART_TX_STOP;
                        break;

                    case (uart_tx_state_t::UART_TX_STOP):
                        /* stop bit */
                        data = 1u;
                        next_state = uart_tx_state_t::UART_TX_START;
                        break;

                    default:
                        /* do nothing or RESET ¯\_(ツ)_/¯ */
                        continue;
                        break;
                }

                /* write out current bit */
                digitalWrite(_this->_pin, data & 1u);
                _this->curr_state = next_state;

                /* do preparations for next byte */
                if (next_state == uart_tx_state_t::UART_TX_START) {
                    /* stop the timer */
                    nrfx_timer_disable(&_this->_timer);

                    /* reset string, if completed */
                    if (_this->ptr >= str_sz) {
                        _this->ptr = 0u;
                        vTaskDelay(1000);
                    }

                    /* load next byte */
                    _this->_data = _this->_str[_this->ptr++];
                    printf("byte = %c\n", _this->_data);
                    fresh = false;
                }
            }
        }

        /* this helps nrf from crashing, WDT tripping most likely */
        taskYIELD();
    }
}

/**
 * @brief Handler for timer events
 * @param event_type
 * @param p_context
 */
void uart_tx::_handler(nrf_timer_event_t event_type, void* p_context)
{
    uart_tx* _this = (uart_tx*)p_context;

    switch (event_type) {
        case NRF_TIMER_EVENT_COMPARE0:
            digitalWrite(_this->_led, _this->_clk_state);
            xSemaphoreGive(_this->_clk_sem);
            _this->_clk_state = !_this->_clk_state;
            break;
        default:
            break;
    }
}

/**
 * @brief Initialize timer peripheral
 */
void uart_tx::_nrfx_timer_init()
{
    nrfx_timer_config_t cfg = NRFX_TIMER_DEFAULT_CONFIG;
    // cfg.bit_width = NRF_TIMER_BIT_WIDTH_16;
    cfg.frequency = NRF_TIMER_FREQ_1MHz;
    cfg.interrupt_priority = 1u;
    cfg.p_context = this;

    int rc = nrfx_timer_init(&_timer, &cfg, uart_tx::_handler);
    if (rc != NRFX_SUCCESS) {
        printf("Error: NRF timer not inited 0x%X\n", rc);
    } else {
        printf("NRFX Timer inited!\n");
    }

    uint32_t ticks = nrfx_timer_us_to_ticks(&_timer, UART_BAUD_RATE_PERIOD_US(UART_BAUD_RATE_9600_BPS));

    nrfx_timer_extended_compare(&_timer, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
    printf("NRFX CC0 compare inited!\n");

    NRFX_IRQ_ENABLE(TIMER2_IRQn);
    printf("NRFX CC0 interrupt inited!\n");

    nrfx_timer_enable(&_timer);
    printf("NRFX Timer enabled!\n");
}

/**
 * @brief
 * @param tx_pin
 * @param timer
 */
uart_tx::uart_tx(const uint32_t& tx_pin, const uint32_t& timer) : _pin(tx_pin)
{
    _timer = NRFX_TIMER_INSTANCE(2);

    pinMode(_pin, OUTPUT);
    pinMode(_led, OUTPUT);

    _nrfx_timer_init();

    _clk_sem = xSemaphoreCreateBinary();
    configASSERT(_clk_sem);

    int rc = xTaskCreate(uart_tx::_run, "uart_tx", 1024u, this, 1u, &_task_handle);
    configASSERT(rc == pdTRUE);

    printf("NRFX Software UART TX inited at pin %d\n", _pin);
}

} // namespace sw_comm