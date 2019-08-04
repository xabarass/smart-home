#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "communicator.h"
#include "gattlib.h"

#define MAX_PROPERTIES 10

struct property;

typedef void (*print_prop)(struct property* prop);
typedef void (*prop_updated)(struct property* prop, const uint8_t* new_data, size_t data_length);

struct property {
	// Parsed UUID of the characteristic
	uuid_t uuid;
	// Property name (Human readable)
	char* name;
	// Propery size in bytes
	size_t buff_len;
	// function pointer for value update
	prop_updated update;
	// debug function
	print_prop debug_print;
	// Notification handler
	struct communicator* cmn;
};

struct float_property {
	struct property prop;
	float value;
};

void update_float_prop(struct property* prop, const uint8_t* new_data, size_t data_length){
	struct float_property* fl_prop = (struct float_property*)prop;
	fl_prop->value=*((const float*)new_data);
	notify_value_changed(prop->cmn, prop->name, fl_prop->value);
}

void print_float_prop(struct property* prop){
	struct float_property* fl_prop = (struct float_property*)prop;
	printf("%s: %f\n", fl_prop->prop.name, fl_prop->value);
}

bool create_float_property(struct property** element, char* UUID, char* name, struct communicator* cmn){
	struct float_property* new_prop = (struct float_property*)malloc(sizeof(struct float_property));
	*element = (struct property*)new_prop;
	// parse uuid
	if (gattlib_string_to_uuid(UUID, strlen(UUID) + 1, &new_prop->prop.uuid) < 0) {
		fprintf(stderr, "Fail to parse %s prop UUID.\n", name);
		return false;
	}
	// Fill in metadata
	new_prop->prop.name=name;
	new_prop->prop.buff_len=4;
	new_prop->prop.update=update_float_prop;
	new_prop->prop.debug_print=print_float_prop;
	new_prop->prop.cmn = cmn;
	
	return true;
}
struct state {
	struct property* properties[MAX_PROPERTIES];
	size_t count;
	struct communicator* cmn;
};

bool init_properties(struct state* program_state){
	if (!create_float_property(&program_state->properties[program_state->count++],
		"00002235-b38d-4985-720e-0f993a68ee41","Temperature", program_state->cmn)){
		return false;
	}
	if (!create_float_property(&program_state->properties[program_state->count++],
		"00001235-b38d-4985-720e-0f993a68ee41","Humidity", program_state->cmn)){
		return false;
	}

	return true;
}

void free_state(struct state* program_state){
	for(int i=0; i<program_state->count; i++){
		free(program_state->properties[i]);
	}
}

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
	struct state* program_state = (struct state*) user_data;
	for(int i=0; i<program_state->count; i++){
		struct property* current=program_state->properties[i];
		if (cmp_uuid_t(&current->uuid, uuid)){
			if (data_length>=current->buff_len){
				current->update(current, data, data_length);
				current->debug_print(current);
			}else{
				printf("Buffer to small for property %s\n", current->name);
			}
		}
	}
}

bool start_notifications(struct state* program_state, gatt_connection_t* connection){
	int ret;
	
	for (int i=0; i<program_state->count; i++){
		ret = gattlib_notification_start(connection, &program_state->properties[i]->uuid);
		if (ret){
			fprintf(stderr, "Fail to start notification for %s\n.", program_state->properties[i]->name);
			return false;
		}
	}

	return true;
}

int main(int argc, char *argv[]) {
	int ret;
	gatt_connection_t* connection;

	if (argc != 2) {
		printf("Specify devices ID\n");
		return 1;
	}
	
	struct state program_state = {.count=0};
	program_state.cmn=new_communicator("tcp://*:5556");
	if (!init_properties(&program_state)){
		goto cleanup;
	}

	connection = gattlib_connect(NULL, argv[1], BDADDR_LE_PUBLIC, BT_SEC_LOW, 0, 0);
	if (connection == NULL) {
		fprintf(stderr, "Fail to connect to the bluetooth device.\n");
		goto cleanup;
	}
	
	gattlib_register_notification(connection, notification_handler, &program_state);
	
	if (!start_notifications(&program_state, connection)){
		goto cleanup;
	}

	GMainLoop *loop = g_main_loop_new(NULL, 0);
	g_main_loop_run(loop);

	g_main_loop_unref(loop);
	gattlib_disconnect(connection);
	puts("Done");
cleanup:
	puts("Running cleanup...");
	free_state(&program_state);
	return 0;
}
