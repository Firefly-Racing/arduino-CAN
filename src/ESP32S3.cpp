// Copyright (c) Firefly Racing. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef ARDUINO_ARCH_ESP32

#include "ESP32S3.h"
#include <driver/gpio.h>
#include <driver/twai.h>
#include <esp_intr_alloc.h>
#include <soc/soc.h>
#include <FreeRTOS.h>
#include <task.h>

ESP32S3Class::ESP32S3Class() :
  CANControllerClass()
{
    _txPin = GPIO_NUM_5;
    _rxPin = GPIO_NUM_4;
    _loopback = false;
}

ESP32S3Class::~ESP32S3Class() {}

int ESP32S3Class::begin(long baudRate)
{
  CANControllerClass::begin(baudRate);

  _loopback = false;

	twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(_txPin, _rxPin, TWAI_MODE_NORMAL);
	twai_timing_config_t t_config;
	switch (baudRate) {
		case (long)1000E3:
			t_config = TWAI_TIMING_CONFIG_1MBITS();
			break;
		case (long)500E3:
			t_config = TWAI_TIMING_CONFIG_500KBITS();
			break;
		case (long)250E3:
			t_config = TWAI_TIMING_CONFIG_250KBITS();
			break;
		case (long)125E3:
			t_config = TWAI_TIMING_CONFIG_125KBITS();
			break;
		case (long)100E3:
			t_config = TWAI_TIMING_CONFIG_100KBITS();
			break;
		case (long)50E3:
			t_config = TWAI_TIMING_CONFIG_50KBITS();
			break;
		default:
			return 0;
	}
	twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

	//Install TWAI driver
	if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
		return 0;
	}

	//Start TWAI driver
	if (twai_start() != ESP_OK) {
		return 0;
	}

	xTaskCreate(ESP32S3Class::receive_task, "CANrecv", 4096, nullptr,  configMAX_PRIORITIES - 1, nullptr);

  return 1;
}

void ESP32S3Class::end()
{


  CANControllerClass::end();
}

int ESP32S3Class::endPacket()
{
	if (!CANControllerClass::endPacket()) {
		return -2;
	}

	twai_message_t msg;
	msg.extd = _txExtended;
	msg.rtr = _txRtr;
	msg.ss = 0;
	msg.self = 0;
	msg.dlc_non_comp = 0;
	msg.identifier = static_cast<uint32_t>(_txId);
	msg.data_length_code = static_cast<uint8_t>(_txLength);

	memcpy(&msg.data, _txData, _txLength);

	if (twai_transmit(&msg, 0) != ESP_OK) {
		return -1;
	}

    return 0;
}

int ESP32S3Class::parsePacket()
{
  return 0;
}

void ESP32S3Class::onReceive(void(*callback)(int))
{
	CANControllerClass::onReceive(callback);
}

int ESP32S3Class::filter(int id, int mask)
{
  return 0;
}

int ESP32S3Class::filterExtended(long id, long mask)
{
 return 0;
}

int ESP32S3Class::observe()
{

  return 0;
}

int ESP32S3Class::loopback()
{
  return 0;
}

int ESP32S3Class::sleep()
{
  return 0;
}

int ESP32S3Class::wakeup()
{
  return 0;
}

void ESP32S3Class::setPins(int rx, int tx)
{
  _rxPin = (gpio_num_t)rx;
  _txPin = (gpio_num_t)tx;
}

void ESP32S3Class::dumpRegisters(Stream& out)
{

}

void ESP32S3Class::handleInterrupt(twai_message_t message)
{
	_rxId = message.identifier;
	_rxExtended = message.extd;
	_rxDlc = message.data_length_code;
	_rxIndex = 0;
	_rxRtr = message.rtr;
	_rxLength = message.rtr ? 0 : _rxDlc;
	memcpy(_rxData, message.data, _rxLength);
	_onReceive(_rxLength);
}

[[noreturn]] void ESP32S3Class::receive_task(void*)
{
	Serial.println("can recv started");
	while (true) {
		twai_message_t message;
		Serial.println("can recv waiting");
		if (twai_receive(&message, portMAX_DELAY) != ESP_OK) {
			continue;
		}
		Serial.println("can recv recv");
		CAN.handleInterrupt(message);
	}
}

ESP32S3Class CAN;

#endif
