#include <zmq.h>
//TODO: Remove helpers!
#include "zhelpers.h"

struct communicator {
	void* context;
	void* value_changes;
};

struct communicator* new_communicator(char* server);
void value_changed(struct communicator* cmn, char* property_name, float new_value);
