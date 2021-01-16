# Notes for Chapter 2

### Loading and Removing Kernel Modules
#### Loading
```bash
sudo insmod kernel_mod.ko
```
To check if module has been loaded, use `dmesg`.

#### Removing
```bash
sudo rmmod kernel_mod
```

To clear buffer (since it can fill up quickly), use `sudo dmesg -c`.
