#!/bin/bash

set -euo pipefail

pio lib -g install "ESP Async WebServer@1.2.3"

pio ci --lib=. --board esp32dev "examples/AutoConnect"
pio ci --lib=. --board esp12e "examples/AutoConnect"
