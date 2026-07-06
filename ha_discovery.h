#pragma once

const char LED1_DISCOVERY[] PROGMEM = R"rawliteral(
{
    "name":"LED Strip",
    "uniq_id":"raintower_led1",

    "cmd_t":"home/raintower/light/led1/set",
    "stat_t":"home/raintower/light/led1/state",

    "pl_on":"ON",
    "pl_off":"OFF",

    "device":{
        "ids":["raintower"],
        "name":"RainTower",
        "manufacturer":"Alexander",
        "model":"ESP32-P4 Controller"
    }
}
)rawliteral";

const char RELAY1_DISCOVERY[] PROGMEM = R"rawliteral(
{
...
}
)rawliteral";