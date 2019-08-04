#include "communicator.h"

struct communicator* new_communicator(char* server) {
	struct communicator* cmn = (struct communicator*)malloc(sizeof(struct communicator));

	cmn->context = zmq_ctx_new ();
	cmn->value_changes = zmq_socket (cmn->context, ZMQ_PUB);
	int rc = zmq_bind (cmn->value_changes, server);
	assert(rc == 0);
	
	return cmn;
}

void notify_value_changed(struct communicator* cmn, char* property_name, float new_value){
	assert(cmn);
	char buffer[10];
	snprintf(buffer, sizeof(buffer), "%f", new_value);
	s_sendmore(cmn->value_changes, property_name);
	s_send(cmn->value_changes, buffer);
}

void communicator_close(struct communicator* cmn){
	assert(cmn);
	zmq_close (cmn->value_changes);
	zmq_ctx_destroy (cmn->context);
}

