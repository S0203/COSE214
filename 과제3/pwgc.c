#include <stdio.h>
#include <stdlib.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename( FILE *fp, int state);

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc( int state, int *p, int *w, int *g, int *c);

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end( int state);

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition( int state1,	int state2);

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP( int state);

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW( int state);

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG( int state);

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC( int state);

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited( int visited[], int level, int state);

// 방문한 상태들을 차례로 화면에 출력
static void print_states( int visited[], int count);

// recursive function
static void dfs_main( int state, int goal_state, int level, int visited[]);

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16]);

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num);

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num);

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int init_state, int goal_state)
{
	int level = 0;
	int visited[16] = {0,}; // 방문한 정점을 저장
	
	dfs_main( init_state, goal_state, level, visited); 
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0,};
	
	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력 (only for debugging)
	print_graph( graph, 16);
	
	// .net 파일 만들기
	save_graph( "pwgc.net", graph, 16);

	// 깊이 우선 탐색
	depth_first_search( 0, 15); // initial state, goal state
	
	return 0;
}


// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph(char* filename, int graph[][16], int num) {

	FILE* fp = fopen(filename, "w");
	fputs("*Vertices 16\n", fp);
	for (int i = 0; i < num; i++) {
		int temp = i + 1;
		fprintf(fp, "%d ", temp);
		print_statename(fp, i);
		fputs("\n", fp);
	}
	fputs("*Edges\n", fp);
	for (int i = 0; i < num; i++) {
		for (int j = i; j < num; j++) {
			int Newi = i + 1;
			int Newj = j + 1;
			if (graph[i][j] == 1) {
				fputs("  ", fp);
				fprintf(fp, "%d", Newi);
				fputs("  ", fp);
				fprintf(fp, "%d", Newj);
				fputs("\n", fp);
			}
		}
	}

	fclose(fp);
}

// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename(FILE* fp, int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	fprintf(fp, "\"<%d%d%d%d>\"", p, w, g, c);
}



// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc(int state, int* p, int* w, int* g, int* c) {
	if (state < 8) *p = 0;
	else *p = 1;
	if ((state % 8) < 4) *w = 0;
	else *w = 1;
	if ((state % 4) < 2) *g = 0;
	else *g = 1;
	if ((state % 2) < 1) *c = 0;
	else *c = 1;
}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end(int state) {
	if (state == -1) return 1;

	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p == 0) {
		if (w == 1 && g == 1) return 1;
		else if (g == 1 && c == 1) return 1;
		else return 0;
	}
	else if (p == 1) {
		if (w == 0 && g == 0) return 1;
		else if (g == 0 && c == 0) return 1;
		else return 0;
	}
}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
static int is_possible_transition(int state1, int state2) {
	if (is_dead_end(state1) == 1) return 0;
	else if (is_dead_end(state2) == 1) return 0;
	else if (changeP(state1) == state2) return 1;
	else if (changePC(state1) == state2) return 1;
	else if (changePG(state1) == state2) return 1;
	else if (changePW(state1) == state2) return 1;
	else return 0;
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p == 0) p = 1;
	else if (p == 1) p = 0;
	return (p * 8 + w * 4 + g * 2 + c * 1);
}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p != w) return -1;

	if (p == 0) p = 1;
	else if (p == 1) p = 0;
	if (w == 0) w = 1;
	else if (w == 1) w = 0;
	
	return (p * 8 + w * 4 + g * 2 + c * 1);
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p != g) return -1;

	if (p == 0) p = 1;
	else if (p == 1) p = 0;
	if (g == 0) g = 1;
	else if (g == 1) g = 0;
	
	return (p * 8 + w * 4 + g * 2 + c * 1);
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC(int state) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	if (p != c) return -1;

	if (p == 0) p = 1;
	else if (p == 1) p = 0;
	if (c == 0) c = 1;
	else if (c == 1) c = 0;
	
	return (p * 8 + w * 4 + g * 2 + c * 1);
}

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited(int visited[], int level, int state) {
	for (int i = 0; i < level; i++) {
		if (visited[i] == state) return 1;
	}
	return 0;
}

// 방문한 상태들을 차례로 화면에 출력
static void print_states(int visited[], int count) {
	int p, w, g, c;
	printf("Goal-state found!\n");
	for (int i = 0; i < count; i++) {
		get_pwgc(visited[i], &p, &w, &g, &c);
		printf("<%d%d%d%d>\n", p, w, g, c);
	}
	printf("\n");
}

// recursive function
static void dfs_main(int state, int goal_state, int level, int visited[]) {
	int p, w, g, c;
	get_pwgc(state, &p, &w, &g, &c);
	printf("cur state is <%d%d%d%d> (level %d)\n", p, w, g, c, level); //현재상태출력

	int newLevel = level + 1;

	if (state == goal_state) {
		print_states(visited, newLevel);
		return;
	}

	int changeState = changeP(state); //1. 농부만 이동하는 경우에 대해 생각
	if (changeState != -1) {
		if (is_dead_end(changeState)) { //허용되지 않는 상태인 경우
			get_pwgc(changeState, &p, &w, &g, &c);
			printf("\tnext state <%d%d%d%d> is dead-end\n", p, w, g, c);
		}
		else if (is_visited(visited, newLevel, changeState)) { //허용되지만 이미 방문한 상태인 경우
			get_pwgc(changeState, &p, &w, &g, &c);
			printf("\tnext state <%d%d%d%d> has been visited\n", p, w, g, c);
		}
		else { //허용되고, 방문하지 않은 상태인 경우
			visited[newLevel] = changeState;
			dfs_main(changeState, goal_state, newLevel, visited);
			get_pwgc(state, &p, &w, &g, &c);
			printf("back to <%d%d%d%d> (level %d)\n", p, w, g, c, level);
		}
	}
	

	changeState = changePW(state); //2. 농부와 늑대가 같이 이동하는 경우에 대해 생각
	if (changeState != -1) {
		if (is_dead_end(changeState)) { //허용되지 않는 상태인 경우
			get_pwgc(changeState, &p, &w, &g, &c);
			printf("\tnext state <%d%d%d%d> is dead-end\n", p, w, g, c);
		}
		else if (is_visited(visited, newLevel, changeState)) { //허용되지만 이미 방문한 상태인 경우
			get_pwgc(changeState, &p, &w, &g, &c);
			printf("\tnext state <%d%d%d%d> has been visited\n", p, w, g, c);
		}
		else { //허용되고, 방문하지 않은 상태인 경우
			visited[newLevel] = changeState;
			dfs_main(changeState, goal_state, newLevel, visited);
			get_pwgc(state, &p, &w, &g, &c);
			printf("back to <%d%d%d%d> (level %d)\n", p, w, g, c, level);
		}
	}
	

	changeState = changePG(state); //3. 농부와 염소가 같이 이동하는 경우에 대해 생각
	if (changeState != -1) {
		if (is_dead_end(changeState)) { //허용되지 않는 상태인 경우
			get_pwgc(changeState, &p, &w, &g, &c);
			printf("\tnext state <%d%d%d%d> is dead-end\n", p, w, g, c);
		}
		else if (is_visited(visited, newLevel, changeState)) { //허용되지만 이미 방문한 상태인 경우
			get_pwgc(changeState, &p, &w, &g, &c);
			printf("\tnext state <%d%d%d%d> has been visited\n", p, w, g, c);
		}
		else { //허용되고, 방문하지 않은 상태인 경우
			visited[newLevel] = changeState;
			dfs_main(changeState, goal_state, newLevel, visited);
			get_pwgc(state, &p, &w, &g, &c);
			printf("back to <%d%d%d%d> (level %d)\n", p, w, g, c, level);
		}
	}
	

	changeState = changePC(state); //4. 농부와 양배추가 같이 이동하는 경우에 대해 생각
	if (changeState != -1) {
		if (is_dead_end(changeState)) { //허용되지 않는 상태인 경우
			get_pwgc(changeState, &p, &w, &g, &c);
			printf("\tnext state <%d%d%d%d> is dead-end\n", p, w, g, c);
		}
		else if (is_visited(visited, newLevel, changeState)) { //허용되지만 이미 방문한 상태인 경우
			get_pwgc(changeState, &p, &w, &g, &c);
			printf("\tnext state <%d%d%d%d> has been visited\n", p, w, g, c);
		}
		else { //허용되고, 방문하지 않은 상태인 경우
			visited[newLevel] = changeState;
			dfs_main(changeState, goal_state, newLevel, visited);
			get_pwgc(state, &p, &w, &g, &c);
			printf("back to <%d%d%d%d> (level %d)\n", p, w, g, c, level);
		}
	}
	
}

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix(int graph[][16]) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (is_possible_transition(i, j)) graph[i][j] = 1;
		}
	}
}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph(int graph[][16], int num) {
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; j++) {
			printf("%d ", graph[i][j]);
		}
		printf("\n");
	}
}

