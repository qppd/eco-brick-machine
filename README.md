# echo-brick-machine

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
```

### `src/`

Contains the project's source code. Keep modules organized by feature or responsibility, and avoid placing documentation or build artifacts here.

### `docs/`

Contains project documentation: design notes, guides, and any additional reference material beyond this README.

## Getting Started

1. Put entry-point code in `src/`.
2. Document decisions and guides in `docs/`.
3. Keep this README updated as the structure evolves.
