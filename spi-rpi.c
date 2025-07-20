#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spi/spi.h>

struct spi_data {
	struct cdev cdev;
	char data[1024];
	dev_t dev_num;
	struct class *class;
	struct device *device;
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

static int adxl345_probe(struct spi_device *spi)
{
	int ret;
	struct spi_data *adxl_device;
	adxl_device = kzalloc(sizeof(struct spi_data), GFP_KERNEL);
	if(!adxl_device)
		return -ENOMEM;
	spi_set_drvdata(spi, adxl_device);
	ret = alloc_chrdev_region(&adxl_device-> dev_t, 0, 1, "adxl345");
	if (ret<0){
		dev_err(,"alloc_chrdev failed\n");
		}
	cdev_init(&adxl_device->cdev, adxl345_fops);
	adxl_device->cdev.owner = THIS_MODULE;
	ret = cdev_add(&adxl_device->cdev, &adxl_device->dev_t ,1);
	if (ret < 0){
		dev_err(&spi->adxl_device,"cdev_add_failed\n");
		unregister_chrdev_region(&adxl_device->devt, 1);
		kfree(adxl_device);
		return ret;

	//create class node
	adxl_device->class = class_create(THIS_MODULE, "adxl345");
	if (IS_ERR(adxl_device->class)){
		ret = PTR_ERR(adxl_device)->class;
		unregister_chrdev_region(&adxl_device->devt, 1);
		return ret;
	}
	//create device node
	adxl_device->device = device_create(adxl_device->class, NULL, adxl_device->dev_num, NULL);
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
