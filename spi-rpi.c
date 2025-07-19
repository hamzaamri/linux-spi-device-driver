#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spi/spi.h>

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
ssize_t (*read) (struct file *, char __user *buf, size_t count,loff_t *offset)
{

	struct spi_data *tldata = file->private_data;
	struct spi_device *spi = tldata->spi;
	u8 tx_buf[1]={ 0x80 | 0x40 | 0x32};
	u8 rx_buf[6] = { 0 };

	struct spi_message msg;
	struct spi_transfer xfers[2] = { 0 };
	int ret;

	if (len < sizeof(rx_buf))
		return -EINVAL;
	spi_message_init(&msg);

	xfers[0].tx_buf = tx_buf;
	xfers[0].len = 1;

	xfers[1].rx_buf = rx_buf;
	xfers[1].len = 6;

	spi_message_add_tail(&xfers[0], &msg);
	spi_message_add_tail(&xfers[1], &msg);

	ret = spi_sync(spi, &msg);
	if (ret < 0)
		return ret;
	if(copy_to_user(buf, rx_buf, sizeof(rx_buf)))
		return -EFAULT;
	return sizeof(rx_buf);
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
	.open = adxl345_open,
	.read = adxl345_read,
}


MODULE_DEVICE_TABLE(of, adxl345_of_match);

MODULE_DESCRIPTION =("Develop spi driver for adxl345 sensor");
MODULE_AUTHOR =("Hamza Amri");
MODULE_LICENCE =("GPL");
