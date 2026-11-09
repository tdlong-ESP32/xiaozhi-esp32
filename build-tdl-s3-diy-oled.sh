#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
python scripts/build.py tdl-s3-diy --name tdl-s3-diy-oled "$@"
