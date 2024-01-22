# NRF52840 ItsyBitsy Software Communication Protocols

<hr>

### Software UART

This is a software, bit-banging, non-blocking API driven by the onboard timer peripheral on the nrf52840 device.

### Requirements
* UART RX [USB to UART or any other device]
* NRF52840 based development board
* Wire
* PlatformIO IDE

### Instructions

1. Open the project in PlatformIO IDE. Let IDE update dependencies.
2. Build and upload project.
3. Connect UART RX [host] to SW UART TX [default: pin 7u].
4. Listen to UART at 9600 baud, 8N1 configuration.

### Output 

```
  ItsyBitsy SW UART!
  ItsyBitsy SW UART!
  ItsyBitsy SW UART!
  ItsyBitsy SW UART!
  ItsyBitsy SW UART!
  ItsyBitsy SW UART!
  ItsyBitsy SW UART!
```

<hr>
