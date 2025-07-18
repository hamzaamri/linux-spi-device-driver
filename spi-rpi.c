#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

struct spi_data {
	struct cdev cdev;
	char data[1024];
};
static struct file_operations adxl345_fops;
static int adxl345_open(struct inode *inode, struct file *file)
{
	struct spi_data *adxl_device;
	adxl_device = container_of(inode->i_cdev, struct spi_data, cdev)
	file->private_data = adxl_device;
	return 0;
}
ssize_t (*read) (struct file *, char __user *buffer, size_t count,loff_t *offset)
{
	
}


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
