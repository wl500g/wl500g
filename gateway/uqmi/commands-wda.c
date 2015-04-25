#include <stdlib.h>

#include "qmi-message.h"

static const struct {
	const char *name;
	QmiWdaLinkLayerProtocol val;
} link_modes[] = {
	{ "802.3", QMI_WDA_LINK_LAYER_PROTOCOL_802_3 },
	{ "raw-ip", QMI_WDA_LINK_LAYER_PROTOCOL_RAW_IP },
};

#define cmd_wda_set_data_format_cb no_cb

static enum qmi_cmd_result
cmd_wda_set_data_format_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	struct qmi_wda_set_data_format_request data_req = {};
	int i;

	for (i = 0; i < ARRAY_SIZE(link_modes); i++) {
		if (strcasecmp(link_modes[i].name, arg) != 0)
			continue;

		qmi_set(&data_req, link_layer_protocol, link_modes[i].val);
		qmi_set_wda_set_data_format_request(msg, &data_req);
		return QMI_CMD_REQUEST;
	}

	uqmi_add_error("Invalid auth mode (valid: 802.3, raw-ip)");
	return QMI_CMD_EXIT;
}

static void
cmd_wda_get_data_format_cb(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg)
{
	struct qmi_wda_get_data_format_response res;
	const char *name = "unknown";
	int i;

	qmi_parse_wda_get_data_format_response(msg, &res);
	for (i = 0; i < ARRAY_SIZE(link_modes); i++) {
		if (link_modes[i].val != res.data.link_layer_protocol)
			continue;

		name = link_modes[i].name;
		break;
	}

	blobmsg_add_string(&status, NULL, name);
}

static enum qmi_cmd_result
cmd_wda_get_data_format_prepare(struct qmi_dev *qmi, struct qmi_request *req, struct qmi_msg *msg, char *arg)
{
	qmi_set_wda_get_data_format_request(msg);
	return QMI_CMD_REQUEST;
}
