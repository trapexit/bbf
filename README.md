# NAME

bbf - a tool for dealing with bad blocks on harddrives

# SYNOPSIS

bbf [options] &lt;instruction&gt; &lt;path&gt;

# DESCRIPTION

**bbf** is a tool built around the workflow in dealing with harddrive bad blocks.

# FEATURES

 * Readonly scanning of bad blocks
 * Forced reallocation of specific blocks
 * Burnin mode for checking new drives
 * Mark blocks as corrupted
 * Find files given list of blocks
 * Dump list of files and associated block ranges
 * Dump list of blocks used by a file
 
# OPTIONS

### arguments ###

* **-f, --force** : override checking if drive is in use when trying to perform destructive actions
* **-t, --rwtype <os|ata>** : select between OS or ATA reads and writes (default: os)
* **-q, --quiet** : redirects stdout to /dev/null
* **-s, --start-block <lba>** : block to start from (default: 0)
* **-e, --end-block <lba>** : block to stop at (default: last block)
* **-o, --output <file>** : file to write bad block list to
* **-i, --input <file>** : file to read bad block list from
* **-r, --retries <count>** : number of retries on certain reads & writes
* **-c, --captcha <captcha>** : needed when performing destructive operations

### instructions ###

* **info** : print out details of the device
* **captcha** : print captcha for device
* **scan** : perform scan for bad blocks by reading
* **fix** : attempt to force drive to reallocate block
* **fix-file** : same behavior as 'fix' but only for a file's blocks
* **burnin** : attempts a non-destructive write, read, & verify
* **find-files** : given a list of bad blocks try to find affected files
* **dump-files** : dump list of block ranges and files assocated with them
* **file-blocks** : dump a list of individual blocks a file uses
* **write-uncorrectable** : mark blocks as corrupted / uncorrectable

# NOTES

* OS mode is higher level and faster than ATA but may not work as well as ATA in certain situations.
* ATA mode is low level and slower relative to OS mode. 
* For scanning using OS mode is generally fine.
* When using "burnin" or "fix" ATA is more likely to work.
* When in doubt or if you wish to be thorough run the behavior twice: once with OS and then with ATA.

# SUPPORT

* github.com: https://github.com/trapexit/bbf/issues
* email: trapexit@spawn.link

