```
esptool.py --chip esp32s3 -p COM7 erase_flash
esptool.py --chip esp32s3 -p COM7 -b 460800 write_flash 0x0 merged-binary.bin
```

