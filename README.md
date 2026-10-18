# EcoBrick Machine

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/platform-ESP32%20WROOM--32-000000.svg)](https://www.espressif.com/)
[![Language: C++](https://img.shields.io/badge/language-C%2B%2B-f34b7d.svg)](https://isocpp.org/)

Firmware for the EcoBrick Machine, an automated line that turns loose plastic
bottles into compacted, sealed bricks. The controller runs on a 38-pin ESP32
DevKit and drives one TB6600 stepper driver plus two linear actuators — one
for compression, one for induction sealing — through a 4-channel relay board.

## Process Overview

```
Shredding → Bottle Filling → Robotic Arm Transfer → Compression
         → Induction Sealing → Conveyor Transport → Collection Box
```

1. **Shredding and filling** — the shredder breaks a plastic bottle into
   flakes, which fall into a second bottle acting as the container.
2. **Robotic arm transfer** — a pick-and-place arm moves the filled bottle
   from the shredding station to the pressing station.
3. **Compression** — the press actuator squeezes the flakes inside the
   bottle, cutting their volume down to a dense pack.
4. **Conveyor and induction sealing** — the bottle is carried to the sealing
   head, which heats the opening and closes it.
5. **Collection** — the sealed bottle rides the conveyor to the end and drops
   into a collection box.

The full write-up lives in [docs/PROCESS_WORKFLOW.md](docs/PROCESS_WORKFLOW.md).

## Control Electronics

| Part | Role |
|------|------|
| ESP32 DevKit, 38-pin (WROOM-32) | Main controller, runs the firmware |
| TB6600 stepper driver | Drives the feed stepper (PUL / DIR / ENA) |
| 2× linear actuators | Press (compression) and sealing head |
| 4-channel relay board | 2 channels per actuator: extend / retract |

### Pinout

| Function | GPIO | Notes |
|----------|------|-------|
| TB6600 PUL (step) | 25 | 5 µs pulse, 1600 steps/s at the shipped speed |
| TB6600 DIR | 26 | HIGH = forward (feed), LOW = return |
| TB6600 ENA | 27 | LOW = energized; released once a move finishes |
| Press actuator — extend | 32 | Relay channel, active LOW |
| Press actuator — retract | 33 | Relay channel, active LOW |
| Sealer actuator — extend | 13 | Relay channel, active LOW |
| Sealer actuator — retract | 14 | Relay channel, active LOW |

GPIO 6–11 sit behind the flash, GPIO 0/2/5/12 are strapping pins and
GPIO 1/3 carry UART0, so the firmware stays away from all of them. The ESP32
outputs 3.3 V: make sure the relay board triggers at that level, and that the
TB6600 optocoupler input registers 3.3 V — if steps go missing, level-shift
the pulse line.

Extend and retract of the same actuator must never be on together (that would
short the polarity reversal). `RELAY_CONF` enforces this interlock, so a
channel is always released before its opposite is energized.

## Operation Cycle

The sketch walks through nine states and repeats:

| State | Action | Duration |
|-------|--------|----------|
| FEED | Stepper feeds the bottle into the press | 1600 steps (~1 s) |
| PRESS_EXTEND | Press actuator extends | 1.5 s |
| PRESS_HOLD | Press stopped, compression held | 0.5 s |
| PRESS_RETRACT | Press actuator returns | 1.5 s |
| SEAL_EXTEND | Sealing head moves onto the bottle | 1.5 s |
| SEAL_HOLD | Induction heating, then stopped | 3 s |
| SEAL_RETRACT | Sealing head returns | 1.5 s |
| RETURN | Stepper backs to the start position | 1600 steps (~1 s) |
| PAUSE | Everything off | 1 s |

A full cycle runs about 12.5 seconds. All of these values are `#define`s at
the top of `src/EcoBrickMachine/EcoBrickMachine.ino`; the stepper speed comes
from `StepperConf_setSpeedRpm(60.0f)` in `setup()` (60 RPM at 1600
steps/revolution = 1600 steps/s). The machine boots with every output off and
starts the first cycle after the opening pause.

Stepping is non-blocking — `StepperConf_update()` is called every `loop()`
iteration, so the serial console and the relay timings stay responsive while
the motor runs.

## Getting Started

### Wiring

Double-check the pinout above before applying power. The relay board and the
TB6600 need their own supply; share grounds between the ESP32, the relay
board and the driver. Leave the actuators disconnected for the first flash
and confirm the relay LEDs behave as expected.

### Arduino IDE

1. Install the ESP32 core from Tools → Board → Boards Manager
   ("esp32" by Espressif Systems).
2. Open `src/EcoBrickMachine/EcoBrickMachine.ino`.
3. Select Tools → Board → ESP32 Arduino → **ESP32 Dev Module**, then pick
   the port.
4. Verify or upload, and open the Serial Monitor at **115200 baud**.

### arduino-cli

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 src/EcoBrickMachine
arduino-cli upload  --fqbn esp32:esp32:esp32 -p COM3 src/EcoBrickMachine
```

Replace `COM3` with your ESP32's port.

## Serial Commands

Available at 115200 baud; manual keys are ignored while the automatic cycle
is running.

| Key | Action |
|-----|--------|
| `a` | Toggle the automatic cycle |
| `f` / `b` | Stepper forward / backward (manual mode) |
| `z` | Stop the stepper |
| `p` / `P` | Press actuator extend / retract (manual mode) |
| `l` / `L` | Seal actuator extend / retract (manual mode) |
| `0` | Stop all actuators |
| `?` | Print status (mode, state, stepper, actuators) |

Pressing `a` while paused drops every relay and restarts the cycle from FEED;
pressing it during a cycle turns all outputs off and stops the stepper.

## Source Layout

```
echo-brick-machine/
├── README.md
├── LICENSE
├── src/
│   └── EcoBrickMachine/       # sketch folder (open this in the IDE)
│       ├── EcoBrickMachine.ino   # cycle state machine + serial handling
│       ├── STEPPER_CONF.h/.cpp   # TB6600: pins, speed, blocking/non-blocking moves
│       └── RELAY_CONF.h/.cpp     # 4 relay channels + actuator interlock
└── docs/
    └── PROCESS_WORKFLOW.md    # process description of the full line
```

## Documentation

- [docs/PROCESS_WORKFLOW.md](docs/PROCESS_WORKFLOW.md) — station-by-station
  description of the machine process

## License

Released under the [MIT License](LICENSE).
