# Missing Sensors and Implementation Requirements

## Required Weather Sensors (per project statement)

1. Temperature (Température)
2. Relative Humidity (Humidité relative)
3. Barometric Pressure (Pression barométrique)
4. Sunlight (Ensoleillement)
5. Precipitation (Quantité de précipitation)
6. Wind Direction (Direction du vent)
7. Wind Speed (Vitesse du vent)

## Present in `00-esp32-emitter.ino` (via includes)
- Temperature (via humidity and/or pressure sensor)
- Humidity
- Barometric Pressure
- Light/Sunlight

## Sensors Missing in Code

### 1. Precipitation (Pluviomètre)
- **What you need to add:**
  - Interface a rain gauge (typically a tipping-bucket type).
  - Use a digital (interrupt) pin to count "tips" (each tip = known mm of rain).
  - Implement counter logic in the code. Reset after reporting if needed.
  - Debounce in software if mechanical.

### 2. Wind Speed (Anémomètre)
- **What you need to add:**
  - Interface a wind speed sensor (cup anemometer with reed/magnet switch).
  - Use a digital (interrupt) pin to count pulses over a set period.
  - Calculate speed using sensor's conversion factor.

### 3. Wind Direction (Girouette)
- **What you need to add:**
  - Interface a wind vane (analog sensor or resistor network for direction).
  - Read voltage value and map it to cardinal directions (e.g., N, NE, E, ...).
  - Typically uses an analog input pin and a lookup table for direction.

---

## Table of Required Sensors (Summary)

| Sensor           | Implemented? | Implementation Required                       |
|------------------|:------------:|-----------------------------------------------|
| Temperature      | Yes          | Already in code (humidity/pressure sensors)   |
| Relative Humidity| Yes          | Already in code                               |
| Pressure         | Yes          | Already in code                               |
| Sunlight         | Yes          | Already in code                               |
| Wind Direction   | Yes          | Add wind vane + analog reading/mapping logic  |
| Wind Speed       | No           | Add anemometer + pulse counting logic         |
| Precipitation    | No           | Add rain gauge + counter logic                |

---

For demonstration, you only need to implement the sensors marked "No" above. See your project statement for extra requirements for commercial deployments (e.g. consider robustness, alternate component choices, etc.).
