# GPS Tracking System with TM4C123GH6PM TivaC Launchpad and SIM808

![Project Image](link_to_your_image.png)

## Overview

This repository contains code for a GPS tracking system using TM4C123GH6PM TivaC Launchpad and SIM808. It leverages GNSS for location tracking and sends coordinates to an AWS server via HTTP GET requests.

## Structure

### 1. [Server Code](/server)
   - Manages HTTP requests on the AWS server.
   - Follow setup instructions in `/server` for configuration.

### 2. [Tiva C Code](/tiva_c)
   - Captures GNSS data and communicates with SIM808.
   - See `/tiva_c` for hardware setup and configuration.

## Getting Started

1. **Server Setup:** Navigate to `/server` for AWS setup.
2. **Tiva C Setup:** Visit `/tiva_c` for TivaC Launchpad setup.
3. **Run the System:** Power up the TivaC Launchpad for GPS tracking.

## Contributions

Contributions welcome! Follow [contribution guidelines](CONTRIBUTING.md).

## License

This project is licensed under [MIT License](LICENSE).

## Acknowledgments

- Mention any libraries, frameworks, or individuals you'd like to acknowledge.
