#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define HBASE 256
#define TBASE 2
#define MAX_LEN 100000
#define swap(a, b) { \
	__typeof(a) temp; \
	temp = a; \
	a = b; \
	b = temp; \
}

typedef struct Node {
	int flag;
	struct Node *next[TBASE];
} Node, *Trie;

typedef struct HFNode {
	int data, freq;
	struct HFNode *lchild, *rchild;
} HFNode;

HFNode *get_Node() {
	HFNode *p = (HFNode *)malloc(sizeof(HFNode));
	p->freq = p->data = 0;
	p->lchild = p->rchild = NULL;
	return p;
}

void build(int n, HFNode *arr[n]) {
	for (int times = 0; times < n - 1; times++) {
		HFNode *minNode = arr[0];
		int ind = 0;
		for (int i = 1; i < n - times; i++) {
			if (arr[i]->freq >= minNode->freq) continue;
			minNode = arr[i];
			ind = i;
		}
		swap(arr[ind], arr[n - times - 1]);
		minNode = arr[0];
		ind = 0;

		for (int i = 1; i < n - times; i++) {
			if (arr[i]->freq >= minNode->freq) continue;
			minNode = arr[i];
			ind = i;
		}
		swap(arr[ind], arr[n - times - 2]);
		HFNode *new_node = get_Node();
		new_node->lchild = arr[n - times - 1];
		new_node->rchild = arr[n - times - 2];
		new_node->freq = arr[n - times - 1]->freq + arr[n - times - 2]->freq;
		arr[n - times - 2] = new_node;
	}
	return ;
}

void get_cn(unsigned char *str, int *CN) {
	for(int i = 0; str[i]; ++i) {
		CN[str[i]]++;
	}
}

void extract(HFNode *root, char *buff, char (*huffman_code)[100], int n) {
	buff[n] = '\0';
	if (root->lchild == NULL && root->rchild == NULL) {
		strcpy(huffman_code[root->data], buff);
		//printf("%d %d : %s\n",root->data, root->freq, buff);
		return ; 
	}
	buff[n] = '0';
	extract(root->lchild, buff, huffman_code, n + 1);
	buff[n] = '1';
	extract(root->rchild, buff, huffman_code, n + 1);
	return ;
}

int node_cnt = 0;
Node *get_new_node() {
	Node *p = (Node *)calloc(sizeof(Node), 1);
	p->flag = 0;
	node_cnt += 1;
	return p;
}

void clear(Trie root) {
	if (!root) return;
	for (int i = 0; i < TBASE; ++i) clear(root->next[i]);
	free(root);
	return ;
}

Node *insert(Trie root, const  char *str) {
	if (!root) root = get_new_node();
	Node *p = root;
	for (int i = 0; str[i]; ++i) {
		int ind = str[i] - '0';
		if (!p->next[ind]) p->next[ind] = get_new_node();
		p = p->next[ind];
	}
	p->flag = 1;
	return root;
}

int search_cnt = 0; 
int search(Trie root, const char *str) {
	Node *p = root;
	int i = 0;
	while (p && str[i]) {
		search_cnt++;
		int ind = str[i++] - '0';
		p = p->next[ind];
	}
	return (p && p->flag);
}

int main() {
	HFNode *arr[HBASE] = {0};
	FILE *fp = NULL;
	unsigned char buffer[MAX_LEN];
	int CN[HBASE] = {0};
	fp = fopen("/Users/zou-jianfeng/HZ/ZY/3.txt", "r"); //读取60M中文文本
	while (~fscanf(fp, "%s", buffer)) {
		//printf("%s\n", buffer);
		get_cn(buffer, CN); //获取字节相应数出现频率存入CN数组中
	}
   	fclose(fp);

   	int n = 0;
	for (int i = 0; i < HBASE; ++i) {
		if(!CN[i]) continue;
		HFNode *new_node = get_Node();
		new_node->data = i;
		new_node->freq = CN[i];
		arr[n++] = new_node; 
	}
	build(n, arr); //构建哈夫曼树
	char buff[100];
	char huffman_code[256][100] = {0};
	extract(arr[0], buff, huffman_code, 0); //将哈夫曼树节点和其路径存入huffman——code中

	Trie root = NULL;
	unsigned char str[MAX_LEN];
	int word_cnt = 0;
	int search_word_cnt = 0;
	FILE *fpp = NULL;
	fpp = fopen("/Users/zou-jianfeng/HZ/ZY/2.txt", "r"); //读取60M中文文本
	while (~fscanf(fpp, "%s", buffer)) {
		char TCN[MAX_LEN] = {0};
		for (int i = 0; buffer[i]; ++i) {
			word_cnt += 1;
			strcat(TCN, huffman_code[buffer[i]]); //获取输入字符串的哈夫曼路径
		}
		root = insert(root, TCN); //将路径插入二叉字典树中
	}
   	fclose(fpp);
	FILE *fppp = NULL;
	fppp = fopen("/Users/zou-jianfeng/HZ/ZY/2.txt", "r"); //读取60M中文文本
	while (~fscanf(fppp, "%s", buffer)) {
		char TCN[MAX_LEN] = {0};
		for (int i = 0; buffer[i]; ++i) {
			search_word_cnt += 1;
			strcat(TCN, huffman_code[buffer[i]]); //获取输入字符串的哈夫曼路径
		}
		if (search(root, TCN)) {
			printf("1");
		} else {
			printf("0");
			search_cnt += 1;
		}
	}
   	fclose(fppp);
	clear(root);
	printf("\n");
	printf("word_cnt = %d node_cnt = %d search_word_cnt = %d search_cnt = %d\n", word_cnt, node_cnt, search_word_cnt, search_cnt);
	printf("searchrate : %lf\n", 1.0 * search_word_cnt / (1.0 * search_cnt));
	printf("rate : %lf\n", 1.0 * word_cnt / (1.0 * node_cnt * sizeof(Node)));
	return 0;
}