#!/bin/sh

set -eux
exec 2>&1

/app/blurry "$@"
