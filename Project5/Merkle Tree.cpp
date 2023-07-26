#include <stdio.h>
#include <stdlib.h>
#include<iostream>
using namespace std;

typedef unsigned int uint;

// Merkle Tree 结构体定义 
typedef struct MerkleTreeNode {
	struct MerkleTreeNode* left;		
	struct MerkleTreeNode* right;		
	struct MerkleTreeNode* parent;		
	uint level;							// 当前树深，底层为0 
	uint data;							// 当前节点的值，最底层为原始数据，其上均为计算的哈希值 
}MerkleTree;

// 定义宏函数，创建一个 Merkle Tree节点
#define New_Merkle_Node(mt, tree_depth) {	\
	mt = (MerkleTree *)malloc(sizeof(MerkleTree)); \
	mt->left = NULL; \
	mt->right = NULL; \
	mt->parent = NULL; \
	mt->level = (uint)tree_depth; \
	mt->data = 0;	\
	}

// 打印 Merkle tree，中续遍历方式 
void Print_Merkle_Tree(MerkleTree* mt, int high)
{
	MerkleTree* p = mt;
	int i;

	if (p == NULL)
		return;

	if (p->left == NULL && p->right == NULL) {
		for (i = 0; i < high - p->level; i++)
			printf("           ");

		printf("------>%s\n", p->data);
	}
	else {
		Print_Merkle_Tree(mt->left, high);
		printf("\n");
		for (i = 0; i < high - p->level; i++)
			printf("           ");

		printf("------>%-6d\n", p->data);

		Print_Merkle_Tree(mt->right, high);
	}
}


// 计算两个整数的hash 值 
uint hash_two_node(uint num1, uint num2) {
	uint cal = 9, hash = 0;
	hash = num1 + num2;
	hash *= cal;
	return hash & 0x7FFFFFFF;
}

// 遍历二叉树，找到最后一个叶子节点，遍历方式为前续遍历方式 
MerkleTree* Find_Last_Node(MerkleTree* mt) {
	MerkleTree* p = mt, * tmp;

	// 如果左右子节点都是NULL，肯定是叶子节点，直接返回 
	if (p->left == NULL && p->right == NULL)
		return p;
	// 如果 右子节点为空，左子节点不为空，继续遍历左子节点，直到叶子节点 
	else if (p->right == NULL && p->left != NULL)
		return Find_Last_Node(p->left);
	// 如果 右子节点不为空，遍历右子节点 
	else if (p->right != NULL)
		return Find_Last_Node(p->right);
}

// 根据最后一个节点，找到插入的位置 
MerkleTree* Find_Empty_Node(MerkleTree* mt) {
	MerkleTree* p = mt->parent;

	// 向上找到第一个存在空缺的节点，直到顶节点 
	while (p->left != NULL && p->right != NULL && p->parent != NULL)  p = p->parent;

	// 如果 p 指向顶节点，说明当前是满二叉树 ，返回 NULL 
	if (p->parent == NULL && p->left != NULL && p->right != NULL)
		return NULL;
	// 否则当前节点就是要插入节点的位置 
	else
		return p;
}

// Merkle tree 初始化 (递归实现)
MerkleTree* Creat_Merkle_Tree(MerkleTree* mt, int* arr, int nums)
{
	MerkleTree* node, * tmp, * p;
	int i;

	if (nums == 0) {

		printf("Merkle Tree 创建完毕！！！\n");
		return mt;
	}
	else {
		New_Merkle_Node(node, 0);
		node->data = *arr;
		printf("新建叶节点 [%d] tree_depth=%d, level=%d, data=%-6d, nums=%d, \n", __LINE__, mt == NULL ? 0 : mt->level, node->level, node->data, nums);

		// 如果 mt 为空，说明当前没有树，需要新建一个头结点 
		if (mt == NULL) {
			New_Merkle_Node(mt, 1);
			mt->left = node;
			node->parent = mt;
			printf("新建头节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, mt->level, mt->data);
			mt->data = hash_two_node(mt->left->data, mt->right == NULL ? 0 : p->right->data);
			mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1);
		}
		//如果 mt 不为空，说明当前已经有树了 
		else
		{
			//遍历当前树，找到一个空的叶子节点，满二叉树时返回NULL 
			p = Find_Empty_Node(Find_Last_Node(mt));

			//如果返回值不为 NULL, 说明已经找到需要插入的节点 
			if (p != NULL) {
				if (p->left->left == NULL && p->right == NULL)
				{
					p->right = node;
					node->parent = p;
					// 更新哈希值 
					p->data = hash_two_node(p->left->data, p->right == NULL ? 0 : p->right->data);
				}
				//如果返回的切点是中间节点 
				else
				{     
					i = p->level - 1;
					New_Merkle_Node(tmp, i);
					p->right = tmp;
					tmp->parent = p;
					printf("新中间节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, tmp->level, tmp->data);

					p = p->right;
					i = p->level - 1;		
					while (i > 0) {
						// 创建中间节点 
						New_Merkle_Node(tmp, i);
						p->left = tmp;
						tmp->parent = p;
						printf("新中间节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, tmp->level, tmp->data);

						p = p->left;
						i--;
					}

					p->left = node;
					node->parent = p;

					while (p != mt) {
						p->data = hash_two_node(p->left->data, p->right == NULL ? 0 : p->right->data);
						p = p->parent;
					}
					p->data = hash_two_node(p->left->data, p->right == NULL ? 0 : p->right->data);

				}
				mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1);
			}
			//如果没有空的节点，说明当前是满二叉树，需要新增头节点了，level也要加1 
			else
			{
				tmp = mt;				
 
				New_Merkle_Node(mt, tmp->level + 1);
				mt->left = tmp; 		
				tmp->parent = mt;
				printf("新建头节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, mt->level, mt->data);
				New_Merkle_Node(tmp, mt->level - 1);
				mt->right = tmp;
				tmp->parent = mt;
				printf("新中间节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, tmp->level, tmp->data);

				p = mt->right;
				i = p->level - 1;

				// 根据树的深度创建同样深度的左树 
				while (i > 0) {
					New_Merkle_Node(tmp, i);
					p->left = tmp;
					tmp->parent = p;
					printf("新中间节点 [%d] tree_depth=%d, level=%d, data=%-6d \n", __LINE__, mt->level, tmp->level, tmp->data);

					p = p->left;
					i--;
				}
				p->left = node;
				node->parent = p;

				// 自底向上更新哈希值
				while (p != mt) {
					p->data = hash_two_node(p->left->data, p->right == NULL ? 0 : p->right->data);
					p = p->parent;
				}
				// 更新父节点哈希值  
				p->data = hash_two_node(p->left->data, p->right == NULL ? 0 : p->right->data);

				// 递归调用 
				mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1);
			}
		}
	}
}


int main(void)
{
	int array[] = { 11, 22, 33 ,44 ,55 ,66, 77, 88, 99, 1010, 1111, 1212 };
	MerkleTree* mt = NULL;

	// 根据数组动态创建 Merkle Tree 
	mt = Creat_Merkle_Tree(mt, array, sizeof(array) / sizeof(int));

	// 打印当前merkle Tree
	if (mt != NULL) {
		printf("\n开始打印当前 Merkle 树:\n");
		Print_Merkle_Tree(mt, mt->level);
		printf("\n\n");
	}

	return 0;
}

