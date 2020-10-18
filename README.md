# NAME

bbf - a safer and more featureful tool for dealing with bad blocks on harddrives

# SYNOPSIS

bbf [options] &lt;instruction&gt; &lt;path&gt;

# DESCRIPTION

**bbf** is a tool built around the workflow in dealing with harddrive bad blocks. It has a number of features to limit risk in using the tool and provides features to more easily track down what files are affected by the bad blocks found. Also gives you the ability to manually mark blocks as corrupted in cases where the block isn't technically bad but causing issues.


# FEATURES

 * readonly scanning of bad blocks
 * safe 'fix' mode which won't overwrite good blocks
 * burnin mode for checking new drives
 * manual marking blocks as corrupted
 * find files given list of blocks
 * dump list of files and associated block ranges
 * dump list of blocks used by a file
 * issue secure drive erasure
 * filesystem stressing


# OPTIONS

### arguments ###

* **-f, --force** : override checking if drive is in use when trying to perform destructive actions
* **-t, --rwtype <os|ata>** : select between OS or ATA reads and writes (default: os)
* **-q, --quiet** : redirects stdout to /dev/null
* **-s, --start-block <lba>** : block to start from (default: 0)
* **-e, --end-block <lba>** : block to stop at (default: last block)
* **-S, --stepping <n>** : number of logical blocks to read at a time (default: physical / logical)
* **-o, --output <file>** : file to write bad block list to
* **-i, --input <file>** : file to read bad block list from
* **-r, --retries <count>** : number of retries on certain reads & writes
* **-c, --captcha <captcha>** : needed when performing destructive operations
* **-M, --maxerrors <n>** : max r/w errors before exiting (default: 1024)


### instructions ###

* **info** : print out details of the device
* **captcha** : print captcha for device
* **scan** : perform scan for bad blocks by reading
* **fix** : attempt to force drive to reallocate block
* **fix-file** : same behavior as 'fix' but only for a file's blocks
* **burnin** : attempts a non-destructive write, read, & verify
* **fsthrash** : hammers the filesystem with calls to core filesystem functions
* **filethrash** : creates 1 file to fill drive and hammers it with writes
* **find-files** : given a list of bad blocks try to find affected files
* **dump-files** : dump list of block ranges and files assocated with them
* **file-blocks** : dump a list of individual blocks a file uses
* **write-pseudo-uncorrectable-wl** : mark blocks as corrupted / uncorrectable
* **write-pseudo-uncorrectable-wol** : mark blocks as corrupted / uncorrectable
* **write-flagged-uncorrectable-wl** : mark blocks as corrupted / uncorrectable
* **write-flagged-uncorrectable-wol** : mark blocks as corrupted / uncorrectable
* **security-erase** : secure erase drive by overwriting data with zeros
* **enhanced-security-erase** : secure erase drive by overwriting data with vendor specific patterns


# NOTES

OS mode is the default but ATA is the suggested mode. Especially for `fix` and `burnin`. Use a higher stepping value to improve the performance. The max value depends on the drive but for a 512 logical block size a value of 128 or 256 seems to work well.

When running a `fix` or `burnin`, rather than writing zeros like other tools, it will first read the block and try to write it back. This will be non-destructive so long as the same location is not being used at the same time. Only if the block read fails will zeros be used.

A captcha is required for destructive operations. This helps with preventing the accidental running of the tool on the wrong drive.


# EXAMPLES

```
# bbf info /dev/sdb
/dev/sdi:
 - serial_number: XXXXXXXX
 - firmware_revision: SC61
 - model_number: ST8000VN0022-2EL112
 - RPM: 7200
 - features:
   - form_factor: 3.5"
   - write_uncorrectable: 1
   - smart_supported: 1
   - smart_enabled: 1
   - security_supported: 1
   - security_enabled: 0
   - security_locked: 0
   - security_frozen: 0
   - security_count_expired: 0
   - security_enhanced_erase_supported: 1
   - security_normal_erase_time: 698
   - security_enhanced_erase_time: 698
   - block_erase: 0
   - overwrite: 1
   - crypto_scramble: 0
   - sanitize: 1
   - supports_sata_gen1: 1
   - supports_sata_gen2: 1
   - supports_sata_gen3: 1
   - trim_supported: 0
 - block_size:
   - physical: 4096
   - logical: 512
   - stepping: 8
 - block_count:
   - physical: 1953506646
   - logical: 15628053168
 - size:
   - bytes: 8001563222016
   - human:
     - base2: 7.28TB
     - base10: 8.00TiB

# bbf -S 256 -t ata scan /dev/sdb
start block: 0
end block: 15628053168
stepping: 256
logical block size: 512
physical block size: 4096
read size: 131072
Scanning: 0 - 15628053168
Current: 2425512192 (15.52%); bps: 179384.74; eta: 20:26:39; bad: 0

# bbf captcha /dev/sdb
Z8400VR0

# bbf -i ~/badblocks.Z8400VR0 -c Z8400VR0 fix /dev/sdb
```


# BUILD / INSTALL

```
$ git clone https://github.com/trapexit/bbf
$ cd bbf
$ make
...
$ sudo cp -av bbf /usr/local/bin
```


# SUPPORT

#### Contact / Issue submission

* github.com: https://github.com/trapexit/bbf/issues
* email: trapexit@spawn.link
* twitter: https://twitter.com/_trapexit
* reddit: https://www.reddit.com/user/trapexit
* discord: https://discord.gg/MpAr69V


#### Support development

This software is free to use and released under a very liberal license. That said if you like this software and would like to support its development donations are welcome.

* Patreon: https://www.patreon.com/trapexit
* PayPal: trapexit@spawn.link
* Bitcoin (BTC): 12CdMhEPQVmjz3SSynkAEuD5q9JmhTDCZA
* Bitcoin Cash (BCH): 1AjPqZZhu7GVEs6JFPjHmtsvmDL4euzMzp
* Ethereum (ETH): 0x09A166B11fCC127324C7fc5f1B572255b3046E94
* Litecoin (LTC): LXAsq6yc6zYU3EbcqyWtHBrH1Ypx4GjUjm
