static struct platform_device_id gpiosensor_id[] = {
	{"gpiosensor", 0},
	{},
};

static const struct of_device_id gpiosensor_dt_ids[] = {
	{
		.compatible = "trdx, gpiosensor"
	}, {
		/*sentinel*/
	}
};
MODULE_DEVICE_TABLE(of, gpiosensor_dt_ids);

static struct platform_driver gpiosensor_driver = {
	.probe = gpiosensor_probe,
	.remove = gpiosensor_remove,
	.id_table = gpiosensor_id,
	.driver = {
		.name = "gpiosensor",
		.owner = THIS_MODULE,
	},	
};

