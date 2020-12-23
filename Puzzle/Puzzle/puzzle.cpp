//24 puzzle solution

# include <iostream>
# include <ctime>
# include <queue>
# include <vector>
# include <algorithm>
# include <cstdlib>
#define N 5

#pragma warning(disable:4996)

using namespace std;

typedef unsigned int ui;
typedef unsigned char uc;

bool isSolved = false;
int Size;
char mdTable[25][25];
char movableTable[25][4];
uc path[1024];
uc movedway[1024];
ui nodeCount;
char dirs[4] = { -1, 1, -2, 2 };
char moves[24][4];

struct NodeInfo {
	ui id;
	ui parent;
	char blank;
	char move;
};

struct Node {
	char board[25];
	char move;
	NodeInfo nodeInfo;
	uc f, g;

	Node(char* bd, char blk, char move, ui id, ui parent, uc f = 0, uc g = 0)
	{
		for (int i = 0; i < Size * Size; ++i)
		{
			board[i] = bd[i];
		}
		nodeInfo.blank = blk;
		nodeInfo.id = id;
		nodeInfo.parent = parent;
		nodeInfo.move = move;
		this->f = f;
		this->g = g;
	}

	bool operator >(const Node& other) const
	{
		return f > other.f;
	}
};

// inversion 횟수를 count
int getInvCount(int arr[])
{
	int inv_count = 0;
	for (int i = 0; i < N * N - 1; i++)
	{
		if (arr[i] == 25) {
			continue;
		}
		for (int j = i + 1; j < N * N; j++)
		{
			//if(arr[j])
			if (arr[j] && arr[i] && arr[i] > arr[j]) {
				inv_count++;
			}
		}
	}
	return inv_count;
}

int findXPosition(int puzzle[N][N])
{
	for (int i = N - 1; i >= 0; i--)
		for (int j = N - 1; j >= 0; j--)
			if (puzzle[i][j] == 25)
				return N - i;
}

bool isSolvable(int puzzle[N][N])
{
	int invCount = getInvCount((int*)puzzle);

	// puzzle size가 홀수일 때 
	// invCount가 짝수라면 true 반환
	if (N & 1) {
		return !(invCount & 1);
	}
	// puzzle size가 짝수일 때
	else
	{
		int pos = findXPosition(puzzle);
		if (pos & 1)
			return !(invCount & 1);
		else
			return invCount & 1;
	}
}

void MakeDirections()
{
	int i = 0;
	char dirs[] = { 0, 1, 2, 3 };
	do {
		moves[i][0] = dirs[0];
		moves[i][1] = dirs[1];
		moves[i][2] = dirs[2];
		moves[i++][3] = dirs[3];
	} while (next_permutation(dirs, dirs + 4));
}

void MakeMovableTable(int size)
{
	//이동방향(LRUD)
	int moves[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
	int board[5][5];

	//보드 위치 번호(0~24)
	for (int y = 0; y < size; ++y)
	{
		for (int x = 0; x < size; ++x)
		{
			board[y][x] = x + y * size;
		}
	}

	//25개의 위치에서 움직일 수 있는 방향(4개)의 자리
	//움직일 수 없는 곳은 -1
	int dx, dy, j;
	for (int y = 0; y < size; ++y)
	{
		for (int x = 0; x < size; ++x)
		{
			for (int i = 0; i < 4; ++i)
			{
				dx = moves[i][0];
				dy = moves[i][1];
				if (x + dx < 0 || y + dy < 0 || x + dx >= size || y + dy >= size)
				{
					//움직일 수 없는 방향
					j = -1;
				}
				else
				{
					//움직일 수 있는 빈칸의 인덱스
					j = board[y + dy][x + dx];
				}
				//i가 지정하는 것 = LRUD 방향
				movableTable[x + y * size][i] = j;
			}
		}
	}
}

void MakeMDTable(int size)
{
	//각 위치마다 거리 저장 -> mdTable
	for (int y = 0, i = 1; y < size * size; ++y, ++i)
	{
		i %= (size*size);
		for (int x = 0; x < size * size; ++x)
		{
			if (i == 0)
			{
				mdTable[i][x] = 0;
			}
			else
			{
				mdTable[i][x] = abs((y / size) - (x / size)) + abs((y % size) - (x % size));
			}
		}
	}
}

//어떤 줄에서 반드시 움직여야 하는 횟수 계산
char GetDistance(int *row, char md, int nums)
{
	if (nums > 1)
	{
		if (nums == 2)
		{
			if (row[0] > row[1])
				md += 2;
		}
		else if (nums == 3)
		{
			if (row[0] > row[1] || row[0] > row[2])
				md += 2;
			if (row[1] > row[2])
				md += 2;
		}
		else if (nums == 4)
		{
			if (row[0] > row[1] || row[0] > row[2] || row[0] > row[3])
				md += 2;
			if (row[1] > row[2] || row[1] > row[3])
				md += 2;
			if (row[2] > row[3])
				md += 2;
		}
		else if (nums == 5)
		{
			if (row[0] > row[1] || row[0] > row[2] || row[0] > row[3] || row[0] > row[4])
				md += 2;
			if (row[1] > row[2] || row[1] > row[3] || row[1] > row[4])
				md += 2;
			if (row[2] > row[3] || row[2] > row[4])
				md += 2;
			if (row[3] > row[4])
				md += 2;
		}
	}
	return md;
}

char GetHeuristic(char* puzzle)
{
	int i, j, x, md = 0;
	int k, n;
	int temp[5];

	//맨해튼 거리를 더함
	for (i = 0; i < Size * Size; ++i)
	{
		md += mdTable[puzzle[i] % (Size*Size)][i];
	}

	// row(가로) 방향에서 반드시 움직여야 하는 횟수를 더함
	for (i = 0, x = 1; i < Size; ++i, ++x)
	{
		k = 0;
		for (j = 0; j < Size; ++j)
		{
			n = puzzle[i * Size + j];
			if (n <= Size * x && n > Size * (x - 1))
			{
				temp[k++] = n;
			}
		}
		md = GetDistance(temp, md, k);
	}

	// column(세로) 방향에서 반드시 움직여야 하는 횟수를 더함
	for (i = 0, x = 1; i < Size; ++i, ++x)
	{
		k = 0;
		for (j = 0; j < Size; ++j)
		{
			n = puzzle[j * Size + i];
			if (n == x || n == x + Size || n == x + Size * 2 || n == x + Size * 3)
			{
				temp[k++] = n;
			}
		}
		md = GetDistance(temp, md, k);
	}
	return md;
}

char GetBlank(char *puzzle)
{
	for (int i = 0; i < Size * Size; ++i)
	{
		if (puzzle[i] == 25)
		{
			return i;
		}
	}
}

int i = 0;

//해당 노드에서 자식 노드들을 생성하여 queue에 push
void ExpandNode(priority_queue<Node, vector<Node>, greater<Node>> &open, const Node &nd)
{
	Node pNode = nd; //top
	char lastMove = pNode.nodeInfo.move;
	char blank = pNode.nodeInfo.blank;

	for (int move = 0; move < 4; ++move)
	{
		if (lastMove == -1 || (move + lastMove) % 4 != 1) //두번째 조건식이 지나온 자리 다시 안 가는 것 거름
		{
			//blank자리에서 move방향 검사
			//moveableTable: 25개의 자리, 4개의 방향
			char newBlank = movableTable[blank][move];
			if (newBlank == -1) continue;

			//가능한 방향마다 하나씩 바꿔봄
			pNode.board[blank] = pNode.board[newBlank];
			pNode.board[newBlank] = 25;

			nodeCount += 1;
			//각 경우마다 휴리스틱 구함
			uc h = GetHeuristic(pNode.board);
			uc g = pNode.g + 1;

			//노드생성
			Node node(pNode.board, newBlank, move, nodeCount, pNode.nodeInfo.id, g + h, g);
			//노드삽입 -> open은 움직일 수 있는 모든 방향의 노드를 저장함
			open.push(node);

			//원상 복귀
			pNode.board[newBlank] = pNode.board[blank];
			pNode.board[blank] = 25;
		}
	}
}

// vector에 저장된 노드들을 마지막부터 꺼내옴
// 선행자의 id와 현재 노드의 parent가 같다면 움직인 경로 movedway에 삽입
void MakePath(vector<NodeInfo> &vni, char* puzzle, uc depth)
{
	bool isFirst = true;
	NodeInfo temp, info;
	while (!vni.empty())
	{
		// vector의 가장 마지막 노드(즉, 정렬이 완료된 board를 가지는 노드)부터 꺼냄
		temp = vni.back();
		if (isFirst || temp.id == info.parent)
		{
			// 해당 board로 오기 위해 움직인 방향의 역을 구함
			// 홀수
			if (temp.move % 2) temp.move -= 1;
			// 짝수
			else temp.move += 1;
			char newBlank = movableTable[temp.blank][temp.move];

			//자리 바꿈
			puzzle[temp.blank] = puzzle[newBlank];
			puzzle[newBlank] = 25;
			path[--depth] = puzzle[temp.blank]; //누른것
			movedway[depth] = temp.move;

			info = temp;
			isFirst = false;
		}
		vni.pop_back();
	}
}

char AStar(char* puzzle)
{
	uc g = 0;
	uc h = GetHeuristic(puzzle);
	uc f = g + h;

	nodeCount = 0;
	ui parent = 0;
	// node 초기화
	// 루트노드의 id = 0 , parent = 0, g=0
	// 루트노드 = 초기 blank의 위치, 보드 등을 저장함
	// 입력된 판이므로 move = -1을 저장함(움직인 방향이 없음)
	Node node(puzzle, GetBlank(puzzle), -1, nodeCount, parent, f, g);
	vector<NodeInfo> closed;
	priority_queue<Node, vector<Node>, greater<Node>> open; //우선순위 큐(min_heap을 사용하여 f의 값이 최소인 노드들이 top에 있도록)

	// 입력된 판이 이미 정렬되어 있는 퍼즐판이므로 depth = 0을 반환
	if (h == 0) {
		return h;
	}

	// 루트 노드의 자식 노드를 생성하여 open 큐에 push
	ExpandNode(open, node);
	while (!open.empty())
	{
		// open큐는 f값이 작은 순으로 priority가 높음, top에 존재
		node = open.top();
		open.pop();

		// node.g = 해당 노드의 depth를 의미함
		// g = open에서 꺼낸 노드의 depth 중 가장 큰 값 
		// (현재 어느 레벨의 노드까지 꺼냈는지를 의미)
		// 꺼낸 노드가 현재 g값보다 크다면 g값을 갱신
		if (node.g > g) 	g = node.g;
		if (g - node.g > 12) continue;

		// solved일 때
		// f = h+g 이므로 f=g일 때, h=0 이므로 완성된 퍼즐이 리프에 나타난 경우
		if (node.f == node.g)
		{
			closed.push_back(node.nodeInfo);
			MakePath(closed, node.board, node.g);
			// 최종적으로 찾은 레벨, depth를 반환함
			// 이 값이 이동 경로 횟수
			return node.g;
		}

		// 꺼낸 노드의 레벨(node.g)가 현재까지 나온 노드의 레벨(g)보다
		// 11레벨 이하로 작은 노드라면
		// close vector에 node를 넣음
		// 12레벨 이상 차이나는 노드라면 넣지 않고 open큐에서 새로운 노드를 꺼냄
		closed.push_back(node.nodeInfo);
		// 저장한 노드의 자식 노드를 다시 생성
		ExpandNode(open, node);
	}
	// http://hochulshin.com/cpp-priority-queue/
	// https://en.cppreference.com/w/cpp/container/priority_queue}
}

void ASTAR(char* puzzle)
{
	uc depth = AStar(puzzle);
	printf("%d ", depth);

	for (int a = 0; a < depth; a++) {
		if (movedway[a] == 0) {
			printf("R ");
		}
		else if (movedway[a] == 1) {
			printf("L ");
		}
		else if (movedway[a] == 2) {
			printf("D ");
		}
		else if (movedway[a] == 3) {
			printf("U ");
		}
	}
	printf("\n");
}

int t; //입력받을 퍼즐의 개수

void puzzleSolution(void) {
	int i = 0, z = 0, k = 0;
	int size = N;
	char puzzle[20][25];
	int board[N][N];

	printf("puzzle size : 5*5, blank : 25\n");
	scanf("%d", &t);

	//t개의 퍼즐판을 입력받음
	printf("input puzzle data\n");
	while (z < t) {
		for (int i = 0; i < size * size; ++i)
		{
			scanf("%d", &puzzle[z][i]);
		}
		++z;
	}
	z = 0;

	//퍼즐 풀어서 결과 출력
	printf("\n====결과====\n");
	while (z < t) {
		MakeMDTable(size);
		MakeMovableTable(size);
		MakeDirections();

		Size = size;

		printf("#%d ", z + 1);

		for (int i = 0; i < size; ++i) {
			for (int k = 0; k < size; ++k) {
				board[i][k] = puzzle[z][size*i + k];
			}
		}

		isSolvable(board) ? cout << "" :
			cout << "-1\n";

		if (isSolvable(board) == false) {
			++z;
			continue;
		}

		isSolved = false;
		ASTAR(puzzle[z]);

		++z;
	}
}

int main()
{
	srand((ui)time(NULL));
	char dir = rand() % 24;

	puzzleSolution();

	return 0;
}
