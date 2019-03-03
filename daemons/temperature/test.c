#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "gattlib.h"

// Battery Level UUID
const uuid_t g_battery_level_uuid = CREATE_UUID16(0x2A19);

const char* TEMPERATURE_UUID = "00002235-b38d-4985-720e-0f993a68ee41";
const char* HUMID_UUID = "00001235-b38d-4985-720e-0f993a68ee41";

static uuid_t g_temperature_level_uuid;
static uuid_t g_humid_level_uuid;

int cmp_uuid_t(const uuid_t* x, const uuid_t* y){
	if (x->type != y->type){
		return false;
	}
	switch(x->type){
		case SDP_UUID16:
			return x->value.uuid16 == y->value.uuid16;
		case SDP_UUID32:
			return x->value.uuid32 == y->value.uuid32;
		case SDP_UUID128:
			return memcmp(&x->value.uuid128, &y->value.uuid128, sizeof(uint128_t))==0;
	}
}

void notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data) {
	if (data_length<4){
		printf("Error: Received too few bytes\n");
		return;
	}
	if (cmp_uuid_t(uuid, &g_temperature_level_uuid)){
		printf("Current temperature is: ");
	}
	if (cmp_uuid_t(uuid, &g_humid_level_uuid)){
		printf("Current humidity is: ");
	}
	
	float* value=(float*)data;
	printf("%f\n", *value);
}

static void usage(char *argv[]) {
	printf("%s <device_address>\n", argv[0]);
}

int main(int argc, char *argv[]) {
	int ret;
	gatt_connection_t* connection;

	if (argc != 2) {
		usage(argv);
		return 1;
	}

		
	if (gattlib_string_to_uuid(TEMPERATURE_UUID, strlen(TEMPERATURE_UUID) + 1, &g_temperature_level_uuid) < 0) {
		fprintf(stderr, "Fail to parse temperature prop UUID.\n");
		return 1;
	}
	if (gattlib_string_to_uuid(HUMID_UUID, strlen(HUMID_UUID) + 1, &g_humid_level_uuid) < 0) {
		fprintf(stderr, "Fail to parse humid prop UUID.\n");
		return 1;
	}

	connection = gattlib_connect(NULL, argv[1], BDADDR_LE_PUBLIC, BT_SEC_LOW, 0, 0);
	if (connection == NULL) {
		fprintf(stderr, "Fail to connect to the bluetooth device.\n");
		return 1;
	}

	gattlib_register_notification(connection, notification_handler, NULL);

	ret = gattlib_notification_start(connection, &g_temperature_level_uuid);
	if (ret) {
		fprintf(stderr, "Fail to start notification\n.");
		return 1;
	}
	ret = gattlib_notification_start(connection, &g_humid_level_uuid);
	if (ret) {
		fprintf(stderr, "Fail to start notification humidity\n.");
		return 1;
	}

	GMainLoop *loop = g_main_loop_new(NULL, 0);
	g_main_loop_run(loop);

	g_main_loop_unref(loop);
	gattlib_disconnect(connection);
	puts("Done");
	return 0;
}
