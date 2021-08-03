#include "my_kvs.h"
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

Node *root = NULL;
int count = 0;

int my_kvs_init(struct my_kvs **my_kvs)
{
	*my_kvs = (struct my_kvs *)calloc(1, sizeof(struct my_kvs));
	return 0;
}

int my_kvs_destroy(struct my_kvs *my_kvs)
{
	free(my_kvs);
	return 0;
}




Node *insertionnode(Node * node, char *key, char *value,kvs_key_t klen, kvs_value_t vlen, uint64_t hash )
{
	if (node == NULL) {
		Node *newnode = (Node*)malloc(sizeof(Node));
		newnode->nkey = (char*)malloc(sizeof(char)*klen);
		newnode->nvalue = (char*)malloc(sizeof(char)*vlen);
		newnode->hash = hash;
		strcpy(newnode->nkey,key);
		strcpy(newnode->nvalue,value);
		newnode->right = NULL;
		newnode->left = NULL;
		return newnode;
	}


	if (node->hash > hash) {
		node->left = insertionnode(node->left, key, value,klen,vlen,hash);
		return node;
	} else if (node->hash < hash) {
		node->right = insertionnode(node->right, key, value, klen ,vlen, hash);
		return node;
	} else if (node->hash == hash) {
		free(node->nvalue);
		node->nvalue = (char*)malloc(sizeof(char)*vlen);
		strcpy(node->nvalue,value);
		return node;
	} else {
		return NULL;
	}
}

int my_kvs_set(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_value *value, struct kvs_context *ctx)
{
	uint64_t hash;
	hash = XXH64(key->key, key->klen,0);
	Node *node = insertionnode(root, key->key, value->value, key->klen, value->vlen,hash);

	if (root == NULL)
		root = node;

	return 0;
}

Node *searchnodes(Node * node, char *key, uint64_t hash)
{
	if (node->hash > hash) {
		searchnodes(node->left, key,hash);
		return node;
	} else if (node->hash < hash) {
		searchnodes(node->right, key,hash);
		return node;
	} else if (node->hash == hash ) {
		return node;
	} else {
		return NULL;
	}
}

int my_kvs_get(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_value *value, struct kvs_context *ctx)
{
	uint64_t hash;
	hash = XXH64(key->key, key->klen,0);
	Node *schnode = searchnodes(root, key->key, hash);
	strcpy(value->value, schnode->nvalue);
	return 0;
}



void freenode(Node *node)
{
	free(node->nkey);
	free(node->nvalue);
	free(node);
	return;
}

Node * findmin(Node *root)
{
	Node *min = root;
	while(min->left != NULL){
		min = min->left;
	}
	return min;
}

Node * deletionnode( Node * node, uint64_t hash,bool *success)
{
	Node *pnode= NULL;
//	int count = 0; 여기다 선언하면 재귀할 때 영향받음


	if (node == NULL){
		return NULL;
	}


	if (node->hash != hash) {	// 내가 삭제할 키랑 현재 노드의 키가 같지 않을 때
		if (node->hash >hash) {
			node->left=deletionnode(node->left,hash,success);	// 루트보다 내가 찾는 키가 작으면 왼쪽으로 가서 재귀
		} else if (node->hash < hash) {
			node->right= deletionnode(node->right, hash,success);	// 반대일 때는 오른쪽으로 가서 재귀
		}
	}

	// 현재 노드랑 내가 지울 노드의 키가 같을 때
	 else{
		 if (node->left != NULL && node->right != NULL) {
			pnode = findmin(node->right);
			node->hash=pnode->hash;
			free(node->nkey);
			free(node->nvalue);
			node->nkey = pnode->nkey;
			node->nvalue=pnode->nvalue;
			count ++;
			node->right = deletionnode(node->right,pnode->hash,success);
		} else {
			pnode = (node ->left == NULL) ? node->right : node->left;

			if (count == 0){
				freenode(node);
			}
			else {
				free(node);
				count = 0; // 큰 가르침
			}
			*success = true;
			return pnode;
		}
	}
	return node;
}

int my_kvs_del(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_context *ctx)
{      
       	bool success=false;	
	uint64_t hash;
	hash = XXH64(key->key, key->klen,0);
	root = deletionnode(root,hash,&success);


	if (success == true)
		return 0;
	else
		return -1;
}

