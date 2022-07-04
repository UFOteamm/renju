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
		enum State :uint8_t/*��ʡ�ڴ�*/ { SPACE, BLACK, WHITE };
	private:
		friend class GameTree;
		static const uint8_t BOARDSIZE = 15;
		int value;//Ҷ�ڵ��ʾ���ۺ����Ľ����MAX�ڵ��ʾ��ֵ��MIN�ڵ��ʾ��ֵ
		int evaluateValue;//���ۺ�������Ľ���������Ż�
		unsigned short depth;//���
		uint8_t lastX, lastY;//��һ�����ӵ�xy����
		Node* father;//���׽ڵ�
		std::vector<Node*> children;//�ӽڵ�
		State** board;//����
		int Evaluate()const//���ۺ���
		{
			static auto EvaluateSome = [](State** board, uint8_t beginX, uint8_t endX, uint8_t beginY, uint8_t endY) {
				static auto EvaluateList = [](const std::array<State, 5>& v)//�ٶ��Լ��ǰ׷�
				{
					//�ж���ɫ����¼���Ӹ���
					State lastColor = SPACE;
					uint8_t count = 0;
					for (State i : v)
					{
						if (i != SPACE)
						{
							++count;
							if (i != lastColor)
							{
								if (lastColor == SPACE)//�����ĵ�һ������
								{
									lastColor = i;
								}
								else//�в�ͬ��ɫ������
								{
									return 0;
								}
							}
						}
					}
					if (!count)//û������
						return 0;
					if (count == 5)
					{
						return lastColor == WHITE ? INT_MAX : INT_MIN;//һ����Ҫ��Ϊ-INT_MAX����INT_MIN��
					}
					const int result = static_cast<int>(std::pow(10, count - 1));
					return lastColor == WHITE ? result : -3 * result;//���ַ��ظ�ֵ���ҷ�������ֵ
				};
				int result = 0;
				for (uint8_t i = beginX; i < endX; i++)//�ֱ���ĸ������ж�
				{
					for (uint8_t j = beginY; j < endY; j++)
					{
						if (j + 4 < endY)
						{
							std::array<State, 5>v;
							for (uint8_t k = 0; k < 5; k++)
								v[k] = board[i][j + k];
							const int t = EvaluateList(v);
							if (t == INT_MAX || t == INT_MIN)//����ʤ��ֱ�ӷ���
								return t;
							result += t;
						}
						if (i + 4 < endX)
						{
							std::array<State, 5>v;
							for (uint8_t k = 0; k < 5; k++)
								v[k] = board[i + k][j];
							const int t = EvaluateList(v);
							if (t == INT_MAX || t == INT_MIN)//����ʤ��ֱ�ӷ���
								return t;
							result += t;
						}
						if (i + 4 < endX && j + 4 < endY)
						{
							std::array<State, 5>v;
							for (uint8_t k = 0; k < 5; k++)
								v[k] = board[i + k][j + k];
							const int t = EvaluateList(v);
							if (t == INT_MAX || t == INT_MIN)//����ʤ��ֱ�ӷ���
								return t;
							result += t;
						}
						if (i + 4 < endX && j >= 4)
						{
							std::array<State, 5>v;
							for (uint8_t k = 0; k < 5; k++)
								v[k] = board[i + k][j - k];
							const int t = EvaluateList(v);
							if (t == INT_MAX || t == INT_MIN)//����ʤ��ֱ�ӷ���
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
			if (t == INT_MAX || t == INT_MIN)//����ʤ��ֱ�ӷ���
				return t;
			return  t - EvaluateSome(father->board, beginX, endX, beginY, endY) + father->evaluateValue;
		}
	public:
		//�Ǹ��ڵ�Ĺ��캯��
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
		//���ڵ�Ĺ��캯��
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
		inline bool IsMaxNode()const//Ĭ�ϼ��������
		{
			return depth & 1u;//�൱��depth%2
		}
		void Search(unsigned short _depth)
		{
			if (_depth == 0 || this->evaluateValue == INT_MAX || this->evaluateValue == INT_MIN)
			{
				this->value = this->evaluateValue;
				return;
			}
			bool created = false;//��¼�Ƿ�new���µ�Node�����û�оͲ��������ˡ�
			if (!board)//���Ǹ��ڵ�
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
						if (_depth >= 2)//��ʣ�����С��2������һ��϶�û��������
						{
							for (Node* child : this->children)
							{
								if (child->lastX == i && child->lastY == j)//�Ѿ���������
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
						});//���չ��۴Ӵ�С�������Ӽ�֦�ĸ���
				}
			}
			else
			{
				this->value = INT_MAX;
				if (created)
				{
					std::sort(this->children.begin(), this->children.end(), [](Node* a, Node* b) {
						return a->GetEvaluateValue() < b->GetEvaluateValue();
						});//���չ��۴�С�����������Ӽ�֦�ĸ���
				}
			}
			auto ReleaseMemory = [this] {
				if (father)//���Ǹ��ڵ�
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
				//�� - �� ��֦
				if (IsMaxNode())
				{
					if (child->value > this->value)
					{
						this->value = child->value;
						if (this->father && this->value >= this->father->value && this != this->father->children.front())
						{
							ReleaseMemory();
							return;//��֦
						}
					}
				}
				else//MIN�ڵ�
				{
					if (child->value < this->value)
					{
						this->value = child->value;
						if (this->father && this->value <= this->father->value && this != this->father->children.front())
						{
							ReleaseMemory();
							return;//��֦
						}
					}
				}
			}
			ReleaseMemory();
		}
		void DeleteAllButThis()
		{
			if (!father->board)//���ڵ㲻�Ǹ��ڵ�
				throw std::invalid_argument("this->father�����Ǹ��ڵ㣡");
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
			free(father);//���������������
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
			throw std::invalid_argument("������������ڵ���2��");
	}
	std::pair<uint8_t, uint8_t>AIGetNextPos(uint8_t x, uint8_t y)
	{
		if (root)
		{
			for (Node* node : root->children)//�����û�ѡ��ķ�֧
			{
				if (node->lastX == x && node->lastY == y)
				{
					node->DeleteAllButThis();
					root = node;
					break;
				}
			}
		}
		else//��һ�ο���
		{
			root = new Node(1, x, y);
		}
		root->Search(maxDepth);
		if (root->IsFull())
			return std::make_pair(Node::BOARDSIZE, Node::BOARDSIZE);
		for (Node* node : root->children)//ѡ���ֵ����
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
				std::cout << "����x,y����";
				std::cin >> x >> y;
			} while (x < 0 || y < 0 || x >= 15 || y >= 15 || (root && root->board[x][y] != Node::SPACE));
			if (root)
			{
				for (Node* node : root->children)//�����û�ѡ��ķ�֧
				{
					if (node->lastX == x && node->lastY == y)
					{
						node->DeleteAllButThis();
						root = node;
						break;
					}
				}
			}
			else//��һ�ο���
			{
				root = new Node(1, x, y);
			}
			system("cls");
			for (int i = 0; i < Node::BOARDSIZE; i++)
			{
				for (int j = 0; j < Node::BOARDSIZE; j++)
				{
					if (root->board[i][j] == Node::SPACE)
						std::cout << "ʮ ";
					else if (root->board[i][j] == Node::BLACK)
						std::cout << "�� ";
					else
						std::cout << "�� ";
				}
				std::cout << '\n';
			}
			root->Search(maxDepth);
			if (root->value == INT_MAX)
			{
				std::cout << "����ʤ����";
				break;
			}
			else if (root->value == INT_MIN)
			{
				std::cout << "���ʤ����";
				break;
			}
			else if (root->IsFull())//������root->value==0�ж�ƽ�֣���Ϊƽ��һ��Ϊ0����Ϊ0��һ��ƽ��
			{
				std::cout << "ƽ�֣�";
				break;
			}
			for (Node* node : root->children)//ѡ���ֵ����
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
						std::cout << "ʮ ";
					else if (root->board[i][j] == Node::BLACK)
						std::cout << "�� ";
					else
						std::cout << "�� ";
				}
				std::cout << '\n';
			}
			if (root->value == INT_MAX)
			{
				std::cout << "����ʤ����";
				break;
			}
			else if (root->value == INT_MIN)
			{
				std::cout << "���ʤ����";
				break;
			}
			else if (root->IsFull())//������root->value==0�ж�ƽ�֣���Ϊƽ��һ��Ϊ0����Ϊ0��һ��ƽ��
			{
				std::cout << "ƽ�֣�";
				break;
			}
		}
	}
	~GameTree()
	{
		delete root;
	}
};
