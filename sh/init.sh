#!/bin/fresh

idling &

# telnetd &

mount /dev/sd0 /mnt fatfs

fresh -t /dev/ttyS0 &


