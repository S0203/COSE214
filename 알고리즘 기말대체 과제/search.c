#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define oneGramNum 26 //알파벳 갯수
#define twoGramNum (26 * 26) //2-gram 색인기 갯수

#define INSERT_COST	1
#define DELETE_COST	1
#define SUBSTITUTE_COST	1
#define TRANSPOSE_COST	1

typedef struct Node
{
	int charNum; //해당 2-gram을 가지고 있는 단어의 인덱스 저장
	int count; //2gram이 몇 번 중복되는지 횟수 저장
	struct	Node* next; //다음 노드를 가리키는 포인터

} tNode;

typedef struct
{
	int total; //총 몇 개의 단어가 저장되어있는지 알아보기 위함 
	int max; //가장 많은 2gram을 가진 단어는 총 몇 개의 2gram을 가지고 있는지
	tNode* wordPtr; //입력받은 단어와 관련있는 문자 정보를 저장할 tNode 포인터

} WORDINFORMATION;


typedef struct Noden
{
	char wordname[100]; //단어를 저장할 공간
	int distance; //편집거리 저장할 공간
	struct	Noden* next; //다음 노드를 가리키는 포인터

} tName;

typedef struct 
{
	int total; //총 몇 개의 단어가 저장되어있는지 
	tName* wordPtr;

} WORDNAME;


void twogram(WORDINFORMATION* wordinformaion ,FILE* indexfp, char* word);

static int makeAlphabetNum(char a, char b);

static unsigned int makeNum(char temp);

void deleteIndex(WORDINFORMATION* wordinformation);


//디버깅을 위한 함수
void printIndex(WORDINFORMATION* wordinformation, FILE* fp);

void makeStr(WORDINFORMATION* wordinformation, WORDNAME* wordname, FILE* wordsfp, char* word);


//편집거리 관련 함수
void editdistance(WORDNAME* wordname, char* word);
static int min_editdistance(char* str1, char* str2);
static int __GetMin3(int a, int b, int c);
static int __GetMin4(int a, int b, int c, int d);

//메모리 해제
void destroy1(WORDINFORMATION* wordinformation);
void destroy2(WORDNAME* wordname);


// argv[1] : words.txt 파일
// argv[2] : index.txt 파일
int main(int argc, char** argv) {

	FILE* wordsfp, *indexfp;
	char word[100]; //입력받은 단어를 저장하기 위한 공간
	int length; //입력받은 단어의 길이를 저장하기 위한 공간

	//단어를 인덱스 정보로 저장
	WORDINFORMATION* wordinformation = (WORDINFORMATION*)malloc(sizeof(WORDINFORMATION)); 
	wordinformation->total = 0; //총 몇개의 단어가 저장되어있는지
	wordinformation->max = 0; //최대 중복 횟수
	wordinformation->wordPtr = NULL; 
	
	//단어를 문자열 형식으로 저장
	WORDNAME* wordname = (WORDNAME*)malloc(sizeof(WORDNAME));
	wordname->total = 0; //총 몇개의 단어가 저장되어있는지
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

		printf("Please enter more alphabets!\n"); //한 글자 입력 받았을 경우 예외처리
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
	
	
	twogram(wordinformation, indexfp, word); //index.txt에서 2-gram관련 정보 읽어온 다음 정리하는 함수

	fclose(indexfp); //index.txt는 이제 끝

	deleteIndex(wordinformation); //2gram 갯수가 최대치랑 같거나 한 개 적은 경우까지로 추림

	makeStr(wordinformation, wordname, wordsfp, word);

	fclose(wordsfp); //words.txt도 이제 끝

	editdistance(wordname, word);
	
	//메모리 두 개 해제
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
		delPtr = Ptr; //삭제할 노드가 현재 노드를 가리킴
		Ptr = Ptr->next; //현재 노드는 다음 노드가 됨
		delPtr->next = NULL; //삭제할 노드 초기화 중

		free(delPtr);//노드 삭제
	}

	free(wordname);//전체 구조체 삭제
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

	for (int i = 0; i < num; i++) { //단어의 숫자 정보의 총 갯수만큼 반복, 너무 많으면 그냥 150개로 자름
		fseek(wordsfp, 0, SEEK_SET); //단어 찾을 때마다 words.txt 처음부터 읽어와야 함
		wordIndex = wordPtr->charNum; //해당 단어의 인덱스를 저장

		for (int j = 0; j <= wordIndex; j++) { 
			fgets(str, 50, wordsfp); 
		}

		str[strlen(str) - 1] = '\0'; //개행문자 없애주기

		insertPtr = (tName*)malloc(sizeof(tName));
		strcpy(insertPtr->wordname, str);
		insertPtr->next = NULL;

		if (wordname->total == 0) { //처음 넣는 경우
			wordname->wordPtr = insertPtr;
		}
		else { //처음 넣는 게 아닌 경우
			endPtr->next = insertPtr;
		}

		(wordname->total)++; //총 저장된 문자형식 단어의 갯수
		endPtr = insertPtr; //저장할 문자 정보를 위한 포인터
		wordPtr = wordPtr->next; //다음 숫자 정보로 넘어감
	}

}

void deleteIndex(WORDINFORMATION* wordinformation) {

	int maxNum = wordinformation->max; //최대 중복 횟수
	if (maxNum < 3) return; //최대 중복 횟수가 최소 3은 되야 지울 수 있게 함
	tNode* delNode; 
	tNode* temp = wordinformation->wordPtr; //첫 포인터 가리키기, 현재 포인터
	tNode* preTemp = NULL; //이전 포인터

	while(temp != NULL) {
		if ((temp->count) < (maxNum - 1)) { //이 경우 지우기
			if (preTemp == NULL) { //첫 번째 원소를 지워야 하는 경우
				delNode = temp; //지우려는 노드가 현재 노드를 가리킴
				wordinformation->wordPtr = temp->next; //두 번째 노드를 첫 번째 노드로 함

				delNode->next = NULL;
				free(delNode); //노드 지우기

				temp = wordinformation->wordPtr; //현재노드는 다시 첫 노드를 가리킴
			}
			else { //첫 번째 이후의 원소를 지워야하는 경우
				delNode = temp; //지우려는 노드가 현재 노드를 가리킴
				preTemp->next = temp->next; //이전 노드의 다음 노드가 현재 노드의 다음 노드를 가리킴

				delNode->next = NULL;
				free(delNode); //삭제

				temp = preTemp->next; //현재노드는 이전 노드의 다음 노드를 가리킴
			}			
			(wordinformation->total)--; //노드를 지운 경우 총 갯수도 -1 갱신
		}
		else { //안지워도 되는 경우
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
	int num = strlen(word); //입력받은 단어의 길이
	char a, b;
	int index;
	char str[100]; //파일에서 읽어오기 위한 임시 문자열 공간
	char str2[10]; //파일에서 길이를 읽어오기 위한 임시 문자열 공간
	int bigNum, smallNum;
	int check;
	int maxcheck; //2gram이 한 번이라도 중복된 적 있는지 체크
	int maxNum = 1;
	int len;
	

	for (int i = 0; i < num - 1; i++) {
		fseek(indexfp, 0, SEEK_SET); //모든 2gram은 index.txt파일의 처음부터 읽어와야 함

		if ((i!=0) && (i%3 == 0)) { //3의 배수번째에 계속 노드를 지워나가면서 확인
			deleteIndex(wordinformaion);
		}

		maxcheck = 0; //노드의 카운트가 증가했는지를 체크함

		strncpy(&a, &word[i], 1);
		strncpy(&b, &word[i + 1], 1);
		printf("............loading about %c%c............\n", a, b);
		
		index = makeAlphabetNum(a, b); //2gram의 인덱스 알아내기

		for (int j = 0; j < index; j++) { //원하는 2gram에 도달할 때까지 계속해서 읽기만 하기
			fscanf(indexfp, "%s", str); //해당 2gram에 있는 단어의 갯수
			bigNum = atoi(str); //해당 2gram에 들어있는 문자의 갯수 숫자로 변환

			for (int k = 0; k < bigNum; k++) { //해당 2gram에 있는 단어의 갯수만큼 읽기
				fscanf(indexfp, "%s\t%s", str, str2);
			}
		}

		//이제 원하는 2gram에 도달
		fscanf(indexfp, "%s", str);
		bigNum = atoi(str); //원하는 2gram에 들어있는 문자의 갯수 알기

		if (num < 5) { //입력 받은 문자의 길이가 4개이하인 경우
			if (i == 0) { //입력 받은 문자에서 처음 2gram을 저장할 때에는 일단 다 저장하기
				for (int j = 0; j < bigNum; j++) { //해당 2gram에 있는 단어의 갯수만큼 반복
					fscanf(indexfp, "%s\t%s", str, str2);
					smallNum = atoi(str); //단어의 인덱스
					len = atoi(str2); //단어의 길이

					if (num + 1 < len || len + 1 < num) continue; //너무 길이차이가 많이(2이상) 나는 문자는 제외

					newNode = (tNode*)malloc(sizeof(tNode));
					newNode->charNum = smallNum; //새 노드에 단어의 인덱스(숫자정보) 저장
					newNode->count = 1; //새 노드에 단어의 2gram 중복 횟수 1로 저장
					newNode->next = NULL;

					if (wordinformaion->total == 0) { //첫 단어를 저장할 때
						wordinformaion->wordPtr = newNode;
					}
					else { //첫 단어 이후부터
						tempPtr = wordinformaion->wordPtr; //원래의 첫 단어를 가리킴
						wordinformaion->wordPtr = newNode; //새 노드를 첫 단어로 함
						newNode->next = tempPtr; //새 노드의 다음 노드(두 번째 노드)가 이전의 첫 단어가 되도록 함
					}
					(wordinformaion->total)++; //단어를 저장하고 난 뒤 단어의 총 갯수 +1 갱신
				}
				wordinformaion->max = 1;//각 단어는 일단 한 개의 2gram을 각각 가지고 있음
			}
			else {
				for (int j = 0; j < bigNum; j++) { //해당 2gram에 있는 단어의 갯수만큼 반복

					fscanf(indexfp, "%s\t%s", str, str2); //한 줄 읽기
					smallNum = atoi(str); //단어의 인덱스
					len = atoi(str2); //단어의 길이
					if (num + 1 < len || len + 1 < num) continue; //너무 길이차이가 많이 나는 문자는 제외

					check = 1; //노드를 추가해줘야하는 경우
					comparison = wordinformaion->wordPtr; //첫번째 포인터 가리키기 (비교할 포인터)
					preComparison = NULL; //(비교할 포인터의 이전 포인터)

					for (int k = 0; k < wordinformaion->total; k++) { //현재 저장되어있는 노드의 갯수만큼 반복하면서 중복 확인

						if ((comparison->charNum) == smallNum) { //중복되는 단어면 노드를 추가하는 게 아니라 노드의 count만 증가
							(comparison->count)++;
							check = 0; //노드의 카운트만 증가했다는 뜻(노드를 추가해줘야할지 판단할때 필요)
							maxcheck = 1;//노드의 카운트가 증가했다는 것 체크해놓기(wordinformation 의 max 값 때문에 필요)
							break; //뒤에 노드까지 살펴볼 필요 없으므로 반복문 탈출
						}
						else if ((comparison->charNum) > smallNum) { //비교대상의 글자가 더 뒤에 있는 글자이므로 더 이상 비교 안해봐도 됨
							//이 경우에는 중복되지 않으므로 check가 처음 설정대로 1이 됨. 즉, 노드를 추가해줘야함(i<2인 경우)
							break; //반복문 탈출
						}

						preComparison = comparison; //comparison의 전 문자 가리키고 있기
						comparison = comparison->next; //comparison은 다음 문자 가리키기
					}
				}
			}
		}
		else {
			if (i == 0) { //입력 받은 문자에서 처음 2gram을 저장할 때에는 일단 다 저장하기
				for (int j = 0; j < bigNum; j++) { //해당 2gram에 있는 단어의 갯수만큼 반복
					fscanf(indexfp, "%s\t%s", str, str2);
					smallNum = atoi(str); //단어의 인덱스
					len = atoi(str2); //단어의 길이

					if (num + 1 < len || len + 1 < num) continue; //너무 길이차이가 많이(2이상) 나는 문자는 제외

					newNode = (tNode*)malloc(sizeof(tNode));
					newNode->charNum = smallNum; //새 노드에 단어의 인덱스(숫자정보) 저장
					newNode->count = 1; //새 노드에 단어의 2gram 중복 횟수 1로 저장
					newNode->next = NULL;

					if (wordinformaion->total == 0) { //첫 단어를 저장할 때
						wordinformaion->wordPtr = newNode;
					}
					else { //첫 단어 이후부터
						tempPtr = wordinformaion->wordPtr; //원래의 첫 단어를 가리킴
						wordinformaion->wordPtr = newNode; //새 노드를 첫 단어로 함
						newNode->next = tempPtr; //새 노드의 다음 노드(두 번째 노드)가 이전의 첫 단어가 되도록 함
					}
					(wordinformaion->total)++; //단어를 저장하고 난 뒤 단어의 총 갯수 +1 갱신
				}
				wordinformaion->max = 1;//각 단어는 일단 한 개의 2gram을 각각 가지고 있음
			}
			else { //두 번째 2gram부터는 중복인지 아닌지 검사하면서 저장하기 
				for (int j = 0; j < bigNum; j++) { //해당 2gram에 있는 단어의 갯수만큼 반복

					fscanf(indexfp, "%s\t%s", str, str2); //한 줄 읽기
					smallNum = atoi(str); //단어의 인덱스
					len = atoi(str2); //단어의 길이
					if (num + 1 < len || len + 1 < num) continue; //너무 길이차이가 많이 나는 문자는 제외

					check = 1; //노드를 추가해줘야하는 경우
					comparison = wordinformaion->wordPtr; //첫번째 포인터 가리키기 (비교할 포인터)
					preComparison = NULL; //(비교할 포인터의 이전 포인터)

					for (int k = 0; k < wordinformaion->total; k++) { //현재 저장되어있는 노드의 갯수만큼 반복하면서 중복 확인

						if ((comparison->charNum) == smallNum) { //중복되는 단어면 노드를 추가하는 게 아니라 노드의 count만 증가
							(comparison->count)++;
							check = 0; //노드의 카운트만 증가했다는 뜻(노드를 추가해줘야할지 판단할때 필요)
							maxcheck = 1;//노드의 카운트가 증가했다는 것 체크해놓기(wordinformation 의 max 값 때문에 필요)
							break; //뒤에 노드까지 살펴볼 필요 없으므로 반복문 탈출
						}
						else if ((comparison->charNum) > smallNum) { //비교대상의 글자가 더 뒤에 있는 글자이므로 더 이상 비교 안해봐도 됨
							//이 경우에는 중복되지 않으므로 check가 처음 설정대로 1이 됨. 즉, 노드를 추가해줘야함(i<2인 경우)
							break; //반복문 탈출
						}

						preComparison = comparison; //comparison의 전 문자 가리키고 있기
						comparison = comparison->next; //comparison은 다음 문자 가리키기
					}

					if (check && i < 2) { //중복검사 끝나고 노드를 추가해야 하는 경우 (두번째 2gram까지만 노드 저장)

						newNode = (tNode*)malloc(sizeof(tNode));
						newNode->charNum = smallNum;
						newNode->count = 1;
						newNode->next = NULL;

						if (preComparison == NULL) { //맨 처음에 노드를 삽입하는 경우
							tempPtr = wordinformaion->wordPtr; //원래의 첫 단어를 가리킴
							wordinformaion->wordPtr = newNode; //새 노드를 첫 단어로 함
							newNode->next = tempPtr; //새 노드의 다음 노드(두 번째 노드)가 이전의 첫 단어가 되도록 함
						}
						else if (preComparison->next == NULL) { //맨 끝에 노드를 삽입하는 경우
							preComparison->next = newNode;
						}
						else { //중간에 노드를 삽입하는 경우
							tempPtr = preComparison->next;
							preComparison->next = newNode;
							newNode->next = tempPtr;
						}

						(wordinformaion->total)++;
					}

				}
			}
		}
		
		
		if (maxcheck == 1) { //노드의 카운트가 증가한 적이 있으면 maxNum +1 갱신
			maxNum++;
		}
		wordinformaion->max = maxNum; //최대 중복 2gram 수 갱신
	}
}

//첫 번째(a)와 두 번째(b) 알파벳을 받아서 몇번째 2-gram 인덱스인지 알아내는 함수
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


////////////편집거리 관련 함수//////////////
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
	while (printNum < 10) { //총 10개까지 출력
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

	//초기화
	d[0][0] = 0;
	for (i = 1; i < n + 1; i++) {
		d[i][0] = i;
	}
	for (j = 1; j < m + 1; j++) {
		d[0][j] = j;
	}

	int M, S, I, D, T;
	int notMove;
	int minValue; //최솟값 저장

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

			d[i][j] = minValue; //최소편집거리 저장

		}
	}//행렬 완성

	return d[n][m];
}

// 세 정수 중에서 가장 작은 값을 리턴한다.
static int __GetMin3(int a, int b, int c)
{
	int min = a;
	if (b < min)
		min = b;
	if (c < min)
		min = c;
	return min;
}

// 네 정수 중에서 가장 작은 값을 리턴한다.
static int __GetMin4(int a, int b, int c, int d)
{
	int min = __GetMin3(a, b, c);
	return (min > d) ? d : min;
}



//////////////디버깅용/////////////////////안씀///////////////////////////
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