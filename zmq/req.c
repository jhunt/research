#include <assert.h>
#include <zmq.h>
#include <stdio.h>

static void *ZMQ;

static void bail(const char *msg)
{
	if (msg) fprintf(stderr, "%s: ", msg);
	fprintf(stderr, "%s\n", zmq_strerror(errno));
}

void req2rep(void)
{
	void *req = zmq_socket(ZMQ, ZMQ_REQ);
	assert(req);

	void *rep = zmq_socket(ZMQ, ZMQ_REP);
	assert(rep);

	int rc = zmq_bind(rep, "inproc://req2rep");
	if (rc != 0) bail("bind");

	rc = zmq_connect(req, "inproc://req2rep");
	if (rc != 0) bail("connect");

	rc = zmq_send(req, "PING\0", 5, 0);
	if (rc < 0) bail("send");
	fprintf(stderr, "sent %i bytes\n", rc);
	assert(rc == 5);

	char buf[8192];
	rc = zmq_recv(rep, buf, 8192, 0);
	if (rc < 0) bail("recv");
	fprintf(stderr, "received %i bytes\n", rc);
	assert(rc == 5);

	rc = strcmp(buf, "PING");
	fprintf(stderr, "payload was '%s'\n", buf);
	assert(rc == 0);
}

void rep2req(void)
{
	void *req = zmq_socket(ZMQ, ZMQ_REQ);
	assert(req);

	void *rep = zmq_socket(ZMQ, ZMQ_REP);
	assert(rep);

	int rc = zmq_bind(req, "inproc://rep2req");
	if (rc != 0) bail("bind");

	rc = zmq_send(req, "PING\0", 5, 0);
	if (rc < 0) bail("send");
	fprintf(stderr, "sent %i bytes\n", rc);
	assert(rc == 5);

	rc = zmq_connect(rep, "inproc://rep2req");
	if (rc != 0) bail("connect");

	char buf[8192];
	rc = zmq_recv(rep, buf, 8192, 0);
	if (rc < 0) bail("recv");
	fprintf(stderr, "received %i bytes\n", rc);
	assert(rc == 5);

	rc = strcmp(buf, "PING");
	fprintf(stderr, "payload was '%s'\n", buf);
	assert(rc == 0);
}

int main(int argc, char **argv)
{
	ZMQ = zmq_ctx_new();
	assert(ZMQ);

	if (argc == 1 || strcmp(argv[1], "req2rep") == 0) {
		fprintf(stdout, "REQ -> REP\n");
		req2rep();
	} else if (strcmp(argv[1], "rep2req") == 0) {
		fprintf(stdout, "REP -> REQ\n");
		rep2req();
	}

	return 0;
}
