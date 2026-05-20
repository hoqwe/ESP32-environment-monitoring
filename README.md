# esp32-environment-monitoring

Monitoring temperature, humidity, and pressure using an ESP32 with a BME280 sensor and an ST7789 display showing three round gauges.

How it looks:

![example](https://github.com/user-attachments/assets/249d51d3-dce5-425d-8e5f-e5ab2d43d371)

**Parts used:**

- ESP32 DevKit V1, 30 pins
- BME280 - pressure, humidity, and temperature sensor
- ST7789 1.3" 240x240 IPS display
- Breadboard
- Jumper wires

**Libraries used:**

- Adafruit BME280 Library by Adafruit, v2.3.0. Way to interact with the BME280 sensor.
- TFT_eSPI by Bodmer, v2.5.43. Way to interact with the ST7789 display.

## Setup

The TFT_eSPI library uses different configuration files for various microcontroller-display combinations. Because of this, some configuration tweaking is required at the first-time setup:

1. Locate the `TFT_eSPI` library folder. With the Arduino IDE on Windows, the default path is `\Users\<username>\Documents\Arduino\libraries\TFT_eSPI`.
2. `\User_Setup_Select.h` - comment the default setup line `#include <User_Setup.h>` and uncomment the custom setup line `#include <User_Setups/Setup24_ST7789.h>`. This custom setup file contains all the settings for the ESP32 + ST7789 combination, though some additional tuning is still needed.
3. `\User_Setups\Setup24_ST7789` - replace this config with the one in the repository.

Pinout ST7789-ESP32:

```
GND -> GND
VCC -> 3V3
SCL -> GPIO18 (VSPI CLK)
SDA -> GPIO23 (VSPI MOSI)
RES -> GPIO4  or any
DC  -> GPIO15 or any
BLK -> not connected or 3V3 or any for backlight control
```

Pinout BME280-ESP32:

```
VIN -> 3V3
GND -> GND
SCL -> GPIO22 (I2C SCL)
SDA -> GPIO21 (I2C SDA)
```

Now you're ready to upload the code :)

---

Everything below is optional. In this section you can set up a server on your PC to store sensor readings and display them in a browser dashboard:

`<pic>`

### PostgreSQL setup

Make sure PostgreSQL is installed and running before executing these commands.

Create a database user, create the database, and initialize the schema:

```bash
createuser iot_user --pwprompt --username postgres

createdb iot_dashboard --owner iot_user --username postgres

psql --host localhost --username iot_user --dbname iot_dashboard --file init.sql
```

Then copy the environment file:

```bash
cp .env.example .env
```

Edit `.env` and update the database credentials if needed.
