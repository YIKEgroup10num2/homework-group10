#include <stdio.h>
#include<iostream>
using namespace std;

typedef unsigned int uint;

// Merkle Tree 
typedef struct MerkleTreeNode {
	struct MerkleTreeNode* left;
	struct MerkleTreeNode* right;
	struct MerkleTreeNode* parent;
	uint hash_num;		// 哈希值 ，如果创建树过程中，该值为当前节点的树深度 
	uint data;
}MerkleTree;

#define New_Merkle_Node(mt, tree_depth) {	\
	mt = (MerkleTree *)malloc(sizeof(MerkleTree)); \
	mt->left = NULL; \
	mt->right = NULL; \
	mt->parent = NULL; \
	mt->hash_num = (uint)tree_depth; \
	mt->data = 0;	\
	}

// 打印 Merkle tree 
int first = 0;
void Print_Merkle_Tree(MerkleTree* mt, int high)
{
	MerkleTree* p = mt;
	int i;

	if (p == NULL) {
		return;
	}
	if (p->left == NULL && p->right == NULL) {
		printf("\n");
		for (i = 0; i < high - p->hash_num; i++)
			printf("      ");

		printf("--->%2d\n", p->data);
		first = 1;

		return;
	}
	else {
		Print_Merkle_Tree(mt->left, high);

		if (first == 1) {
			for (i = 0; i < high - p->hash_num; i++)
				printf("      ");

			printf("--->");
		}
		else
			printf("--->");

		printf("%2d", p->hash_num);
		first = 0;

		Print_Merkle_Tree(mt->right, high);
		//printf("\n");
	}
}

// 计算一个字符串的hash值 
uint hash_string(char* key) {
	uint cal = 11, hash = 0;
	while (*key != '\0' && *key != 0) {
		hash = hash * cal + *key;
		key++;
	}
	return hash & 0x7FFFFFFF;
}

// 计算一个字符串的hash值 
uint hash_int(int num) {
	uint cal = 11, hash = 0;

	hash = num * cal;
	return hash & 0x7FFFFFFF;
}

// 计算两个整数的hash 值 
uint hash_uint(uint num1, uint num2) {
	uint cal = 9, hash = 0;
	hash = num1 + num2;
	hash *= cal;
	return hash & 0x7FFFFFFF;
}

// 遍历二叉树，如果找到空的叶子节点返回叶子结点指针
// 如果没有找到，即为满二叉树，则返回 NULL 
MerkleTree* Find_Last_Node(MerkleTree* mt) {
	MerkleTree* p = mt, * tmp;

	if (p->left == NULL && p->right == NULL)	// 叶子节点 
		return p;
	else if (p->right == NULL && p->left != NULL)
		return Find_Last_Node(p->left);
	else if (p->right != NULL)
		return Find_Last_Node(p->right);
}

// 根据最后一个节点，找到插入的位置 
MerkleTree* Find_Empty_Node(MerkleTree* mt) {
	MerkleTree* p = mt->parent;

	while (p->left != NULL && p->right != NULL && p->parent != NULL) {
		p = p->parent;
	}
	if (p->parent == NULL && p->left != NULL && p->right != NULL) {		// 到顶了
		//printf("当前节点位置，p->hash_num=%d, 到顶了！！！ \n", p->hash_num); 
		return NULL;
	}
	else {
		//printf("当前节点位置，p->hash_num=%d \n", p->hash_num); 
		return p;
	}
}

// 更新树的哈希值 
void update_hash_Merkle(MerkleTree* mt, int tree_depth)
{
	if (mt->hash_num == 0) {
		mt->hash_num = 0;
	}
	else if (mt->hash_num == 1) {
		mt->hash_num = hash_uint(hash_int(mt->left->data), hash_int(mt->right->data));
	}
	else {
		update_hash_Merkle(mt->left, tree_depth - 1);
		update_hash_Merkle(mt->right, tree_depth - 1);
		mt->hash_num = hash_uint(mt->left->hash_num, mt->right->hash_num);
	}
}

// Merkle tree 初始化 (递归实现)
MerkleTree* Creat_Merkle_Tree(MerkleTree* mt, int* arr, int nums, int tree_depth)
{
	MerkleTree* node, * tmp, * p;
	int i;
	if (nums == 0) {
		update_hash_Merkle(mt, tree_depth);
		printf("创建完毕\n");

		if (mt != NULL) {
			first = 0;
			printf("\n开始打印当前 Merkle 树:\n");
			Print_Merkle_Tree(mt, mt->hash_num);
			printf("\n");
		}
		return mt;
	}
	else {
		printf("叶子节点 [%d] arr=%d, nums=%d, tree_depth=%d\n", __LINE__, *arr, nums, tree_depth);
		// 每次添加一个叶子节点，优先左树，其次右树
		// 创建一个树结点 
		New_Merkle_Node(node, 0);
		node->data = *arr;

		// 如果 mt 为空，说明当前没有树	
		if (mt == NULL) {
			New_Merkle_Node(mt, 1);
			mt->left = node; 	
			node->parent = mt;
			tree_depth++;
			printf("新头结点 [%d] tree_depth=%d, mt->hash_num=%d\n", __LINE__, tree_depth, mt->hash_num);
			mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
		}
		// 如果 mt 不为空,mt为头结点 
		else
		{
			p = Find_Empty_Node(Find_Last_Node(mt));	// 遍历当前树，找到一个空的叶子节点，满二叉树时返回NULL 

			// 如果flag 为1 说明存在空的 右叶子节点 
			if (p != NULL) {
				// 如果最底下就是叶子节点，就直接赋值 
				if (p->left->hash_num == 0 && p->right == NULL)
				{
					p->right = node;
					node->parent = p;
				}
				else
				{
					i = p->hash_num - 1;
					New_Merkle_Node(tmp, i);
					p->right = tmp;
					tmp->parent = p;

					p = p->right;
					printf("插入结点 [%d] tree_depth=%d, hash_num=%d\n", __LINE__, tree_depth, p->hash_num);

					i--;
					// 根据树的深度创建同样深度的左树 
					while (i > 0) {
						New_Merkle_Node(tmp, i);
						p->left = tmp;
						tmp->parent = p;

						p = p->left;
						i--;
					}
					p->left = node;
					node->parent = p;
				}
				mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
			}
			//如果没有空的叶子节点，则新建一个头结点 
			else
			{
				tmp = mt;	
				tree_depth++; 		

				// 创建一个新的头结点
				New_Merkle_Node(mt, tree_depth);
				mt->left = tmp; 	
				tmp->parent = mt;
				i = tree_depth - 1;	
				New_Merkle_Node(tmp, i);
				mt->right = tmp;
				tmp->parent = mt;

				i--;
				p = mt->right;
				printf("插入结点 [%d] tree_depth=%d, hash_num=%d\n", __LINE__, tree_depth, p->hash_num);

				// 根据树的深度创建同样深度的左树 
				while (i > 0) {
					New_Merkle_Node(tmp, i);
					p->left = tmp;
					tmp->parent = p;

					p = p->left;
					i--;
				}
				
				p->left = node;
				node->parent = p;
				mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
			}
		}
	}
}

int main(void)
{
	// , This Is Cielle.
	int array[] = { 12, 23, 34 ,45 ,56 ,67, 78, 89, 90, 101, 1212, 1234 };
	MerkleTree* mt = NULL;

	Creat_Merkle_Tree(mt, array, sizeof(array) / sizeof(int), 0);


	return 0;
}
