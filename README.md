# Pico Weather Base Station

Base station for the pico weather station.

## Cloning and building the project

Clone the project with FreeRTOS and sensor submodules to get the pico functionality:

```
git clone https://github.com/eremiticengineer/pico-weather-base-station
cd pico-weather-base-station
git submodule update --init --progress --jobs 4
git -C lib/FreeRTOS-Kernel submodule update --init --recursive --progress
cp src/secrets.example.hpp src/secrets.hpp
./build_project
```

## FreeRTOSConfig.h

This file customises FreeRTOS for your project. The file:

```
include/FreeRTOSConfig.h
```

is this one from the pico-examples:

```
pico-examples/freertos/FreeRTOSConfig_examples_common.h
```

## References

* [Task priorites](https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/03-Task-priorities)
* [uxTaskGetStackHighWaterMark](https://www.freertos.org/Documentation/02-Kernel/04-API-references/03-Task-utilities/04-uxTaskGetStackHighWaterMark)
