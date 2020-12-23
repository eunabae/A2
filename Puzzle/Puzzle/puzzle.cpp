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

// inversion Ƚ���� count
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

	// puzzle size�� Ȧ���� �� 
	// invCount�� ¦����� true ��ȯ
	if (N & 1) {
		return !(invCount & 1);
	}
	// puzzle size�� ¦���� ��
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
	//�̵�����(LRUD)
	int moves[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
	int board[5][5];

	//���� ��ġ ��ȣ(0~24)
	for (int y = 0; y < size; ++y)
	{
		for (int x = 0; x < size; ++x)
		{
			board[y][x] = x + y * size;
		}
	}

	//25���� ��ġ���� ������ �� �ִ� ����(4��)�� �ڸ�
	//������ �� ���� ���� -1
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
					//������ �� ���� ����
					j = -1;
				}
				else
				{
					//������ �� �ִ� ��ĭ�� �ε���
					j = board[y + dy][x + dx];
				}
				//i�� �����ϴ� �� = LRUD ����
				movableTable[x + y * size][i] = j;
			}
		}
	}
}

void MakeMDTable(int size)
{
	//�� ��ġ���� �Ÿ� ���� -> mdTable
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

//� �ٿ��� �ݵ�� �������� �ϴ� Ƚ�� ���
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

	//����ư �Ÿ��� ����
	for (i = 0; i < Size * Size; ++i)
	{
		md += mdTable[puzzle[i] % (Size*Size)][i];
	}

	// row(����) ���⿡�� �ݵ�� �������� �ϴ� Ƚ���� ����
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

	// column(����) ���⿡�� �ݵ�� �������� �ϴ� Ƚ���� ����
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

//�ش� ��忡�� �ڽ� ������ �����Ͽ� queue�� push
void ExpandNode(priority_queue<Node, vector<Node>, greater<Node>> &open, const Node &nd)
{
	Node pNode = nd; //top
	char lastMove = pNode.nodeInfo.move;
	char blank = pNode.nodeInfo.blank;

	for (int move = 0; move < 4; ++move)
	{
		if (lastMove == -1 || (move + lastMove) % 4 != 1) //�ι�° ���ǽ��� ������ �ڸ� �ٽ� �� ���� �� �Ÿ�
		{
			//blank�ڸ����� move���� �˻�
			//moveableTable: 25���� �ڸ�, 4���� ����
			char newBlank = movableTable[blank][move];
			if (newBlank == -1) continue;

			//������ ���⸶�� �ϳ��� �ٲ㺽
			pNode.board[blank] = pNode.board[newBlank];
			pNode.board[newBlank] = 25;

			nodeCount += 1;
			//�� ��츶�� �޸���ƽ ����
			uc h = GetHeuristic(pNode.board);
			uc g = pNode.g + 1;

			//������
			Node node(pNode.board, newBlank, move, nodeCount, pNode.nodeInfo.id, g + h, g);
			//������ -> open�� ������ �� �ִ� ��� ������ ��带 ������
			open.push(node);

			//���� ����
			pNode.board[newBlank] = pNode.board[blank];
			pNode.board[blank] = 25;
		}
	}
}

// vector�� ����� ������ ���������� ������
// �������� id�� ���� ����� parent�� ���ٸ� ������ ��� movedway�� ����
void MakePath(vector<NodeInfo> &vni, char* puzzle, uc depth)
{
	bool isFirst = true;
	NodeInfo temp, info;
	while (!vni.empty())
	{
		// vector�� ���� ������ ���(��, ������ �Ϸ�� board�� ������ ���)���� ����
		temp = vni.back();
		if (isFirst || temp.id == info.parent)
		{
			// �ش� board�� ���� ���� ������ ������ ���� ����
			// Ȧ��
			if (temp.move % 2) temp.move -= 1;
			// ¦��
			else temp.move += 1;
			char newBlank = movableTable[temp.blank][temp.move];

			//�ڸ� �ٲ�
			puzzle[temp.blank] = puzzle[newBlank];
			puzzle[newBlank] = 25;
			path[--depth] = puzzle[temp.blank]; //������
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
	// node �ʱ�ȭ
	// ��Ʈ����� id = 0 , parent = 0, g=0
	// ��Ʈ��� = �ʱ� blank�� ��ġ, ���� ���� ������
	// �Էµ� ���̹Ƿ� move = -1�� ������(������ ������ ����)
	Node node(puzzle, GetBlank(puzzle), -1, nodeCount, parent, f, g);
	vector<NodeInfo> closed;
	priority_queue<Node, vector<Node>, greater<Node>> open; //�켱���� ť(min_heap�� ����Ͽ� f�� ���� �ּ��� ������ top�� �ֵ���)

	// �Էµ� ���� �̹� ���ĵǾ� �ִ� �������̹Ƿ� depth = 0�� ��ȯ
	if (h == 0) {
		return h;
	}

	// ��Ʈ ����� �ڽ� ��带 �����Ͽ� open ť�� push
	ExpandNode(open, node);
	while (!open.empty())
	{
		// openť�� f���� ���� ������ priority�� ����, top�� ����
		node = open.top();
		open.pop();

		// node.g = �ش� ����� depth�� �ǹ���
		// g = open���� ���� ����� depth �� ���� ū �� 
		// (���� ��� ������ ������ ���´����� �ǹ�)
		// ���� ��尡 ���� g������ ũ�ٸ� g���� ����
		if (node.g > g) 	g = node.g;
		if (g - node.g > 12) continue;

		// solved�� ��
		// f = h+g �̹Ƿ� f=g�� ��, h=0 �̹Ƿ� �ϼ��� ������ ������ ��Ÿ�� ���
		if (node.f == node.g)
		{
			closed.push_back(node.nodeInfo);
			MakePath(closed, node.board, node.g);
			// ���������� ã�� ����, depth�� ��ȯ��
			// �� ���� �̵� ��� Ƚ��
			return node.g;
		}

		// ���� ����� ����(node.g)�� ������� ���� ����� ����(g)����
		// 11���� ���Ϸ� ���� �����
		// close vector�� node�� ����
		// 12���� �̻� ���̳��� ����� ���� �ʰ� openť���� ���ο� ��带 ����
		closed.push_back(node.nodeInfo);
		// ������ ����� �ڽ� ��带 �ٽ� ����
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

int t; //�Է¹��� ������ ����

void puzzleSolution(void) {
	int i = 0, z = 0, k = 0;
	int size = N;
	char puzzle[20][25];
	int board[N][N];

	printf("puzzle size : 5*5, blank : 25\n");
	scanf("%d", &t);

	//t���� �������� �Է¹���
	printf("input puzzle data\n");
	while (z < t) {
		for (int i = 0; i < size * size; ++i)
		{
			scanf("%d", &puzzle[z][i]);
		}
		++z;
	}
	z = 0;

	//���� Ǯ� ��� ���
	printf("\n====���====\n");
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
