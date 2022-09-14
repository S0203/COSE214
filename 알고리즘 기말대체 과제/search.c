#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define oneGramNum 26 //���ĺ� ����
#define twoGramNum (26 * 26) //2-gram ���α� ����

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1

typedef struct Node
{
	int charNum; //�ش� 2-gram�� ������ �ִ� �ܾ��� �ε��� ����
	int count; //2gram�� �� �� �ߺ��Ǵ��� Ƚ�� ����
	struct	Node* next; //���� ��带 ����Ű�� ������

} tNode;

typedef struct
{
	int total; //�� �� ���� �ܾ ����Ǿ��ִ��� �˾ƺ��� ���� 
	int max; //���� ���� 2gram�� ���� �ܾ�� �� �� ���� 2gram�� ������ �ִ���
	tNode* wordPtr; //�Է¹��� �ܾ�� �����ִ� ���� ������ ������ tNode ������

} WORDINFORMATION;


typedef struct Noden
{
	char wordname[100]; //�ܾ ������ ����
	int distance; //�����Ÿ� ������ ����
	struct	Noden* next; //���� ��带 ����Ű�� ������

} tName;

typedef struct 
{
	int total; //�� �� ���� �ܾ ����Ǿ��ִ��� 
	tName* wordPtr;

} WORDNAME;


void twogram(WORDINFORMATION* wordinformaion ,FILE* indexfp, char* word);

static int makeAlphabetNum(char a, char b);

static unsigned int makeNum(char temp);

void deleteIndex(WORDINFORMATION* wordinformation);


//������� ���� �Լ�
void printIndex(WORDINFORMATION* wordinformation, FILE* fp);

void makeStr(WORDINFORMATION* wordinformation, WORDNAME* wordname, FILE* wordsfp, char* word);


//�����Ÿ� ���� �Լ�
void editdistance(WORDNAME* wordname, char* word);
static int min_editdistance(char* str1, char* str2);
static int __GetMin3(int a, int b, int c);
static int __GetMin4(int a, int b, int c, int d);

//�޸� ����
void destroy1(WORDINFORMATION* wordinformation);
void destroy2(WORDNAME* wordname);


// argv[1] : words.txt ����
// argv[2] : index.txt ����
int main(int argc, char** argv) {

	FILE* wordsfp, *indexfp;
	char word[100]; //�Է¹��� �ܾ �����ϱ� ���� ����
	int length; //�Է¹��� �ܾ��� ���̸� �����ϱ� ���� ����

	//�ܾ �ε��� ������ ����
	WORDINFORMATION* wordinformation = (WORDINFORMATION*)malloc(sizeof(WORDINFORMATION)); 
	wordinformation->total = 0; //�� ��� �ܾ ����Ǿ��ִ���
	wordinformation->max = 0; //�ִ� �ߺ� Ƚ��
	wordinformation->wordPtr = NULL; 
	
	//�ܾ ���ڿ� �������� ����
	WORDNAME* wordname = (WORDNAME*)malloc(sizeof(WORDNAME));
	wordname->total = 0; //�� ��� �ܾ ����Ǿ��ִ���
	wordname->wordPtr = NULL;

	if (argc != 3)
	{
		fprintf(stderr, "%s input-file-01 encoded-file-02\n", argv[0]);
		return 1;
	} 

	printf("Please enter a word!\n");

	while(1){
		
		scanf("%s", word);
		length = strlen(word);

		if (length > 1) break;

		printf("Please enter more alphabets!\n"); //�� ���� �Է� �޾��� ��� ����ó��
	} 
	
	wordsfp = fopen(argv[1], "rt");
	if (wordsfp == NULL)
	{
		fprintf(stderr, "File open error: %s\n", argv[1]);
		return 1;
	}

	indexfp = fopen(argv[2], "rt");
	if (indexfp == NULL)
	{
		fprintf(stderr, "File open error: %s\n", argv[2]);
		return 1;
	}
	
	
	twogram(wordinformation, indexfp, word); //index.txt���� 2-gram���� ���� �о�� ���� �����ϴ� �Լ�

	fclose(indexfp); //index.txt�� ���� ��

	deleteIndex(wordinformation); //2gram ������ �ִ�ġ�� ���ų� �� �� ���� �������� �߸�

	makeStr(wordinformation, wordname, wordsfp, word);

	fclose(wordsfp); //words.txt�� ���� ��

	editdistance(wordname, word);
	
	//�޸� �� �� ����
	destroy1(wordinformation);
	destroy2(wordname);

}

void destroy1(WORDINFORMATION* wordinformation) {
	int total = wordinformation->total;
	tNode* delPtr;
	tNode* Ptr = wordinformation->wordPtr;

	for (int i = 0; i < total; i++) {
		delPtr = Ptr;
		Ptr = Ptr->next;
		delPtr->next = NULL;

		free(delPtr);
	}

	free(wordinformation);
}
void destroy2(WORDNAME* wordname) {
	int total = wordname->total;
	tName* delPtr;
	tName* Ptr = wordname->wordPtr;

	for (int i = 0; i < total; i++) {
		delPtr = Ptr; //������ ��尡 ���� ��带 ����Ŵ
		Ptr = Ptr->next; //���� ���� ���� ��尡 ��
		delPtr->next = NULL; //������ ��� �ʱ�ȭ ��

		free(delPtr);//��� ����
	}

	free(wordname);//��ü ����ü ����
}

void makeStr(WORDINFORMATION* wordinformation, WORDNAME* wordname, FILE* wordsfp, char* word) {

	char str[50];
	tNode* wordPtr = wordinformation->wordPtr;
	int wordIndex;
	tName* insertPtr; 
	tName* endPtr = NULL;
	int num;

	if (wordinformation->total > 200) {
		num = 150;
	}
	else {
		num = wordinformation->total;
	}

	printf("............%d processing............\n", num);

	for (int i = 0; i < num; i++) { //�ܾ��� ���� ������ �� ������ŭ �ݺ�, �ʹ� ������ �׳� 150���� �ڸ�
		fseek(wordsfp, 0, SEEK_SET); //�ܾ� ã�� ������ words.txt ó������ �о�;� ��
		wordIndex = wordPtr->charNum; //�ش� �ܾ��� �ε����� ����

		for (int j = 0; j <= wordIndex; j++) { 
			fgets(str, 50, wordsfp); 
		}

		str[strlen(str) - 1] = '\0'; //���๮�� �����ֱ�

		insertPtr = (tName*)malloc(sizeof(tName));
		strcpy(insertPtr->wordname, str);
		insertPtr->next = NULL;

		if (wordname->total == 0) { //ó�� �ִ� ���
			wordname->wordPtr = insertPtr;
		}
		else { //ó�� �ִ� �� �ƴ� ���
			endPtr->next = insertPtr;
		}

		(wordname->total)++; //�� ����� �������� �ܾ��� ����
		endPtr = insertPtr; //������ ���� ������ ���� ������
		wordPtr = wordPtr->next; //���� ���� ������ �Ѿ
	}

}

void deleteIndex(WORDINFORMATION* wordinformation) {

	int maxNum = wordinformation->max; //�ִ� �ߺ� Ƚ��
	if (maxNum < 3) return; //�ִ� �ߺ� Ƚ���� �ּ� 3�� �Ǿ� ���� �� �ְ� ��
	tNode* delNode; 
	tNode* temp = wordinformation->wordPtr; //ù ������ ����Ű��, ���� ������
	tNode* preTemp = NULL; //���� ������

	while(temp != NULL) {
		if ((temp->count) < (maxNum - 1)) { //�� ��� �����
			if (preTemp == NULL) { //ù ��° ���Ҹ� ������ �ϴ� ���
				delNode = temp; //������� ��尡 ���� ��带 ����Ŵ
				wordinformation->wordPtr = temp->next; //�� ��° ��带 ù ��° ���� ��

				delNode->next = NULL;
				free(delNode); //��� �����

				temp = wordinformation->wordPtr; //������� �ٽ� ù ��带 ����Ŵ
			}
			else { //ù ��° ������ ���Ҹ� �������ϴ� ���
				delNode = temp; //������� ��尡 ���� ��带 ����Ŵ
				preTemp->next = temp->next; //���� ����� ���� ��尡 ���� ����� ���� ��带 ����Ŵ

				delNode->next = NULL;
				free(delNode); //����

				temp = preTemp->next; //������� ���� ����� ���� ��带 ����Ŵ
			}			
			(wordinformation->total)--; //��带 ���� ��� �� ������ -1 ����
		}
		else { //�������� �Ǵ� ���
			preTemp = temp;
			temp = temp->next;
		}
	}
}

void twogram(WORDINFORMATION* wordinformaion, FILE* indexfp, char* word) {

	tNode* newNode;
	tNode* tempPtr;
	tNode* comparison;
	tNode* preComparison;
	int num = strlen(word); //�Է¹��� �ܾ��� ����
	char a, b;
	int index;
	char str[100]; //���Ͽ��� �о���� ���� �ӽ� ���ڿ� ����
	char str2[10]; //���Ͽ��� ���̸� �о���� ���� �ӽ� ���ڿ� ����
	int bigNum, smallNum;
	int check;
	int maxcheck; //2gram�� �� ���̶� �ߺ��� �� �ִ��� üũ
	int maxNum = 1;
	int len;
	

	for (int i = 0; i < num - 1; i++) {
		fseek(indexfp, 0, SEEK_SET); //��� 2gram�� index.txt������ ó������ �о�;� ��

		if ((i!=0) && (i%3 == 0)) { //3�� �����°�� ��� ��带 ���������鼭 Ȯ��
			deleteIndex(wordinformaion);
		}

		maxcheck = 0; //����� ī��Ʈ�� �����ߴ����� üũ��

		strncpy(&a, &word[i], 1);
		strncpy(&b, &word[i + 1], 1);
		printf("............loading about %c%c............\n", a, b);
		
		index = makeAlphabetNum(a, b); //2gram�� �ε��� �˾Ƴ���

		for (int j = 0; j < index; j++) { //���ϴ� 2gram�� ������ ������ ����ؼ� �б⸸ �ϱ�
			fscanf(indexfp, "%s", str); //�ش� 2gram�� �ִ� �ܾ��� ����
			bigNum = atoi(str); //�ش� 2gram�� ����ִ� ������ ���� ���ڷ� ��ȯ

			for (int k = 0; k < bigNum; k++) { //�ش� 2gram�� �ִ� �ܾ��� ������ŭ �б�
				fscanf(indexfp, "%s\t%s", str, str2);
			}
		}

		//���� ���ϴ� 2gram�� ����
		fscanf(indexfp, "%s", str);
		bigNum = atoi(str); //���ϴ� 2gram�� ����ִ� ������ ���� �˱�

		if (num < 5) { //�Է� ���� ������ ���̰� 4�������� ���
			if (i == 0) { //�Է� ���� ���ڿ��� ó�� 2gram�� ������ ������ �ϴ� �� �����ϱ�
				for (int j = 0; j < bigNum; j++) { //�ش� 2gram�� �ִ� �ܾ��� ������ŭ �ݺ�
					fscanf(indexfp, "%s\t%s", str, str2);
					smallNum = atoi(str); //�ܾ��� �ε���
					len = atoi(str2); //�ܾ��� ����

					if (num + 1 < len || len + 1 < num) continue; //�ʹ� �������̰� ����(2�̻�) ���� ���ڴ� ����

					newNode = (tNode*)malloc(sizeof(tNode));
					newNode->charNum = smallNum; //�� ��忡 �ܾ��� �ε���(��������) ����
					newNode->count = 1; //�� ��忡 �ܾ��� 2gram �ߺ� Ƚ�� 1�� ����
					newNode->next = NULL;

					if (wordinformaion->total == 0) { //ù �ܾ ������ ��
						wordinformaion->wordPtr = newNode;
					}
					else { //ù �ܾ� ���ĺ���
						tempPtr = wordinformaion->wordPtr; //������ ù �ܾ ����Ŵ
						wordinformaion->wordPtr = newNode; //�� ��带 ù �ܾ�� ��
						newNode->next = tempPtr; //�� ����� ���� ���(�� ��° ���)�� ������ ù �ܾ �ǵ��� ��
					}
					(wordinformaion->total)++; //�ܾ �����ϰ� �� �� �ܾ��� �� ���� +1 ����
				}
				wordinformaion->max = 1;//�� �ܾ�� �ϴ� �� ���� 2gram�� ���� ������ ����
			}
			else {
				for (int j = 0; j < bigNum; j++) { //�ش� 2gram�� �ִ� �ܾ��� ������ŭ �ݺ�

					fscanf(indexfp, "%s\t%s", str, str2); //�� �� �б�
					smallNum = atoi(str); //�ܾ��� �ε���
					len = atoi(str2); //�ܾ��� ����
					if (num + 1 < len || len + 1 < num) continue; //�ʹ� �������̰� ���� ���� ���ڴ� ����

					check = 1; //��带 �߰�������ϴ� ���
					comparison = wordinformaion->wordPtr; //ù��° ������ ����Ű�� (���� ������)
					preComparison = NULL; //(���� �������� ���� ������)

					for (int k = 0; k < wordinformaion->total; k++) { //���� ����Ǿ��ִ� ����� ������ŭ �ݺ��ϸ鼭 �ߺ� Ȯ��

						if ((comparison->charNum) == smallNum) { //�ߺ��Ǵ� �ܾ�� ��带 �߰��ϴ� �� �ƴ϶� ����� count�� ����
							(comparison->count)++;
							check = 0; //����� ī��Ʈ�� �����ߴٴ� ��(��带 �߰���������� �Ǵ��Ҷ� �ʿ�)
							maxcheck = 1;//����� ī��Ʈ�� �����ߴٴ� �� üũ�س���(wordinformation �� max �� ������ �ʿ�)
							break; //�ڿ� ������ ���캼 �ʿ� �����Ƿ� �ݺ��� Ż��
						}
						else if ((comparison->charNum) > smallNum) { //�񱳴���� ���ڰ� �� �ڿ� �ִ� �����̹Ƿ� �� �̻� �� ���غ��� ��
							//�� ��쿡�� �ߺ����� �����Ƿ� check�� ó�� ������� 1�� ��. ��, ��带 �߰��������(i<2�� ���)
							break; //�ݺ��� Ż��
						}

						preComparison = comparison; //comparison�� �� ���� ����Ű�� �ֱ�
						comparison = comparison->next; //comparison�� ���� ���� ����Ű��
					}
				}
			}
		}
		else {
			if (i == 0) { //�Է� ���� ���ڿ��� ó�� 2gram�� ������ ������ �ϴ� �� �����ϱ�
				for (int j = 0; j < bigNum; j++) { //�ش� 2gram�� �ִ� �ܾ��� ������ŭ �ݺ�
					fscanf(indexfp, "%s\t%s", str, str2);
					smallNum = atoi(str); //�ܾ��� �ε���
					len = atoi(str2); //�ܾ��� ����

					if (num + 1 < len || len + 1 < num) continue; //�ʹ� �������̰� ����(2�̻�) ���� ���ڴ� ����

					newNode = (tNode*)malloc(sizeof(tNode));
					newNode->charNum = smallNum; //�� ��忡 �ܾ��� �ε���(��������) ����
					newNode->count = 1; //�� ��忡 �ܾ��� 2gram �ߺ� Ƚ�� 1�� ����
					newNode->next = NULL;

					if (wordinformaion->total == 0) { //ù �ܾ ������ ��
						wordinformaion->wordPtr = newNode;
					}
					else { //ù �ܾ� ���ĺ���
						tempPtr = wordinformaion->wordPtr; //������ ù �ܾ ����Ŵ
						wordinformaion->wordPtr = newNode; //�� ��带 ù �ܾ�� ��
						newNode->next = tempPtr; //�� ����� ���� ���(�� ��° ���)�� ������ ù �ܾ �ǵ��� ��
					}
					(wordinformaion->total)++; //�ܾ �����ϰ� �� �� �ܾ��� �� ���� +1 ����
				}
				wordinformaion->max = 1;//�� �ܾ�� �ϴ� �� ���� 2gram�� ���� ������ ����
			}
			else { //�� ��° 2gram���ʹ� �ߺ����� �ƴ��� �˻��ϸ鼭 �����ϱ� 
				for (int j = 0; j < bigNum; j++) { //�ش� 2gram�� �ִ� �ܾ��� ������ŭ �ݺ�

					fscanf(indexfp, "%s\t%s", str, str2); //�� �� �б�
					smallNum = atoi(str); //�ܾ��� �ε���
					len = atoi(str2); //�ܾ��� ����
					if (num + 1 < len || len + 1 < num) continue; //�ʹ� �������̰� ���� ���� ���ڴ� ����

					check = 1; //��带 �߰�������ϴ� ���
					comparison = wordinformaion->wordPtr; //ù��° ������ ����Ű�� (���� ������)
					preComparison = NULL; //(���� �������� ���� ������)

					for (int k = 0; k < wordinformaion->total; k++) { //���� ����Ǿ��ִ� ����� ������ŭ �ݺ��ϸ鼭 �ߺ� Ȯ��

						if ((comparison->charNum) == smallNum) { //�ߺ��Ǵ� �ܾ�� ��带 �߰��ϴ� �� �ƴ϶� ����� count�� ����
							(comparison->count)++;
							check = 0; //����� ī��Ʈ�� �����ߴٴ� ��(��带 �߰���������� �Ǵ��Ҷ� �ʿ�)
							maxcheck = 1;//����� ī��Ʈ�� �����ߴٴ� �� üũ�س���(wordinformation �� max �� ������ �ʿ�)
							break; //�ڿ� ������ ���캼 �ʿ� �����Ƿ� �ݺ��� Ż��
						}
						else if ((comparison->charNum) > smallNum) { //�񱳴���� ���ڰ� �� �ڿ� �ִ� �����̹Ƿ� �� �̻� �� ���غ��� ��
							//�� ��쿡�� �ߺ����� �����Ƿ� check�� ó�� ������� 1�� ��. ��, ��带 �߰��������(i<2�� ���)
							break; //�ݺ��� Ż��
						}

						preComparison = comparison; //comparison�� �� ���� ����Ű�� �ֱ�
						comparison = comparison->next; //comparison�� ���� ���� ����Ű��
					}

					if (check && i < 2) { //�ߺ��˻� ������ ��带 �߰��ؾ� �ϴ� ��� (�ι�° 2gram������ ��� ����)

						newNode = (tNode*)malloc(sizeof(tNode));
						newNode->charNum = smallNum;
						newNode->count = 1;
						newNode->next = NULL;

						if (preComparison == NULL) { //�� ó���� ��带 �����ϴ� ���
							tempPtr = wordinformaion->wordPtr; //������ ù �ܾ ����Ŵ
							wordinformaion->wordPtr = newNode; //�� ��带 ù �ܾ�� ��
							newNode->next = tempPtr; //�� ����� ���� ���(�� ��° ���)�� ������ ù �ܾ �ǵ��� ��
						}
						else if (preComparison->next == NULL) { //�� ���� ��带 �����ϴ� ���
							preComparison->next = newNode;
						}
						else { //�߰��� ��带 �����ϴ� ���
							tempPtr = preComparison->next;
							preComparison->next = newNode;
							newNode->next = tempPtr;
						}

						(wordinformaion->total)++;
					}

				}
			}
		}
		
		
		if (maxcheck == 1) { //����� ī��Ʈ�� ������ ���� ������ maxNum +1 ����
			maxNum++;
		}
		wordinformaion->max = maxNum; //�ִ� �ߺ� 2gram �� ����
	}
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


////////////�����Ÿ� ���� �Լ�//////////////
void editdistance(WORDNAME* wordname, char* word) {
	char temp[100];
	tName* Ptr = wordname->wordPtr;
	int distance;
	int mindistance = 100;

	for (int i = 0; i < wordname->total; i++) {
		strcpy(temp, Ptr->wordname);
		distance = min_editdistance(temp, word);
		Ptr->distance = distance;

		if (mindistance > distance) mindistance = distance;

		Ptr = Ptr->next;
	}


	int printNum = 0;
	printf("Did you find this word?\n");
	int num = wordname->total;
	while (printNum < 10) { //�� 10������ ���
		Ptr = wordname->wordPtr;
		for (int i = 0; i < wordname->total; i++) {
			if (mindistance == Ptr->distance) {
				printf("%d. %s\t%d\n", printNum + 1, Ptr->wordname, Ptr->distance);
				printNum++;
				if (printNum == 10) return;
				if (num == printNum) {
					printf("done!\n");
					return;
				}
			}
			Ptr = Ptr->next;
		}

		mindistance++;
	}

}

static int min_editdistance(char* str1, char* str2) {
	int n = strlen(str1);
	int m = strlen(str2);

	int i, j;
	int d[n + 1][m + 1];
	int EditDistance = 0;

	//�ʱ�ȭ
	d[0][0] = 0;
	for (i = 1; i < n + 1; i++) {
		d[i][0] = i;
	}
	for (j = 1; j < m + 1; j++) {
		d[0][j] = j;
	}

	int M, S, I, D, T;
	int notMove;
	int minValue; //�ּڰ� ����

	for (i = 1; i < n + 1; i++) {
		for (j = 1; j < m + 1; j++) {

			I = d[i][j - 1] + INSERT_COST;
			D = d[i - 1][j] + DELETE_COST;
			if (strncmp(&str1[i - 1], &str2[j - 1], 1) == 0) {
				notMove = 1;
				M = d[i - 1][j - 1];
			}
			else {
				notMove = 0;
				S = d[i - 1][j - 1] + SUBSTITUTE_COST;
			}

			if (i == 1 || j == 1) {
				if (notMove) {
					minValue = __GetMin3(M, D, I);
				}
				else {
					minValue = __GetMin3(S, D, I);
				}
			}
			else {
				if ((strncmp(&str1[i - 1 - 1], &str2[j - 1], 1) == 0) && (strncmp(&str1[i - 1], &str2[j - 1 - 1], 1) == 0)) {
					T = d[i - 2][j - 2] + TRANSPOSE_COST;
				}
				else T = 1000;

				if (notMove) {
					minValue = __GetMin4(T, M, D, I);
				}
				else {
					minValue = __GetMin4(T, S, D, I);
				}

			}

			d[i][j] = minValue; //�ּ������Ÿ� ����

		}
	}//��� �ϼ�

	return d[n][m];
}

// �� ���� �߿��� ���� ���� ���� �����Ѵ�.
static int __GetMin3(int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if (c < min)
		min = c;
	return min;
}

// �� ���� �߿��� ���� ���� ���� �����Ѵ�.
static int __GetMin4(int a, int b, int c, int d)
{
	int min = __GetMin3(a, b, c);
	return (min > d) ? d : min;
}



//////////////������/////////////////////�Ⱦ�///////////////////////////
void printIndex(WORDINFORMATION* wordinformation, FILE* fp) {

	tNode* temp;
	temp = wordinformation->wordPtr;

	fprintf(fp, "%d, %d", wordinformation->total, wordinformation->max);
	fputs("\n\n", fp);

	for (int i = 0; i < wordinformation->total; i++) {


		fprintf(fp, "%d, %d", temp->charNum, temp->count);
		fputs("\n", fp);

		temp = temp->next;


	}

}