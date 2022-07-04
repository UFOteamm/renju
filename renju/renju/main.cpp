#pragma once
#include<iostream>
#include<vector>
#include<array>
#include<fstream>
#include<algorithm>
class GameTree
{
public:
	class Node
	{
	public:
		enum State :uint8_t/*节省内存*/ { SPACE, BLACK, WHITE };
	private:
		friend class GameTree;
		static const uint8_t BOARDSIZE = 15;
		int value;//叶节点表示估价函数的结果，MAX节点表示α值，MIN节点表示β值
		int evaluateValue;//估价函数计算的结果，用于优化
		unsigned short depth;//深度
		uint8_t lastX, lastY;//上一次落子的xy坐标
		Node* father;//父亲节点
		std::vector<Node*> children;//子节点
		State** board;//棋盘
		int Evaluate()const//估价函数
		{
			static auto EvaluateSome = [](State** board, uint8_t beginX, uint8_t endX, uint8_t beginY, uint8_t endY) {
				static auto EvaluateList = [](const std::array<State, 5>& v)//假定自己是白方
				{
					//判断颜色并记录棋子个数
					State lastColor = SPACE;
					uint8_t count = 0;
					for (State i : v)
					{
						if (i != SPACE)
						{
							++count;
							if (i != lastColor)
							{
								if (lastColor == SPACE)//遇到的第一个棋子
								{
									lastColor = i;
								}
								else//有不同颜色的棋子
								{
									return 0;
								}
							}
						}
					}
					if (!count)//没有棋子
						return 0;
					if (count == 5)
					{
						return lastColor == WHITE ? INT_MAX : INT_MIN;//一定不要认为-INT_MAX就是INT_MIN！
					}
					const int result = static_cast<int>(std::pow(10, count - 1));
					return lastColor == WHITE ? result : -3 * result;//对手返回负值，我方返回正值
				};
				int result = 0;
				for (uint8_t i = beginX; i < endX; i++)//分别从四个方向判断
				{
					for (uint8_t j = beginY; j < endY; j++)
					{
						if (j + 4 < endY)
						{
							std::array<State, 5>v;
							for (uint8_t k = 0; k < 5; k++)
								v[k] = board[i][j + k];
							const int t = EvaluateList(v);
							if (t == INT_MAX || t == INT_MIN)//决出胜负直接返回
								return t;
							result += t;
						}
						if (i + 4 < endX)
						{
							std::array<State, 5>v;
							for (uint8_t k = 0; k < 5; k++)
								v[k] = board[i + k][j];
							const int t = EvaluateList(v);
							if (t == INT_MAX || t == INT_MIN)//决出胜负直接返回
								return t;
							result += t;
						}
						if (i + 4 < endX && j + 4 < endY)
						{
							std::array<State, 5>v;
							for (uint8_t k = 0; k < 5; k++)
								v[k] = board[i + k][j + k];
							const int t = EvaluateList(v);
							if (t == INT_MAX || t == INT_MIN)//决出胜负直接返回
								return t;
							result += t;
						}
						if (i + 4 < endX && j >= 4)
						{
							std::array<State, 5>v;
							for (uint8_t k = 0; k < 5; k++)
								v[k] = board[i + k][j - k];
							const int t = EvaluateList(v);
							if (t == INT_MAX || t == INT_MIN)//决出胜负直接返回
								return t;
							result += t;
						}
					}
				}
				return result;
			};
			uint8_t beginX, endX, beginY, endY;
			if (lastX <= 5)
				beginX = 0;
			else
				beginX = lastX - 5;
			endX = lastX + 5;
			if (endX > BOARDSIZE)
				endX = BOARDSIZE;
			if (lastY <= 5)
				beginY = 0;
			else
				beginY = lastY - 5;
			endY = lastY + 5;
			if (endY > BOARDSIZE)
				endY = BOARDSIZE;
			const int t = EvaluateSome(board, beginX, endX, beginY, endY);
			if (t == INT_MAX || t == INT_MIN)//决出胜负直接返回
				return t;
			return  t - EvaluateSome(father->board, beginX, endX, beginY, endY) + father->evaluateValue;
		}
	public:
		//非根节点的构造函数
		Node(Node* nf, uint8_t x, uint8_t y) :father(nf), lastX(x), lastY(y), depth(nf->depth + 1), value(0), board(new State* [BOARDSIZE])
		{
			father->children.push_back(this);
			for (int i = 0; i < BOARDSIZE; ++i)
			{
				board[i] = new State[BOARDSIZE];
				memcpy_s(board[i], BOARDSIZE * sizeof(State), father->board[i], BOARDSIZE * sizeof(State));
			}
			board[lastX][lastY] = IsMaxNode() ? BLACK : WHITE;
			evaluateValue = Evaluate();
			for (int i = 0; i < BOARDSIZE; ++i)
			{
				delete[] board[i];
			}
			delete[] board;
			board = nullptr;
		}
		//根节点的构造函数
		Node(int _depth, uint8_t x, uint8_t y) :father(nullptr), depth(_depth), lastX(x), lastY(y), value(0), evaluateValue(0), board(new State* [BOARDSIZE])
		{
			for (int i = 0; i < BOARDSIZE; ++i)
			{
				board[i] = new State[BOARDSIZE];
				memset(board[i], 0, BOARDSIZE * sizeof(State));
			}
			board[x][y] = IsMaxNode() ? BLACK : WHITE;
		}
		inline int GetEvaluateValue()const
		{
			return evaluateValue;
		}
		inline bool IsMaxNode()const//默认计算机后手
		{
			return depth & 1u;//相当于depth%2
		}
		void Search(unsigned short _depth)
		{
			if (_depth == 0 || this->evaluateValue == INT_MAX || this->evaluateValue == INT_MIN)
			{
				this->value = this->evaluateValue;
				return;
			}
			bool created = false;//记录是否new出新的Node，如果没有就不用排序了。
			if (!board)//不是根节点
			{
				board = new State * [BOARDSIZE];
				for (int i = 0; i < BOARDSIZE; ++i)
				{
					board[i] = new State[BOARDSIZE];
					memcpy_s(board[i], BOARDSIZE * sizeof(State), father->board[i], BOARDSIZE * sizeof(State));
				}
				board[lastX][lastY] = IsMaxNode() ? BLACK : WHITE;
			}
			for (uint8_t i = 0; i < BOARDSIZE; i++)
			{
				for (uint8_t j = 0; j < BOARDSIZE; j++)
				{
					if (!board[i][j])
					{
						bool flag = false;
						if (_depth >= 2)//若剩余深度小于2，则下一层肯定没有搜索过
						{
							for (Node* child : this->children)
							{
								if (child->lastX == i && child->lastY == j)//已经被搜索过
								{
									flag = true;
									break;
								}
							}
						}
						if (!flag)
						{
							new Node(this, i, j);
							created = true;
						}
					}
				}
			}
			if (IsMaxNode())
			{
				this->value = INT_MIN;
				if (created)
				{
					std::sort(this->children.begin(), this->children.end(), [](Node* a, Node* b) {
						return a->GetEvaluateValue() > b->GetEvaluateValue();
						});//按照估价从大到小排序，增加剪枝的概率
				}
			}
			else
			{
				this->value = INT_MAX;
				if (created)
				{
					std::sort(this->children.begin(), this->children.end(), [](Node* a, Node* b) {
						return a->GetEvaluateValue() < b->GetEvaluateValue();
						});//按照估价从小到大排序，增加剪枝的概率
				}
			}
			auto ReleaseMemory = [this] {
				if (father)//不是根节点
				{
					for (int i = 0; i < BOARDSIZE; ++i)
					{
						delete[] board[i];
					}
					delete[] board;
					board = nullptr;
				}
			};
			for (Node* child : this->children)
			{
				child->Search(_depth - 1);
				//α - β 剪枝
				if (IsMaxNode())
				{
					if (child->value > this->value)
					{
						this->value = child->value;
						if (this->father && this->value >= this->father->value && this != this->father->children.front())
						{
							ReleaseMemory();
							return;//剪枝
						}
					}
				}
				else//MIN节点
				{
					if (child->value < this->value)
					{
						this->value = child->value;
						if (this->father && this->value <= this->father->value && this != this->father->children.front())
						{
							ReleaseMemory();
							return;//剪枝
						}
					}
				}
			}
			ReleaseMemory();
		}
		void DeleteAllButThis()
		{
			if (!father->board)//父节点不是根节点
				throw std::invalid_argument("this->father必须是根节点！");
			for (Node* n : father->children)
			{
				if (n != this)
					delete n;
			}
			board = new State * [BOARDSIZE];
			for (int i = 0; i < BOARDSIZE; ++i)
			{
				board[i] = new State[BOARDSIZE];
				memcpy_s(board[i], BOARDSIZE * sizeof(State), father->board[i], BOARDSIZE * sizeof(State));
				delete[] father->board[i];
			}
			delete[] father->board;
			board[lastX][lastY] = IsMaxNode() ? BLACK : WHITE;
			free(father);//避免调用析构函数
			father = nullptr;
		}
		inline bool IsFull()const
		{
			return depth == BOARDSIZE * BOARDSIZE;
		}
		inline State GetWinner()const
		{
			if (this->value == INT_MAX)
			{
				return Node::WHITE;
			}
			else if (this->value == INT_MIN)
			{
				return Node::BLACK;
			}
			return Node::SPACE;
		}
		~Node()
		{
			if (board)
			{
				for (int i = 0; i < BOARDSIZE; ++i)
				{
					delete[] board[i];
				}
				delete[] board;
			}
			for (Node* n : children)
			{
				delete n;
			}
		}
	};
private:
	Node* root;
	const unsigned short maxDepth;
public:
	GameTree(unsigned short _maxDepth) : root(nullptr), maxDepth(_maxDepth)
	{
		if (_maxDepth < 2)
			throw std::invalid_argument("最大层数必须大于等于2！");
	}
	std::pair<uint8_t, uint8_t>AIGetNextPos(uint8_t x, uint8_t y)
	{
		if (root)
		{
			for (Node* node : root->children)//进入用户选择的分支
			{
				if (node->lastX == x && node->lastY == y)
				{
					node->DeleteAllButThis();
					root = node;
					break;
				}
			}
		}
		else//第一次开局
		{
			root = new Node(1, x, y);
		}
		root->Search(maxDepth);
		if (root->IsFull())
			return std::make_pair(Node::BOARDSIZE, Node::BOARDSIZE);
		for (Node* node : root->children)//选择分值最大的
		{
			if (node->value == root->value)
			{
				node->DeleteAllButThis();
				root = node;
				break;
			}
		}
		return std::make_pair(root->lastX, root->lastY);
	}
	Node::State GetWinner()const
	{
		return root->GetWinner();
	}
	void Run()
	{
		while (1)
		{
			int x, y;
			do
			{
				std::cout << "输入x,y坐标";
				std::cin >> x >> y;
			} while (x < 0 || y < 0 || x >= 15 || y >= 15 || (root && root->board[x][y] != Node::SPACE));
			if (root)
			{
				for (Node* node : root->children)//进入用户选择的分支
				{
					if (node->lastX == x && node->lastY == y)
					{
						node->DeleteAllButThis();
						root = node;
						break;
					}
				}
			}
			else//第一次开局
			{
				root = new Node(1, x, y);
			}
			system("cls");
			for (int i = 0; i < Node::BOARDSIZE; i++)
			{
				for (int j = 0; j < Node::BOARDSIZE; j++)
				{
					if (root->board[i][j] == Node::SPACE)
						std::cout << "十 ";
					else if (root->board[i][j] == Node::BLACK)
						std::cout << "黑 ";
					else
						std::cout << "白 ";
				}
				std::cout << '\n';
			}
			root->Search(maxDepth);
			if (root->value == INT_MAX)
			{
				std::cout << "电脑胜利！";
				break;
			}
			else if (root->value == INT_MIN)
			{
				std::cout << "玩家胜利！";
				break;
			}
			else if (root->IsFull())//不能用root->value==0判断平局，因为平局一定为0，但为0不一定平局
			{
				std::cout << "平局！";
				break;
			}
			for (Node* node : root->children)//选择分值最大的
			{
				if (node->value == root->value)
				{
					node->DeleteAllButThis();
					root = node;
					break;
				}
			}
			system("cls");
			for (int i = 0; i < Node::BOARDSIZE; i++)
			{
				for (int j = 0; j < Node::BOARDSIZE; j++)
				{
					if (root->board[i][j] == Node::SPACE)
						std::cout << "十 ";
					else if (root->board[i][j] == Node::BLACK)
						std::cout << "黑 ";
					else
						std::cout << "白 ";
				}
				std::cout << '\n';
			}
			if (root->value == INT_MAX)
			{
				std::cout << "电脑胜利！";
				break;
			}
			else if (root->value == INT_MIN)
			{
				std::cout << "玩家胜利！";
				break;
			}
			else if (root->IsFull())//不能用root->value==0判断平局，因为平局一定为0，但为0不一定平局
			{
				std::cout << "平局！";
				break;
			}
		}
	}
	~GameTree()
	{
		delete root;
	}
};
