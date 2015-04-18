#define __uqmi_wda_commands \
	__uqmi_command(wda_set_data_format, wda-set-data-format, required, QMI_SERVICE_WDA), \
	__uqmi_command(wda_get_data_format, wda-get-data-format, no, QMI_SERVICE_WDA)


#define wda_helptext \
		"  --wda-set-data-format <type>:     Set data format (type: 802.3|raw-ip)\n" \
		"  --wda-get-data-format:            Get data format\n" \

