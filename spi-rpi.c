#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>

struct adxl_dev {
	struct cdev cdev;
	char buffer[1024];
};

struct adxl_dev *adxl_device;
static struct file_operations adxl345_fops;

static const struct of_device_id adxl345_of_match[] = {
	{ .compatible = "adi,adxl345"},
	{ }
};
static struct spi_driver adxl345_spi_driver = {
	.driver={
		.name=spi_driver,
		.of_match_table = adxl345_of_match,
	},
	.probe = adxl345_probe,
	.remove = adxl345_remove,
};
static struct file_operations adxl345_fops = {
	.owner = THIS_MODULE,
	.open = adxl345_read,
	.read = adxl345_read,
}


MODULE_DEVICE_TABLE(of, adxl345_of_match);

MODULE_DESCRIPTION =("Develop spi driver for adxl345 sensor");
MODULE_AUTHOR =("Hamza Amri");
MODULE_LICENCE =("GPL");
