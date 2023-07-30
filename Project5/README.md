###软件环境：Visual Studio 2019 硬件环境：Intel(R) Core(TM) i5-10210U CPU @ 1.60GHz 2.11 GHz 
代码实现思路：
Merkle Tree是Dynamo中用来同步数据一致性的算法，Merkle Tree是基于数据HASH构建的一个树。它具有以下几个特点：
1、数据结构是一个树，可以是二叉树，也可以是多叉树（本BLOG以二叉树来分析）
2、Merkle Tree的叶子节点的value是数据集合的单元数据或者单元数据HASH。
3、Merke Tree非叶子节点value是其所有子节点value的HASH值。
对数据块做hash运算,相邻两个hash块串联，然后做hash运算。重复将相邻两个hash块串联，然后做hash运算，直到最后只剩一个结点。
可以通过根哈希保证整体数据完整性。
