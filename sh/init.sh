#!/bin/fresh

idling &
telnetd &
ntpc pool.ntp.org &
mount /dev/sd0 /mnt fatfs
fresh -t /dev/ttyS0
