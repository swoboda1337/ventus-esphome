#include "w132.h"

namespace esphome {
namespace w132 {

void IRAM_ATTR HOT W132Store::dataTrigger(W132Store *arg)
{
  static volatile uint32_t lastTrigger;

  static uint8_t bitPos = 0;
  static uint8_t messageNum;
  // there are always sent 6 messages. For temperature/humidity, the message is repeatet 6 times. For wind, 2 messages are repeatet 3 times each
  static int message1Bits[36];
  static int message2Bits[36];

  static uint32_t  now;
  static uint32_t  duration;

  now = micros();
  duration = now - lastTrigger;
  //ESP_LOGD("custom", "Now: %d, Last, %d, Duration: %d", now, lastTrigger, duration);
  lastTrigger = now;

  if (duration > 30000)
  { // a news block of messages begins
      // ESP_LOGD("custom", "New message");
      messageNum = 1;
  }

  if (duration > 7000)
  { // ~9 ms = sync signal
      //ESP_LOGD("custom", "Sync signal");
      if (bitPos-1 == 36)
      { // we got a full message
          if (messageNum == 1)
          { // 1st message completed
              messageNum = 2;
              // ESP_LOGD("custom", "First message completed");
          }
          else if (messageNum == 2)
          { // 2nd message completed
              //decodeMessages();
              // ESP_LOGD("custom", "Second message completed");

              // static char msg1[36];
              // static char msg2[36];
              // for (int i = 0;i<36;i++)
              // {
              //     msg1[i] = char(message1Bits[i]);
              //     msg2[i] = char(message2Bits[i]);
              // }
              // ESP_LOGD("custom", "Message 1 Bits: %s", msg1);
              // ESP_LOGD("custom", "Message 2 Bits: %s", msg2);

              memcpy(arg->message1, message1Bits, 36 * sizeof(int));
              memcpy(arg->message2, message2Bits, 36 * sizeof(int));
              arg->have_data = 1;
              messageNum = 0;
          }
      }
      bitPos = 1; // message beginns
      return;
  }

  if (messageNum < 0)
  {
    // ESP_LOGD("custom", "Repeated message");
    return; // ignore repeated messages
  }
  if (bitPos == 0)
  {
    // ESP_LOGD("custom", "Invalid message");
    return; // invalid message, ignored
  }

  if (messageNum == 1)
  {
    message1Bits[bitPos-1] = (duration > 3300); // 2.2ms=LOW, 4.4ms = HIGH bits
  }
  else if(messageNum == 2)
  {
    message2Bits[bitPos-1] = (duration > 3300); // 2.2ms=LOW, 4.4ms = HIGH bits
  }
  bitPos++;
  if (bitPos-1 > 36)
      bitPos = 0; // message too long -> invalid
}

bool W132::verifyChecksum(int bits[])
{
  int checksum = 0xf;
  for (int i = 0; i < 8; i++)
  {
    checksum -= bits[i * 4] | bits[i * 4 + 1] << 1 | bits[i * 4 + 2] << 2 | bits[i * 4 + 3] << 3;
  }
  checksum &= 0xf;
  int expectedChecksum = bits[32] | bits[33] << 1 | bits[34] << 2 | bits[35] << 3;
  return checksum == expectedChecksum;
}

void W132::decodeMessages()
{
  /*char msg1[36];
  char msg2[36];
  for (int i = 0;i<36;i++)
  {
      sprintf( &msg1[i], "%d", message1[i]);
      sprintf( &msg2[i], "%d", message2[i]);
  }
  ESP_LOGD("custom", "Message 1 Copy: %s", msg1);
  ESP_LOGD("custom", "Message 2 Copy: %s", msg2);*/

  if (!verifyChecksum(this->store_.message1))
  {
    ESP_LOGD("custom", "Checksum mismatch in message #1");
    //return;
  }

  if (this->store_.message1[9] == 1 && this->store_.message1[10] == 1)
  { // wind data (2 messages)

    if (!verifyChecksum(this->store_.message2))
    {
        ESP_LOGD("custom", "Checksum mismatch in message #2");
        //return;
    }

    float windSpeed = (this->store_.message1[24] | this->store_.message1[25] << 1 | this->store_.message1[26] << 2 | this->store_.message1[27] << 3 |
                       this->store_.message1[28] << 4 | this->store_.message1[29] << 5 | this->store_.message1[30] << 6 | this->store_.message1[31] << 7) *
                      0.2f;
    ESP_LOGD("custom", "Average wind speed: %f m/s", windSpeed);

    if (xwind_speed != nullptr)   xwind_speed->publish_state(windSpeed * 3.6);

    float windGust = (this->store_.message2[24] | this->store_.message2[25] << 1 | this->store_.message2[26] << 2 | this->store_.message2[27] << 3 |
                      this->store_.message2[28] << 4 | this->store_.message2[29] << 5 | this->store_.message2[30] << 6 | this->store_.message2[31] << 7) *
                     0.2f;
    ESP_LOGD("custom", "Max wind speed: %f m/s", windGust);

    if (xwind_gust != nullptr)   xwind_gust->publish_state(windGust * 3.6);

    int windDirection = (this->store_.message2[15] | this->store_.message2[16] << 1 | this->store_.message2[17] << 2 | this->store_.message2[18] << 3 |
                         this->store_.message2[19] << 4 | this->store_.message2[20] << 5 | this->store_.message2[21] << 6 | this->store_.message2[22] << 7 |
                         this->store_.message2[23] << 8);
    ESP_LOGD("custom", "Wind direction: %d°", windDirection);

    if (xwind_direction != nullptr)   xwind_direction->publish_state(windDirection);
  }
  else
  { // temperature/humidity in both messages
    int temperatureRaw = (this->store_.message1[12] | this->store_.message1[13] << 1 | this->store_.message1[14] << 2 | this->store_.message1[15] << 3 |
                          this->store_.message1[16] << 4 | this->store_.message1[17] << 5 | this->store_.message1[18] << 6 | this->store_.message1[19] << 7 |
                          this->store_.message1[20] << 8 | this->store_.message1[21] << 9 | this->store_.message1[22] << 10 | this->store_.message1[23] << 11);
    if (temperatureRaw & 0x800)
        temperatureRaw += 0xF000; // negative number, 12 to 16 bit
    float temperature = temperatureRaw * 0.1f;
    ESP_LOGD("custom", "Temperature: %f °C", temperature);

    if (xwind_temperature != nullptr)   xwind_temperature->publish_state(temperature);

    int humidity = (this->store_.message1[24] | this->store_.message1[25] << 1 | this->store_.message1[26] << 2 | this->store_.message1[27] << 3) +
                   (this->store_.message1[28] | this->store_.message1[29] << 1 | this->store_.message1[30] << 2 | this->store_.message1[31] << 3) * 10;
    ESP_LOGD("custom", "Humidity: %d percent", humidity);

    if (xwind_humidity != nullptr)   xwind_humidity->publish_state(humidity);
  }
}

void W132::setup() {
  this->set_update_interval(1000);
  this->pin_->setup();
  this->store_.pin = pin_->to_isr();
  this->pin_->attach_interrupt(W132Store::dataTrigger, &this->store_, gpio::INTERRUPT_FALLING_EDGE);
}

void W132::update() {
  if (this->store_.have_data) {
    decodeMessages();
    this->store_.have_data = 0;
  }   
}

}
}
