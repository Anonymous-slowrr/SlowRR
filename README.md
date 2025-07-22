# Where there’s a slow, there’s another way: Mitigating Tail-latency via Inter-device Coordination

The repository includes the following artifacts:

```SlowRR Framework``` : The source code of SlowRR.

```Filebench and Workload``` : The source code of modified Filebench(based on [filebench/filebench](https://github.com/filebench/filebench)) and three Workloads.


## How to execute performance test:


### Step 1: Install Filebench:

```
cd ./filebench-new
./configure
sudo make
sudo make install
echo 0 > /proc/sys/kernel/randomize_va_space
filebench -f ./workloads/fileserver.f
```

### Step 2: Set Kernel Config (Important):

First you should enter the main directory of system source code, and then

```
sudo make menuconfig
```

Set the Device Drivers -> Block devices -> RAM block device support to "M".
And then save changes to .config.

Next, replace (source code)/drivers/block/brd.c by brd.c in this repository.
And add slowrr.c to (source code)/drivers/block/slowrr.c.
Replace (source code)/drivers/block/Makefile by Makefile in this repository.

After that, re-compile the system kernel.

```
sudo make -j8
make modules_install
make install
reboot
```

### At this point, SlowRR deployment is complete, and you can use SlowRR on your system to improve SSD array performance!


### Step Test: Script Custom Settings:

Some scripts are provided to help with simple testing. (LinnOS is necessary for test! Please install according to https://www.usenix.org/conference/osdi20/presentation/hao)
Settings in the script shoule be set ahead of run(Flexible adjustment is allowed):

```
#output directory
dist_dir="/home/result_slowrr/"
#test dev
nvme_dev="nvme0n1"
nvme_name="nvme0"
#workload directory
workload_dir="/home/filebench-new/workloads"
#workload names
workload_name_Fileserver="new_fileserver.f"
```

Before testing, make sure all module is unloaded ahead of run because they will be loaded with different parameters during running.

And then use the scripts.(About 12 hours)
```
./Scripts/slowrr_filebench_ext4.sh
```




