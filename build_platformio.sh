#!/bin/bash

set -euo pipefail

pio ci --lib=. --board esp32dev "examples/AutoConnect"
pio ci --lib=. --board esp12e "examples/AutoConnect"
