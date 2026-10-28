# EcoBrick Machine

Firmware for the EcoBrick Machine, an automated production line that converts loose plastic bottles into compacted, sealed bricks. The controller runs on a 38-pin ESP32 DevKit and coordinates one stepper motor, two linear actuators, a 220 V soldering iron, an AC shredder motor, and a load cell for bottle fill detection.

## How It Works

Plastic bottles are shredded into flakes, compacted inside a container bottle, and then heat-sealed at the neck to form a solid brick shape. The machine moves through nine states per cycle:

1. **Feed** — stepper positions the bottle under the press
2. **Wait for fill** — load cell verifies bottle has sufficient plastic flakes
3. **Press extend** — compression actuator squeezes the flakes
4. **Press hold** — compression is maintained while the plastic relaxes
5. **Press retract** — press returns to rest position
6. **Seal extend** — iron carrier lowers the heated tip onto the bottle neck
7. **Seal hold** — iron powers the PET neck material to melt and fuse
8. **Seal retract** — iron lifts away; the seal cools
9. **Return** — stepper backs to the start position
10. **Pause** — brief idle before the next cycle

A full cycle takes roughly 12 seconds. The complete process is documented in [docs/PROCESS_WORKFLOW.md](docs/PROCESS_WORKFLOW.md).

## Hardware

| Component | Purpose |
|-----------|---------|
| ESP32 DevKit, 38-pin (WROOM-32) | Main controller |
| TB6600 stepper driver | Drives the feed stepper (PUL / DIR / ENA) |
| Linear actuator (press) | Compresses plastic inside the bottle |
| 2CH 5V relay board with optocoupler | Drives press actuator (extend/retract) |
| Linear actuator (seal carrier, ~50 mm stroke) | Positions the soldering iron tip against the bottle neck |
| 2CH 5V relay board with optocoupler | Drives seal actuator (extend/retract) |
| 220 V soldering iron with custom tip | Heat-seals the PET bottle neck by melt-fusing |
| 1CH 5V relay module for iron (active LOW) | Switches the 220 V iron on/off |
| AC shredder motor | Shreds plastic bottles into flakes |
| 1CH SSR module for shredder (active HIGH) | Switches the AC shredder motor on/off (turns ON with START button) |
| Load cell (1kg) + HX711 ADC | Detects when bottle is filled with sufficient flakes |
| START button | Tactile switch, GPIO 21, internal pull-up, active LOW |
| STOP button | Tactile switch, GPIO 22, internal pull-up, active LOW |

### Pinout

| Function | GPIO | Notes |
|----------|------|-------|
| TB6600 PUL (step) | 25 | 5 us pulse width |
| TB6600 DIR | 26 | HIGH = feed, LOW = return |
| TB6600 ENA | 27 | LOW = energized; released after each move |
| Press actuator — extend | 32 | Relay channel, active LOW |
| Press actuator — retract | 33 | Relay channel, active LOW |
| Seal actuator — extend | 13 | Relay channel, active LOW |
| Seal actuator — retract | 14 | Relay channel, active LOW |
| Soldering iron relay | 4 | 1CH relay module, active LOW |
| Shredder motor SSR | 18 | SSR gate, active HIGH (SPI CS pin repurposed) |
| Load cell DOUT | 34 | HX711 data out (ADC2, safe) |
| Load cell SCK | 35 | HX711 clock |
| START button | 21 | Tactile, internal pull-up, active LOW |
| STOP button | 22 | Tactile, internal pull-up, active LOW |

GPIO 6–11 are reserved for flash and cannot be used as I/O. GPIO 0, 2, 5, and 12 are strapping pins and should be avoided for output devices. GPIO 1 and 3 carry UART0. The firmware avoids all of these. GPIO 4 is safe for the iron relay — it is not a strapping pin. GPIO 18 is also safe (SPI CS pin repurposed as GPIO). GPIO 21 and 22 are safe general-purpose I/O pins. GPIO 34 and 35 are ADC2 pins safe for input-only use with the HX711.

The ESP32 outputs 3.3 V logic. Ensure the relay board triggers reliably at that level, and that the TB6600 optocoupler recognizes 3.3 V. If steps are missing, add a level shifter on the pulse line.

Most SSRs for 220 V loads are active HIGH (input HIGH = load powered). Verify your specific SSR's datasheet. The relay board itself is typically active LOW.

Extend and retract of the same actuator must never be enabled simultaneously — that would short the polarity reversal. `RELAY_CONF` enforces an interlock so a channel is always released before its opposite is energized.

## Firmware

The state machine is defined in `src/EcoBrickMachine/EcoBrickMachine.ino`. All timing values are `#define` constants at the top of that file; adjust them as needed for your specific iron wattage and tip design.

Stepping is non-blocking. `StepperConf_update()` is called on every loop iteration, so serial input and relay timing remain responsive while the motor runs.

## Bottle Fill Detection (Load Cell)

The 1kg load cell with HX711 ADC monitors the weight of the bottle during the FEED phase. Once the weight exceeds `FILL_WEIGHT_THRESHOLD_G` (default: 120g), the cycle proceeds to PRESS_EXTEND.

### Calibration Procedure

1. Ensure the load cell platform is EMPTY (no bottle)
2. Upload firmware and open Serial Monitor at 115200 baud
3. Send `c` to calibrate (tare) the load cell
4. Place a known weight (e.g., 100g calibration weight or known bottle + flakes)
5. Call `loadCell.setScale(100.0f)` in code or adjust `LOAD_CELL_DEFAULT_SCALE` in `LOAD_CELL_CONF.h`
6. Send `w` to verify weight reading
7. Adjust `FILL_WEIGHT_THRESHOLD_G` based on trial (empty bottle ~30-40g, target filled ~150-180g)

### Serial Commands for Load Cell

| Command | Action |
|---------|--------|
| `c` | Calibrate/tare the load cell (call when empty) |
| `w` | Print current weight and fill status |

## Hardware Operation

The machine has two modes of operation:

### Button Control
- **START button** (GPIO 21): Press to start the machine. This turns ON the iron relay and shredder SSR, then begins the auto cycle.
- **STOP button** (GPIO 22): Press to pause the cycle. Iron and shredder remain ON. Press START again to resume.

### Serial Commands (115200 baud)
Manual controls are available when the machine is in IDLE or PAUSED state:

| Key | Action |
|-----|--------|
| `f` / `b` | Stepper forward / backward |
| `z` | Stop the stepper |
| `p` / `P` | Press actuator extend / retract |
| `l` / `L` | Seal actuator extend / retract |
| `c` | Calibrate load cell (tare) |
| `w` | Print current weight |
| `0` | Stop all actuators |
| `?` | Print current status to serial |

The iron relay and shredder SSR cannot be controlled via serial — they are managed automatically by the button logic.

## Getting Started

### Wiring

Double-check the pinout before applying power. The relay board, TB6600, and SSR modules need their own supplies. Share grounds between the ESP32, the relay board, the stepper driver, and the SSR modules. Leave the actuators and motors disconnected for the first flash and verify that the relay LEDs respond to serial commands before connecting any motors.

Connect the HX711 load cell module:
```
HX711     ESP32
VCC      →  5V (or 3.3V)
GND      →  GND
DT       →  GPIO 34
SCK      →  GPIO 35
```

### Arduino IDE

1. Install the ESP32 core via Tools → Board → Boards Manager ("esp32" by Espressif Systems).
2. Open `src/EcoBrickMachine/EcoBrickMachine.ino`.
3. Select **ESP32 Dev Module** under Tools → Board, then choose your port.
4. Upload and open the Serial Monitor at 115200 baud.

### arduino-cli

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 src/EcoBrickMachine
arduino-cli upload  --fqbn esp32:esp32:esp32 -p <port> src/EcoBrickMachine
```

Replace `<port>` with your ESP32's serial port (e.g., `COM3` on Windows or `/dev/ttyUSB0` on Linux).

## Source Layout

```
eco-brick-machine/
├── README.md
├── LICENSE
├── src/
│   └── EcoBrickMachine/           # Arduino sketch folder
│       ├── EcoBrickMachine.ino    # Main state machine and serial handler
│       ├── STEPPER_CONF.h/.cpp    # TB6600 driver: pins, speed, stepping
│       ├── RELAY_CONF.h/.cpp      # Relay board + SSR control
│       └── LOAD_CELL_CONF.h/.cpp  # HX711 load cell interface
└── docs/
    └── PROCESS_WORKFLOW.md        # Full process description
```

## License

Released under the MIT License. See [LICENSE](LICENSE) for details.
