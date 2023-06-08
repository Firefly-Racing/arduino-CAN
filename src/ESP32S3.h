// Copyright (c) Firefly Racing. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef ARDUINO_ARCH_ESP32

#ifndef ESP32_S3_H
#define ESP32_S3_H

#include "CANController.h"
#include <driver/twai.h>

#define DEFAULT_CAN_RX_PIN GPIO_NUM_4
#define DEFAULT_CAN_TX_PIN GPIO_NUM_5

class ESP32S3Class : public CANControllerClass {

public:
  ESP32S3Class();
  virtual ~ESP32S3Class();

  virtual int begin(long baudRate);
  virtual void end();

  virtual int endPacket();

  virtual int parsePacket();

  virtual void onReceive(void(*callback)(int));

  using CANControllerClass::filter;
  virtual int filter(int id, int mask);
  using CANControllerClass::filterExtended;
  virtual int filterExtended(long id, long mask);

  virtual int observe();
  virtual int loopback();
  virtual int sleep();
  virtual int wakeup();

  void setPins(int rx, int tx);

private:
  void reset();

  void handleInterrupt(twai_message_t message);

	[[noreturn]] static void receive_task(void*);

private:
  gpio_num_t _rxPin;
  gpio_num_t _txPin;
  bool _loopback;
};

extern ESP32S3Class CAN;

#endif

#endif
