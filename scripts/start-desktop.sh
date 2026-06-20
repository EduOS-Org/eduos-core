#!/bin/bash

# Wait for Wayland socket to be ready
sleep 0.5

# Launch the panel
/home/davidokocha/eduos-core/shell/panel/panel &

# Keep compositor alive
wait