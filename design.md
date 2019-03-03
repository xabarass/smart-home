# Smart home system

This system is designed to provide modular design for managing different types of sensors in a smart home environment. 
Idea is to provide centralizes solution for aggregatin all sensors data and exposing it through web ui and api that can be controlled as an Amazon Alexa smart home skill


## Design

This chapter talks about design of different modules.
Idea is to have a separate daemon that is handling small group of sensors. They will expose custom API and communicate with centralizes server over ZeroMQ.

## Temperature and humidity daemon

This daemon is used to control [this](https://www.sensirion.com/en/environmental-sensors/humidity-sensors/development-kit/) sensor.
C application is based on [gattlib](https://github.com/labapart/gattlib) that provides bluetooth LE GATT profile reading/writing functionality.


