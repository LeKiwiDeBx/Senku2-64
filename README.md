# Senku64

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Build Status](https://img.shields.io/travis/username/Senku64.svg)](https://travis-ci.org/username/Senku64)
[![Coverage Status](https://coveralls.io/repos/github/username/Senku64/badge.svg?branch=main)](https://coveralls.io/github/username/Senku64?branch=main)

## Table of Contents

- [Senku64](#senku64)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Features](#features)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Configuration](#configuration)
  - [Contributing](#contributing)
  - [License](#license)
  - [Acknowledgements](#acknowledgements)

## Overview

Senku64 is a digital version of the classic peg solitaire game, also known as "solitary." It is built using the GTK3 API for the graphical interface and written in the C programming language. The game offers an engaging and challenging experience for fans of the traditional game.

## Features

- Classic peg solitaire gameplay.
- Intuitive graphical interface built with GTK3.
- Cross-platform support (Windows, Linux).

## Installation

### Prerequisites

Before you begin, ensure you have met the following requirements:

- **GTK3 Runtime**: For Windows, you need to download and install the GTK3 runtime from [GTK3 Downloads](https://www.gtk.org/download/windows.php) or set up an MSYS2 development environment.
- **C Compiler**: Ensure you have a C compiler installed (e.g., GCC).

### Steps

1. Clone the repository:
   ```sh
   git clone https://github.com/username/Senku64.git
   ```
2. Navigate to the project directory:
   ```sh
   cd Senku64
   ```
3. Install dependencies:

   For Windows (using MSYS2):
   ```sh
   pacman -S mingw-w64-x86_64-gtk3
   ```

   For Linux (Debian-based):
   ```sh
   sudo apt-get install libgtk-3-dev
   ```

4. Build the project:
   ```sh
   gcc -o senku64 src/main.c `pkg-config --cflags --libs gtk+-3.0`
   ```

## Usage

To start the game, run:

```sh
./senku64
```

For detailed usage instructions, refer to the [documentation](docs/usage.md).

## Configuration

This project doesn't require extensive configuration. Ensure that the GTK3 runtime or development environment is properly set up as described in the installation steps.

## Contributing

We welcome contributions! Please see the [contributing guidelines](CONTRIBUTING.md) for more details.

To contribute:

1. Fork the repository.
2. Create a new branch:
   ```sh
   git checkout -b feature/YourFeatureName
   ```
3. Make your changes and commit them:
   ```sh
   git commit -m 'Add some feature'
   ```
4. Push to the branch:
   ```sh
   git push origin feature/YourFeatureName
   ```
5. Open a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Acknowledgements

- [GTK3](https://www.gtk.org/) for providing the graphical toolkit.
- [Person 1](https://github.com/person1) for initial development and contributions.
- [Library/Tool](https://linktolibrary.com) for usage in the project.

Feel free to open an issue or submit a pull request for any improvements or bug fixes. Thank you for your interest and contribution!
