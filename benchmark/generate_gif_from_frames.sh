#!/usr/bin/env bash
ffmpeg -y -framerate 5 -pattern_type glob -i 'gif/*.png' -c:v libx264 -pix_fmt yuv420p out.mp4
