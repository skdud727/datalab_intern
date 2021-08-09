#include "my_kvs.h"
#include <stdio.h>
#include <cstdio>
#include <stdbool.h>
#include <cstring>
#define XXH_INLINE_ALL
#include "xxhash.h"

void my_kvs_set_env(struct my_kvs *my_kvs, KVS_SET my_kvs_set, KVS_GET my_kvs_get, KVS_DEL my_kvs_del)
{
	my_kvs->my_kvs = my_kvs;
	my_kvs->set = my_kvs_set;
	my_kvs->get = my_kvs_get;
	my_kvs->del = my_kvs_del;
	return;
}

Node *head = NULL;

int my_kvs_init(struct my_kvs **my_kvs)
{
	*my_kvs = (struct my_kvs *)calloc(1, sizeof(struct my_kvs));
	Node *first = (Node *) malloc(sizeof(Node));
	head = first;		// 다른 함수에서도 head가 바뀌려면
	first->next = NULL;
	first->pre = NULL;
	first->hash = 0;
	return 0;
}

int my_kvs_destroy(struct my_kvs *my_kvs)
{
	free(my_kvs);
	return 0;
}

void printnodes(Node * node)
{
	if (node == NULL) {
		printf("여기 노드 없어요");
	} else {
		while ((node->next) != NULL) {
			printf("%s\n", node->next->nkey);
			node = node->next;
		}
		printf("끝");
	}

}

Node * mnode ( char * key, char * value, kvs_key_t klen, kvs_value_t vlen, uint64_t hash)
{

	Node *newnode = (Node *) malloc(sizeof(Node));
	newnode->nkey = (char *)malloc(sizeof(char) * klen);
	newnode->nvalue = (char *)malloc(sizeof(char) * vlen);
	newnode->hash = hash;
	strcpy(newnode->nkey, key);
	strcpy(newnode->nvalue, value);
	return newnode;
}

int  *insertionnode(Node * node, char *key, char *value, kvs_key_t klen, kvs_value_t vlen,uint64_t hash)
{

	if (node== NULL){
		Node *newnode = mnode (key, value, klen, vlen,  hash );
		newnode->pre=head;
		newnode->next = NULL;
		head->next=newnode;
		head->pre = NULL;
		return 0; 
	} else if (node->hash <hash) {	
		if (node->next == NULL){
		Node *newnode=mnode( key,value, klen,  vlen, hash);
			newnode->pre = node;
			newnode->next = NULL;
			node->next = newnode;
			return 0;
		} else {
			insertionnode (node->next,key,value,klen,vlen, hash);
			return 0;}
	} else if (node->hash > hash) {
		Node *newnode=mnode(key, value, klen, vlen,  hash);
		newnode->pre=node->pre;
		newnode->next= node;
		node->pre->next=newnode;
		node->pre=newnode;
		return 0;
	} else if (node->hash == hash){
		free(node->nvalue);
	       	node->nvalue = (char *)malloc (sizeof(char)*vlen);
		strcpy(node->nvalue,value);
		return 0;	
	} else
		return NULL;

}

int my_kvs_set(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_value *value, struct kvs_context *ctx)
{
	uint64_t hash;
	hash = XXH64(key->key, key->klen, 0);
	insertionnode(head->next, key->key, value->value, key->klen, value->vlen , hash);
	return 0;
}

Node *searchnodes(Node * node, char *key, uint64_t hash)
{
	while (node->hash < hash) {
		node = node->next;
	}
	if (node->hash == hash) {
		return node;
	}
	return node;

}

int my_kvs_get(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_value *value, struct kvs_context *ctx)
{
	uint64_t hash;
	hash = XXH64(key->key, key->klen, 0);
	Node *schnode = searchnodes(head, key->key, hash);
	strcpy(value->value, schnode->nvalue);
	return 0;
}

void freenode(Node * node)
{
	free(node->nkey);
	free(node->nvalue);
	free(node);
	return;
}

Node *deletionnode(Node * node, uint64_t hash, bool *success)
{

	if (node == NULL){
	      return NULL;
	}

	if (node->hash == hash) {
		if (node->next != NULL) {
			if (node->pre == head) {
				head->next = node->next;
				node->next->pre = head;
				freenode(node);
			} else {
				(node->pre)->next = node->next;
				(node->next)->pre = node->pre;
				freenode(node);

			}
		} else {
			(node->pre)->next = NULL;
			freenode(node);
		}
	} else {
		if (node->next == NULL) {
			return node;
		}
		else {
			deletionnode(node->next, hash, success);
			return node; // 큰 가르침 ---- 여기서 리턴 해줘야 함수를 나가는 과정에서 완전히 끝날 수 있음
		}
	}

	*success = true;
	return node;

}

int my_kvs_del(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_context *ctx)
{
	bool success = false;
	uint64_t hash;
	hash = XXH64(key->key, key->klen, 0);
	deletionnode(head->next, hash, &success);


	if (success == true) {
		return 0;
	} else
		return -1;
}
