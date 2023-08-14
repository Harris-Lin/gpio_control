#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include "gpio_control.h"

struct gpio_led_device {
    struct cdev cdev;
    int led_pin_red;
};

static dev_t dev_num;
static struct class *LED_class;
struct gpio_led_device led_device;

static int led_gpio_open(struct inode *inode, struct file *filp) 
{
    gpio_direction_output(led_device.led_pin_red, 1);
    return 0;
}

static int led_gpio_release(struct inode *inode, struct file *filp) 
{
    gpio_set_value(led_device.led_pin_red, 0);
    return 0;
}

static ssize_t led_gpio_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) 
{
    char val;
    int ret;

    ret = copy_from_user(&val, buf, sizeof(char));
    if (ret) {
        printk(KERN_ERR "Failed to copy data from user\n");
        return -EFAULT;
    }

    gpio_set_value(led_device.led_pin_red, val - '0');
    return sizeof(val);
}

static struct file_operations led_gpio_fops = {
    .owner = THIS_MODULE,
    .open = led_gpio_open,
    .release = led_gpio_release,
    .write = led_gpio_write,
};

static int __init gpio_init(void)
{
    int ret = 0;

    ret = gpio_request(LED_RED_PIN, "Red LED");
    if (ret < 0) {
        printk(KERN_ERR "Failed to register GPIO driver\n");
        return ret;
    }

    ret = alloc_chrdev_region(&dev_num, 0, 1, "led_device");
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate device number\n");
        return ret;
    }

    led_device.led_pin_red = LED_RED_PIN;
    cdev_init(&led_device.cdev, &led_gpio_fops);
    ret = cdev_add(&led_device.cdev, dev_num, 1);

    if (ret) {
        printk(KERN_ERR "Failed to add cdev\n");
        return ret;
    }

    LED_class = class_create(THIS_MODULE, "LED_class");
    device_create(LED_class, NULL, dev_num, NULL, "LED_device");

    printk(KERN_INFO "Module loaded, major num is %d\n", dev_num >> 20);

    return 0;
}

static void __exit gpio_exit(void)
{
    gpio_set_value(LED_RED_PIN, 0);
    device_destroy(LED_class, dev_num);
    class_destroy(LED_class);
    cdev_del(&led_device.cdev);
    unregister_chrdev_region(dev_num, 1);
    gpio_free(LED_RED_PIN);
    printk(KERN_INFO "GPIO driver exit\n");
}

module_init(gpio_init);
module_exit(gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HarrisLin");
MODULE_DESCRIPTION("LED GPIO driver");