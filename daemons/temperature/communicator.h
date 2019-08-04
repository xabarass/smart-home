#include <zmq.h>
//TODO: Remove helpers!
#include "zhelpers.h"

struct communicator {
	void* context;
	void* value_changes;
};

struct communicator* new_communicator(char* server);
void notify_value_changed(struct communicator* cmn, char* property_name, float new_value);
void communicator_close(struct communicator* cmn);
