# echo-brick-machine

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Arduino](https://img.shields.io/badge/Arduino-UNO%20%2F%20Nano%20%2F%20Mega-00979D.svg)](https://www.arduino.cc/)
[![Language: C++](https://img.shields.io/badge/language-C%2B%2B-f34b7d.svg)](https://isocpp.org/)

Arduino controller for the EcoBrickMachine: 1x TB6600 stepper + 2 linear actuators (press and induction sealer), 2 relay channels each.

## Machine Process

```
Shredding → Bottle Filling → Robotic Arm Transfer → Compression
         → Induction Sealing → Conveyor Transport → Collection Box
```

1. **Plastic Bottle Shredding and Filling** – the shredder processes the plastic bottle into smaller pieces, collected inside another plastic bottle.
2. **Robotic Arm Transfer** – a robotic arm pick-and-places the filled bottle from the shredding station to the pressing station.
3. **Plastic Compression** – the press compresses the shredded plastic to reduce its volume.
4. **Conveyor and Induction Sealing** – the conveyor moves the bottle to the induction sealing station, where heat seals the opening.
5. **Final Conveying and Collection** – the sealed bottle travels to the end of the conveyor and drops into a collection box.

Full details: [docs/PROCESS_WORKFLOW.md](docs/PROCESS_WORKFLOW.md)

## Hardware (current code)

| Device                      | Pin(s)   | Notes                                   |
|-----------------------------|----------|-----------------------------------------|
| TB6600 stepper – PUL        | 3        | 5 µs pulse, 1000 steps/s default        |
| TB6600 stepper – DIR        | 4        | HIGH = forward                          |
| TB6600 stepper – ENA        | 5        | freed when a move finishes              |
| Press actuator – extend     | 6        | relay, active LOW                       |
| Press actuator – retract    | 7        | relay, active LOW                       |
| Sealer actuator – extend    | 8        | relay, active LOW                       |
| Sealer actuator – retract   | 9        | relay, active LOW                       |

> Never energize extend + retract of the same actuator together — the code
> enforces this interlock.

## Project Structure

```
echo-brick-machine/
├── README.md      # This file
├── LICENSE        # MIT license
├── src/           # Source code
│   └── EcoBrickMachine/   # Arduino sketch
│       ├── EcoBrickMachine.ino
│       ├── STEPPER_CONF.h / .cpp
│       └── RELAY_CONF.h / .cpp
└── docs/          # Project documentation
    └── PROCESS_WORKFLOW.md
```

### `src/`

Contains the project's source code. Keep modules organized by feature or responsibility, and avoid placing documentation or build artifacts here.

### `docs/`

Contains project documentation: design notes, guides, and any additional reference material beyond this README.

## Getting Started

### Flash with the Arduino IDE

1. Open `src/EcoBrickMachine/EcoBrickMachine.ino` in the Arduino IDE.
2. Select your board (Tools → Board) and port (Tools → Port).
3. Verify the pins in `STEPPER_CONF.h` and `RELAY_CONF.h` match your wiring.
4. Upload, then open the Serial Monitor at **115200 baud**.

### Flash with arduino-cli

```sh
arduino-cli compile --fqbn arduino:avr:uno src/EcoBrickMachine
arduino-cli upload  --fqbn arduino:avr:uno -p COM3 src/EcoBrickMachine
```

## Serial Commands (115200 baud)

| Key   | Action                                    |
|-------|-------------------------------------------|
| `a`   | Toggle automatic cycle on/off             |
| `f` / `b` | Stepper forward / backward (manual mode) |
| `z`   | Stop the stepper                          |
| `p` / `P` | Press actuator extend / retract (manual) |
| `l` / `L` | Seal actuator extend / retract (manual)  |
| `0`   | Stop all actuators                        |
| `?`   | Print status                              |

Manual commands only work while auto mode is **off**.

## Documentation

- [docs/PROCESS_WORKFLOW.md](docs/PROCESS_WORKFLOW.md) – machine process workflow

## License

Licensed under the [MIT License](LICENSE).
