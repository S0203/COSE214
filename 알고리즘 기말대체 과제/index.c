#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define oneGramNum 26 //���ĺ� ����
#define twoGramNum (26 * 26) //2-gram ���α� ����

typedef struct Node
{
	int charNum; //�ش� ���ڿ�(2-gram)�� ������ �ִ� �ܾ��� �ε��� ����
	int len; //�ܾ��� ���� ����
	struct	Node* next; //���� ��带 ����Ű�� ������
	
} tNode;

typedef struct
{			
	tNode* tNodeArr[twoGramNum]; //�ش� ���ڿ�(2-gram)�� ���� �ܾ���� �ε����� ������ ������ �迭
	int tNodeNum[twoGramNum]; //�ش� ���ڿ�(2-gram)�� ��� �ܾ �ش�Ǵ���

} WORDINDEX;

void makeIndexer(WORDINDEX* wordindex, char* str, int wordNum);

static int makeAlphabetNum(char a, char b);

static unsigned int makeNum(char temp);

static void insertIndex(WORDINDEX* wordindex, int index, int wordNum, int num);

void printIndex(WORDINDEX* wordindex, FILE* fp);

void indexDestroy(WORDINDEX* wordindex);


// argv[1] : �Է� �ؽ�Ʈ ����
// argv[2] : ��� �ؽ�Ʈ ����
int main(int argc, char** argv) {


	FILE* fp;
	char str[100];
	int wordNum = 0;
	WORDINDEX* wordindex; //word�� �ִ� �� �ܾ��� ���� ����
	wordindex = (WORDINDEX*)malloc(sizeof(WORDINDEX));
	memset(wordindex->tNodeNum, 0, sizeof(int) * twoGramNum);


	if (argc != 3)
	{
		fprintf(stderr, "%s input-file output-file\n", argv[0]);
		return 1;
	}

	fp = fopen(argv[1], "rt");
	if (fp == NULL)
	{
		fprintf(stderr, "File open error: %s\n", argv[1]);
		return 1;
	}


	while (fscanf(fp, "%s", str) != EOF)
	{
		makeIndexer(wordindex, str, wordNum);
		wordNum++;
		//if (wordNum = 100000) break; //valgrind�� 

	}

	printf("Total number of words: %d\n", wordNum);

	fclose(fp);

	fp = fopen(argv[2], "wt");

	printIndex(wordindex, fp);
	
	fclose(fp);

	indexDestroy(wordindex); //�޸� ����

	return 0;


}

void indexDestroy(WORDINDEX* wordindex) {
	
	tNode* delPtr;
	tNode* nextPtr;

	for (int i = 0; i < twoGramNum; i++) {
		if (wordindex->tNodeNum[i] == 0) {
			continue;
		}

		delPtr = wordindex->tNodeArr[i];
		nextPtr = delPtr->next;

		while (nextPtr != NULL) {
			free(delPtr);
			delPtr = nextPtr;
			nextPtr = delPtr->next;
		}

		free(delPtr);
	}

	free(wordindex);
}

void printIndex(WORDINDEX* wordindex, FILE* fp) {

	tNode* temp;

	for (int i = 0; i < twoGramNum; i++) {
		if (wordindex->tNodeNum[i] == 0) {
			continue;
		}

		temp = wordindex->tNodeArr[i];

		fprintf(fp, "%d", wordindex->tNodeNum[i]);
		fputs("\n", fp);

		for (int j = 0; j < wordindex->tNodeNum[i]; j++) {
			fprintf(fp, "%d\t%d", temp->charNum, temp->len);
			fputs("\n", fp);
			temp = temp->next;
		}		
	}

}


//wordindex�� 2-gram�� ��Ƶ� ����ü, str�� �о�� �ܾ�, wordNum�� �о�� �ܾ��� �ε���
void makeIndexer(WORDINDEX* wordindex, char* str, int wordNum) {

	int num = strlen(str); //�о�� ������ ���� ���ϱ�
	char a, b; //2 gram ���� �� ���
	unsigned int index;
	int arr[100] = { 0, };
	int k = 0;

	if (num == 1) return;

	for (int i = 0; i < num - 1; i++) {
		int check = 1;

		strncpy(&a, &str[i], 1);
		strncpy(&b, &str[i+1], 1);
		index = makeAlphabetNum(a, b); //2 gram �ε��� ��ȣ ���ϱ�

		for (int j = 0; j < k; j++) {
			if (arr[j] == index) { //�� �ܾ ������ 2 gram�� �ִ��� Ȯ���ϱ�
				check = 0;
				break;
			}
		}

		if (check) {
			arr[k] = index;
			k++;
			insertIndex(wordindex, index, wordNum, num);
		}
	}

	return;
}

static void insertIndex(WORDINDEX* wordindex, int index, int wordNum, int num) {
	tNode* insertNode = (tNode*)malloc(sizeof(tNode));
	insertNode->charNum = wordNum;
	insertNode->len = num;
	insertNode->next = NULL;

	if (wordindex->tNodeNum[index] == 0) {
		wordindex->tNodeArr[index] = insertNode;
	}
	else {
		tNode* temp = wordindex->tNodeArr[index];
		wordindex->tNodeArr[index] = insertNode;
		insertNode->next = temp;
	}
	wordindex->tNodeNum[index] = wordindex->tNodeNum[index] + 1;
	return;
}

//ù ��°(a)�� �� ��°(b) ���ĺ��� �޾Ƽ� ���° 2-gram �ε������� �˾Ƴ��� �Լ�
static int makeAlphabetNum(char a, char b) {
	
	unsigned int a1, b1, result;

	a1 = makeNum(a);
	b1 = makeNum(b);

	result = a1 * oneGramNum + b1;
	return result;

}

static unsigned int makeNum(char temp) {
	if (temp == 97) return 0;
	else if (temp == 98) return 1;
	else if (temp == 99) return 2;
	else if (temp == 100) return 3;
	else if (temp == 101) return 4;
	else if (temp == 102) return 5;
	else if (temp == 103) return 6;
	else if (temp == 104) return 7;
	else if (temp == 105) return 8;
	else if (temp == 106) return 9;
	else if (temp == 107) return 10;
	else if (temp == 108) return 11;
	else if (temp == 109) return 12;
	else if (temp == 110) return 13;
	else if (temp == 111) return 14;
	else if (temp == 112) return 15;
	else if (temp == 113) return 16;
	else if (temp == 114) return 17;
	else if (temp == 115) return 18;
	else if (temp == 116) return 19;
	else if (temp == 117) return 20;
	else if (temp == 118) return 21;
	else if (temp == 119) return 22;
	else if (temp == 120) return 23;
	else if (temp == 121) return 24;
	else if (temp == 122) return 25;
	else return -1;
}