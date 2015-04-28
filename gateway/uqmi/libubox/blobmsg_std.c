#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include "blobmsg.h"
#include "blobmsg_std.h"

static const char *indent_str = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

#define indent_printf(indent, ...) do { \
	if (indent > 0) { \
		if (indent > 15) \
			indent = 15; \
		fwrite(indent_str, indent, 1, stdout); \
	} \
	printf(__VA_ARGS__); \
} while(0)

static void blobmsg_format_element(struct blob_attr *data, int next_indent)
{
	switch (blobmsg_type(data)) {
	case BLOBMSG_TYPE_UNSPEC:
		printf("null\n");
		break;
	case BLOBMSG_TYPE_STRING:
		printf("%s\n", blobmsg_get_string(data));
		break;
	case BLOBMSG_TYPE_BOOL:
		printf("%s\n", blobmsg_get_u8(data) ? "true" : "false");
		break;
	case BLOBMSG_TYPE_INT16:
		printf("%d\n", blobmsg_get_u16(data));
		break;
	case BLOBMSG_TYPE_INT32:
		printf("%d\n", blobmsg_get_u32(data));
		break;
	case BLOBMSG_TYPE_INT64:
		printf("%"PRIu64"\n", blobmsg_get_u64(data));
		break;
	case BLOBMSG_TYPE_TABLE:
	case BLOBMSG_TYPE_ARRAY:
		blobmsg_format_std_indent(data, next_indent);
		break;
	}
}

void blobmsg_format_std_indent(const struct blob_attr *data, int indent)
{
	struct blob_attr *attr;
	const struct blob_attr *head = blobmsg_data(data);
	int len = blobmsg_data_len(data);

	__blob_for_each_attr(attr, head, len) {
		int type = blobmsg_type(attr);

		if (blobmsg_type(data) != BLOBMSG_TYPE_ARRAY
			 && blobmsg_name(attr)[0])
			indent_printf(indent, "%s: ", blobmsg_name(attr));
		else
			indent_printf(indent, "%s", "");

		if (type == BLOBMSG_TYPE_TABLE)
			printf("{\n");
		else if (type == BLOBMSG_TYPE_ARRAY)
			printf("[\n");

		blobmsg_format_element(attr, indent + 1);

		if (type == BLOBMSG_TYPE_TABLE)
			indent_printf(indent, "}\n");
		else if (type == BLOBMSG_TYPE_ARRAY)
			indent_printf(indent, "]\n");
	}
}
