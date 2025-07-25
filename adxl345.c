#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>

static int adxl345_open(struct inode *inode, struct file *file);
ssize_t adxl345_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static int adxl345_probe(struct spi_device *spi);
static int adxl345_remove(struct spi_device *spi);

// File operations
static struct file_operations adxl345_fops = {
	.owner = THIS_MODULE,
	.open = adxl345_open,
	.read = adxl345_read,
};

struct spi_data {
	struct cdev cdev;
	char data[1024];
	dev_t dev_num;
	struct class *class;
	struct device *device;
	struct spi_device *spi;
};

static int adxl345_open(struct inode *inode, struct file *file)
{
	struct spi_data *adxl_device;
	adxl_device = container_of(inode->i_cdev, struct spi_data, cdev);
	file->private_data = adxl_device;
	return 0;
}
ssize_t adxl345_read(struct file *file, char __user *buf, size_t count,loff_t *offset)
{

	struct spi_data *tldata = file->private_data;
	struct spi_device *spi = tldata->spi;
	u8 tx_buf[1]={ 0x80 | 0x40 | 0x32};
	u8 rx_buf[6] = { 0 };

	struct spi_message msg;
	struct spi_transfer xfers[2] = { 0 };
	int ret;

	if (count < sizeof(rx_buf))
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
	ret = alloc_chrdev_region(&adxl_device-> dev_num, 0, 1, "adxl345");
	if (ret<0){
		dev_err(&spi->dev,"alloc_chrdev failed\n");
		}
	cdev_init(&adxl_device->cdev, &adxl345_fops);
	adxl_device->cdev.owner = THIS_MODULE;
	ret = cdev_add(&adxl_device->cdev, adxl_device->dev_num ,1);
	if (ret < 0){
		dev_err(&spi->dev,"cdev_add_failed\n");
		unregister_chrdev_region(adxl_device->dev_num, 1);
		kfree(adxl_device);
		return ret;
	}
	//create class node
	adxl_device->class = class_create(THIS_MODULE, "adxl345");
	if (IS_ERR(adxl_device->class)){
		ret = PTR_ERR(adxl_device->class);
		unregister_chrdev_region(adxl_device->dev_num, 1);
		return ret;
	}
	//create device node
	adxl_device->device = device_create(adxl_device->class, NULL, adxl_device->dev_num, NULL, "adxl345");
	return 0;
}
static int adxl345_remove(struct spi_device *spi){
	struct spi_data *adxl_device = spi_get_drvdata(spi);
	device_destroy(adxl_device->class, adxl_device->dev_num);
	class_destroy(adxl_device->class);
	cdev_del(&adxl_device->cdev);
	unregister_chrdev_region(adxl_device->dev_num, 1);
	kfree(adxl_device);
	return 0;
}
static const struct of_device_id adxl345_of_match[] = {
	{ .compatible = "adi,adxl345"},
	{ }
};
MODULE_DEVICE_TABLE(of, adxl345_of_match);
static struct spi_driver adxl345_spi_driver = {
	.driver={
		.name= "adxl345",
		.of_match_table = adxl345_of_match,
	},
	.probe = adxl345_probe,
	.remove = adxl345_remove,
};



MODULE_DESCRIPTION ("Develop spi driver for adxl345 sensor");
MODULE_AUTHOR ("Hamza Amri");
MODULE_LICENSE ("GPL");
module_spi_driver(adxl345_spi_driver);
