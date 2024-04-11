#pragma once

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace w132 {

struct W132Store {
  static void dataTrigger(W132Store *arg);
  char have_data;
  int message1[36];
  int message2[36];
  ISRInternalGPIOPin pin;
};

class W132 : public PollingComponent
{
public:
  void setup() override;
  void update() override;
  bool verifyChecksum(int bits[]);
  void decodeMessages();
  void set_direction_sensor(sensor::Sensor *direction_sensor) { xwind_direction = direction_sensor; }
  void set_speed_sensor(sensor::Sensor *speed_sensor) { xwind_speed = speed_sensor; }
  void set_gust_sensor(sensor::Sensor *gust_sensor) { xwind_gust = gust_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { xwind_temperature = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { xwind_humidity = humidity_sensor; }
  void set_pin(InternalGPIOPin *pin) { this->pin_ = pin; }

protected:
  W132Store store_;
  sensor::Sensor *xwind_direction{nullptr};
  sensor::Sensor *xwind_speed{nullptr};
  sensor::Sensor *xwind_gust{nullptr};
  sensor::Sensor *xwind_temperature{nullptr};
  sensor::Sensor *xwind_humidity{nullptr};
  InternalGPIOPin *pin_{nullptr};
};

}  // namespace w132
}  // namespace esphome
