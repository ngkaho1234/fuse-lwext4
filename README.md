fuse-lwext4
======
This is a implementation of EXT4 on FUSE.

##Installation:
```bash
make
make install
```

##NOTES:
1. Currently fuse-lwext4 runs in single-threaded mode, because running  
the fuse module in multi-threaded mode may be unstable.

##CREDIT:
Here i pay tribute sincerely to those people:

Gerard Lledó Vives, gerard.lledo@gmail.com (Originally based on the fuse layer by ext4fuse)  
  
Grzegorz Kostka, kostka.grzegorz@gmail.com (The author of lwext4)

##WARNINGS:
 
    The software may crash your system and ruin your data unexpectedly,
    since there might be software conflicts and I could only test it
    on some of the popular platforms. You should use it with care and
    use it at your own risk!

##Usage:
```bash
fuse-lwext4 <disk> <mountpoint> [-f] [-o <options>]
```
Reminder:  
If you want to specify more than one option, you should separate them with commas.

#Options:
journal - Journalling support will be enabled if it is supported by the filesystem  
debug   - Print out debug messages
