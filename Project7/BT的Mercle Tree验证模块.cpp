#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include <openssl/sha.h>
using namespace std;

// 定义 Merkle Tree 节点结构
typedef struct {
    unsigned char hash[SHA256_DIGEST_LENGTH]; // 哈希值
} MerkleNode;

// 计算块的哈希值
void calculateBlockHash(const unsigned char* data, size_t dataSize, unsigned char* hash) {
    SHA256(data, dataSize, hash);
}

// 构建 Merkle Tree
void buildMerkleTree(const unsigned char** blocks, size_t numBlocks, MerkleNode* merkleTree) {
    size_t numNodes = numBlocks;
    size_t levelOffset = 0;

    // 计算叶子节点的哈希值
    for (size_t i = 0; i < numBlocks; i++) {
        calculateBlockHash(blocks[i], SHA256_DIGEST_LENGTH, merkleTree[levelOffset + i].hash);
    }

    // 逐层计算父节点的哈希值
    for (size_t levelSize = numBlocks; levelSize > 1; levelSize = (levelSize + 1) / 2) {
        for (size_t i = 0; i < levelSize; i += 2) {
            size_t left = levelOffset + i;
            size_t right = left + 1;
            size_t parent = numNodes + (i / 2);

            // 计算父节点的哈希值
            SHA256_CTX ctx;
            SHA256_Init(&ctx);
            SHA256_Update(&ctx, merkleTree[left].hash, SHA256_DIGEST_LENGTH);
            SHA256_Update(&ctx, merkleTree[right].hash, SHA256_DIGEST_LENGTH);
            SHA256_Final(merkleTree[parent].hash, &ctx);
        }

        levelOffset += levelSize;
        numNodes += (levelSize + 1) / 2;
    }
}

// 验证下载的数据的完整性
int verifyDataIntegrity(const unsigned char** blocks, size_t numBlocks, const unsigned char* rootHash) {
    MerkleNode* merkleTree = malloc(sizeof(MerkleNode) * numBlocks * 2);

    // 构建 Merkle Tree
    buildMerkleTree(blocks, numBlocks, merkleTree);

    // 比较根节点的哈希值
    int result = memcmp(merkleTree[numBlocks * 2 - 2].hash, rootHash, SHA256_DIGEST_LENGTH) == 0;

    free(merkleTree);
    return result;
}

int main() {
    // 模拟下载的数据块
    const unsigned char* blocks[] = {
        (const unsigned char*)"block1",
        (const unsigned char*)"block2",
        (const unsigned char*)"block3",
        (const unsigned char*)"block4"
    };
    size_t numBlocks = sizeof(blocks) / sizeof(blocks[0]);

    // 模拟种子文件中的根节点哈希值
    unsigned char rootHash[SHA256_DIGEST_LENGTH] = {
        0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
        0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
        0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
        0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0
    };

    // 验证数据完整性
    int integrity = verifyDataIntegrity(blocks, numBlocks, rootHash);

    if (integrity) {
        printf("数据完整性验证通过！\n");
    }
    else {
        printf("数据完整性验证失败！\n");
    }

    return 0;
}
