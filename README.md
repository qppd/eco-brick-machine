# echo-brick-machine

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

1. Put entry-point code in `src/`.
2. Document decisions and guides in `docs/`.
3. Keep this README updated as the structure evolves.
