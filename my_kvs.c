#include "my_kvs.h"
#include <cstdio>
#include <cstring>

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

Node *insertionnode(Node * node, char *key, char *value)
{
	if (node == NULL) {
		Node *newnode = (Node*)malloc(sizeof(Node));
		newnode->nkey = key;
		newnode->nvalue = value;
		newnode->right = NULL;
		newnode->left = NULL;
		return newnode;
	} else if (strcmp (node->nkey , key)==1) {
		node->left = insertionnode(node, key, value);
		return node;
	} else if (strcmp(node->nkey, key)==-1) {
		node->right = insertionnode(node, key, value);
		return node;
	} else if (strcmp(node->nkey , key)==0) {
		node->nvalue = value;
		return node;
	} else {
		return NULL;
	}
}

int my_kvs_set(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_value *value, struct kvs_context *ctx)
{
	if (count == 0) {
		root = insertionnode(NULL, key->key, value->value);
	} else {
		insertionnode(root, key->key, value->value);
	}
	return 0;
}

Node *searchnodes(Node * node, char *key)
{
	if (strcmp(node->nkey ,key)==1) {
		searchnodes(node->left, key);
		return node;
	} else if (strcmp(node->nkey, key)==-1) {
		searchnodes(node->right, key);
		return node;
	} else if (strcmp(node->nkey, key)==0) {
		return node;
	} else {
		return NULL;
	}
}

int my_kvs_get(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_value *value, struct kvs_context *ctx)
{
	Node *schnode = searchnodes(root, key->key);
	//printf("%s", schnode->nvalue);
	strcpy(value->value, schnode->nvalue);
	return 0;
}

void deletionnode(Node * pnode, Node * node, char *key)
{
	if (node != NULL) {
		if (strcmp(node->nkey , key)!=0) {	// 내가 삭제할 키랑 현재 노드의 키가 같지 않을 때
			if (strcmp(node->nkey, key)==1)	{
				deletionnode(node, node->left, key);	// 루트보다 내가 찾는 키가 작으면 왼쪽으로 가서 재귀
			} else if (strcmp(node->nkey, key)==-1) {
				deletionnode(node, node->right, key);	// 반대일 때는 오른쪽으로 가서 재귀
			}
		} else {	// 현재 노드랑 내가 지울 노드의 키가 같을 때
			if (node->left == NULL && node->right != NULL) {	// 그 노드의 위치에 대한 경우의수 4가지 준비하고
				if (pnode == NULL) {	// 지울 노드가 루트일 때는 루트 자식들로 루트를 대체하고 ( 포인터 느낌) 그리고 원래 루트는 프리시켜준 다음 루트노드 위치 갱신 후 함수 끝내주기
					node = node->right;
					free(root);
					root = node;
					return;
				} else if (pnode->right == node) {
					pnode->right = node->right;
				} else if (pnode->left == node) {
					pnode->left = node->right;
				} else
					printf("error");
				free(node);
			} else if (node->left != NULL && node->right == NULL) {
				if (pnode == NULL) {
					node = node->left;
					free(root);
					root = node;
					return;
				} else if (pnode->right == node) {
					pnode->right = node->left;
				} else if (pnode->left == node) {
					pnode->left = node->left;
				} else
					printf("error");
				free(node);
			} else if (node->left != NULL && node->right != NULL) {
				if (pnode == NULL) {
					node = node->left;
					free(root);
					root = node;
					return;
				} else if (pnode->right == node) {
					pnode->right = node->left;
				} else if (pnode->left == node) {
					node->left->right = node->right;
					pnode->left = node->left;
				} else
					printf("error");
				free(node);
			} else {
				if (pnode == NULL) {
					free(root);
					root = NULL;
					return;
				}
				if (pnode->right == node) {
					pnode->right = NULL;
				}
				if (pnode->left == node) {
					pnode->left = NULL;
				}
				free(node);
			}
		}
	}
	return;
}

int my_kvs_del(struct my_kvs *my_kvs, struct kvs_key *key, struct kvs_context *ctx)
{
	deletionnode(NULL, root, key->key);
	return 0;
}

