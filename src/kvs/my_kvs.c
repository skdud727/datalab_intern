#include "my_kvs.h"

void  my_kvs_set_env (struct my_kvs *my_kvs, KVS_SET my_kvs_set, KVS_GET my_kvs_get, KVS_DEL my_kvs_del) {
	my_kvs->my_kvs = my_kvs;
	my_kvs->set = my_kvs_set;
	my_kvs->get = my_kvs_get;
	my_kvs->del = my_kvs_del;
	return;
}

int my_kvs_init (struct my_kvs **my_kvs) {
	*my_kvs = (struct my_kvs *)calloc(1, sizeof (struct my_kvs));
	return 0;
}

int my_kvs_destroy (struct my_kvs *my_kvs) {
	free(my_kvs);
	return 0;
}

int my_kvs_set (struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_value *value, struct kvs_context *ctx) {
	return 0;
}

int my_kvs_get (struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_value *value, struct kvs_context *ctx) {
	return 0;
}

int my_kvs_del (struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_context *ctx) {
	return 0;
}
