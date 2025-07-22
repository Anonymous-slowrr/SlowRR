// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define SLOWRR_MAX_DEVICES 8
#define SLOWRR_THRESHOLD_NS 5000000 // 5ms

static struct block_device *slowrr_devices[SLOWRR_MAX_DEVICES];
static int slowrr_dev_count = 0;

// [SLOWRR DIFF]
static bool slowrr_predict_slow(struct bio *bio)
{
    static u64 last_latency = 0;
    u64 now = ktime_get_ns();

    // 模拟：每 20 次 IO 检测一次
    static int counter = 0;
    if (++counter % 20 == 0) {
        // 模拟预测：随机慢
        last_latency = (now % 10 == 0) ? SLOWRR_THRESHOLD_NS + 1 : 0;
    }

    return last_latency > SLOWRR_THRESHOLD_NS;
}

// [SLOWRR DIFF] 判断是否需要 reroute
bool slowrr_should_reroute(struct bio *bio)
{
    return slowrr_predict_slow(bio);
}

// [SLOWRR DIFF] 执行 reroute：暂时简单打印
void slowrr_reroute_bio(struct bio *bio)
{
    struct block_device *new_bdev = slowrr_devices[0]; // 默认 reroute 到第一个设备
    printk(KERN_INFO "[SLOWRR] Rerouting bio %p to %s\n", bio, new_bdev->bd_disk->disk_name);
    // TODO: 实际 reroute：构造新 bio 或 remap
}

// [SLOWRR DIFF] 注册设备
int slowrr_register_device(struct block_device *bdev)
{
    if (slowrr_dev_count >= SLOWRR_MAX_DEVICES)
        return -ENOSPC;

    slowrr_devices[slowrr_dev_count++] = bdev;
    printk(KERN_INFO "[SLOWRR] Registered device %s\n", bdev->bd_disk->disk_name);
    return 0;
}
EXPORT_SYMBOL_GPL(slowrr_register_device);

static int __init slowrr_init(void)
{
    printk(KERN_INFO "[SLOWRR] Module loaded\n");
    return 0;
}

static void __exit slowrr_exit(void)
{
    printk(KERN_INFO "[SLOWRR] Module unloaded\n");
}

module_init(slowrr_init);
module_exit(slowrr_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SLOWRR");
MODULE_DESCRIPTION("Slow Re-router: Predictive I/O Rerouting");