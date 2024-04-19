ESPHome Ventus W132 wind sensor / anemometer component, code taken from random source, code quality is questionable, ported for someone else, haven't cleaned it up, no hardware to test with
  
    external_components:
      - source:
          type: git
          url: https://github.com/swoboda1337/ventus-esphome
          ref: main
        refresh: 1d
    
    w132:
      pin: 13
    
    sensor:
    - platform: w132
      xwind_direction:
        name: "Direction"
      xwind_gust:
        name: "Gust"
      xwind_speed:
        name: "Speed"
      xwind_temperature:
        name: "Temperature"
      xwind_humidity:
        name: "Humidity"
