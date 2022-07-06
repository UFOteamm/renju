#include <iostream>
#include <string>
#include <vector>
#include<time.h>
#include "jsoncpp/json.h" // C++����ʱĬ�ϰ����˿� 
#define ROW 15
#define COL 15
#include<math.h>
#include<string.h>
#define PTIF 2147483647//������,Beta
#define NGIF -2147483648//������,Alpha
int X, Y;
int chess[15][15];
int score(int a, int co[15][15], int n)
//����,aΪ1��ʾ���壬Ϊ2��ʾ����,nΪ1����ݹ���ó�����ѭ��
{
	int i, j, k, l;
	int sum = 0, su = 0;
	for (i = 0; i < 15; i++)                //��
		for (j = 0; j < 15; j++)
			if (co[i][j] == a)
			{
				for (k = i - 1; k < i + 2; k++)
					for (l = j - 1; l < j + 2; l++)
						su += co[k][l];
				if (su == a && i != 0 && i != 14 && j != 0 && j != 14)
					sum += 10;
				su = 0;
			}
	su = 0;
	for (i = 0; i < 15; i++)                  //��
		for (j = 0; j < 15; j++)
		{
			if (co[i][j] == a)
				su++;
			else if (su > 1)
			{
				if (co[i][j - su - 1] == 0 && co[i][j] == 0)
				{
					sum += pow(10, su);
				}
				else if (co[i][j - su - 1] != 0 && co[i][j] != 0)
				{
					if (su == 5)
						sum += 100000;
				}
				else
				{
					if (su == 5)
						sum += 100000;
					else
						sum += pow(10, su - 1);

				}
				su = 0;
			}
			else
				su = 0;
		}
	for (su = 0, i = 0; i < 15; i++)              //��
		for (j = 0; j < 15; j++)
		{
			if (co[j][i] == a)
				su++;
			else if (su > 1)
			{
				if (co[j - su - 1][i] == 0 && co[j][i] == 0)
				{
					sum += pow(10, su);
				}
				else if (co[j - su - 1][i] != 0 && co[j][i] != 0)
				{
					if (su == 5)
						sum += 100000;
				}
				else
				{
					if (su == 5)
						sum += 100000;
					else
						sum += pow(10, su - 1);
				}
				su = 0;
			}
			else
				su = 0;
		}
	for (su = 0, i = 0; i < 15; i++)              //�u
		for (j = i, k = 14; j < 15; j++, k--)
			if (j >= 0 && j < 15 && k >= 0 && k < 15)
			{
				if (co[j][k] == a)
					su++;
				else if (su > 1)
				{
					if (co[j - su - 1][k + su + 1] == 0 && co[j][k] == 0)
					{
						sum += pow(10, su);
					}
					else if (co[j - su - 1][k + su + 1] != 0 && co[j][k] != 0)
					{
						if (su == 5)
							sum += 100000;
					}
					else
					{
						if (su == 5)
							sum += 100000;
						else
							sum += pow(10, su - 1);
					}
					su = 0;
				}
				else
					su = 0;
			}
	for (su = 0, i = 0; i < 20; i++)                    //�v
		for (j = i, k = 0; j < 15; j++, k++)
			if (j >= 0 && j < 15 && k >= 0 && k < 15)
			{
				if (co[j][k] == a)
					su++;
				else if (su > 1)
				{
					if (co[j - su - 1][k - su - 1] == 0 && co[j][k] == 0)
					{
						sum += pow(10, su);
					}
					else if (co[j - su - 1][k - su - 1] != 0 && co[j][k] != 0)
					{
						if (su == 5)
							sum += 100000;
					}
					else
					{
						if (su == 5)
							sum += 100000;
						else
							sum += pow(10, su - 1);
					}
					su = 0;
				}
				else
					su = 0;
			}
	if (n == 1 && a == 1)
		return sum - score(2, co, 2);
	else if (n == 1 && a == 2)
		return sum - score(1, co, 2);
	else if (n == 2)
		return sum;
}
int jdgen(int co[15][15], int x, int y, int n)
//�ж�x��y�����Ƿ���������ӣ����Է���1�����򷵻�0,nΪ1���ж��ڲ࣬Ϊ2���ж����
{
	int i, j;
	int sum = 0;
	if (n == 1)
	{
		for (i = x - 1; i < x + 2; i++)
			for (j = y - 1; j < y + 2; j++)
				if ((i > 0 && j > 0 && i < 15 && j < 15) && (co[i][j] == 1 || co[i][j] == 2))
					return 1;
	}
	else
	{
		for (i = x - 2; i < x + 3; i += 2)
			for (j = y - 2; j < y + 3; j += 2)
				if ((i > 0 && j > 0 && i < 15 && j < 15) && (co[i][j] == 1 || co[i][j] == 2))
					return 1;
	}
	return 0;
}
typedef struct
{
	int data;
	int Alpha;
	int Beta;
	int X;
	int Y;
}Tree;
int minMax(int co[15][15], int deep, int a, int Alpha, int Beta)
//��������,aΪ1��ʾ���壬Ϊ2��ʾ����,����ʱAlpha��Beta��ΪNGIF,PTIF
{
	int i, j;
	int c[15][15];
	int minmax;
	int b;
	int n = 1;
	Tree tree;
	tree.data = NGIF;
	tree.Alpha = Alpha;
	tree.Beta = Beta;
	tree.X = 7;
	tree.Y = 7;
	if (a == 1)
		b = 2;
	else
		b = 1;
	if (deep > 0)
	{
		for (i = 0; i < 15; i++)
			for (j = 0; j < 15; j++)
			{
				if (co[i][j] == 0 && jdgen(co, i, j, 1) && tree.Alpha < tree.Beta)
				{
					memcpy(c, co, sizeof(int) * 225);
					c[i][j] = a;
					minmax = minMax(c, deep - 1, b, tree.Alpha, tree.Beta);
					if (deep % 2 == 0)
					{
						if (n == 1)
						{
							tree.data = minmax;
							n++;
						}
						if (tree.Alpha < minmax)
						{
							tree.Alpha = minmax;
							tree.data = minmax;
							tree.X = i;
							tree.Y = j;
						}
					}
					else
					{
						if (n == 1)
						{
							tree.data = minmax;
							n++;
						}
						if (tree.Beta > minmax)
						{
							tree.Beta = minmax;
							tree.data = minmax;
							tree.X = i;
							tree.Y = j;
						}
					}
				}
			}
		//for (i = 0; i < 15; i++)
		//	for (j = 0; j < 15; j++)
		//	{
		//		if (co[i][j] == 0 && jdgen(co, i, j, 2) && tree.Alpha < tree.Beta)
		//		{
		//			memcpy(c, co, sizeof(int) * 225);
		//			c[i][j] = a;
		//			minmax = minMax(c, deep - 1, b, tree.Alpha, tree.Beta);
		//			if (deep % 2 == 0)
		//			{
		//				if (tree.Alpha < minmax)
		//				{
		//					tree.Alpha = minmax;
		//					tree.data = minmax;
		//					tree.X = i;
		//					tree.Y = j;
		//				}
		//			}
		//			else
		//			{
		//				if (tree.Beta > minmax)
		//				{
		//					tree.Beta = minmax;
		//					tree.data = minmax;
		//					tree.X = i;
		//					tree.Y = j;
		//				}
		//			}
		//		}
		//	}
		X = tree.X;
		Y = tree.Y;
		return tree.data;
	}
	else
	{
		return score(a, co, 1);
	}
}
using namespace std;


Json::Value putChess(int a)
{

	minMax(chess, 4, a, NGIF, PTIF);		//ѡ��һ��λ�� 
	//AIBoard.chess[chooseX][chooseY] = first;	//�����λ�÷������ǵ����� 
	Json::Value ret;
	ret["x"] = X;
	ret["y"] = Y;
	return ret;
}



void placeAt(int x, int y, int a)
{
	if (chess[x][y] > 0) return;
	if (x >= 0 && y >= 0) {
		chess[x][y] = a;
	}
}
void reverse() {
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			if (chess[i][j] == 1) {
				chess[i][j] = 2;
			}
			else if (chess[i][j] == 2) {
				chess[i][j] = 1;
			}
			else continue;

		}
	}
}
int main()
{


	// ����JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	int i = 0;
	int requestsID = input["requests"].size();
	int responsesID = input["responses"].size();

	int first;



	int turnID = input["responses"].size();
	//����
	int a,b;//a��ʾ�Լ���b��ʾ���֣���ֵ1λ���壬2Ϊ����
	if (input["requests"][i]["x"].asInt() < 0 && requestsID == 1) {
		a = 2;
		b = 1;
	}
	else {
		a = 1;
		b = 2;
	}


	for (int i = 0; i < turnID; i++) {
		if (input["responses"][i]["x"].asInt() < 0) {
			placeAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), b);
			placeAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), a);
			reverse();

			continue;
		}
		else if (input["requests"][i]["x"].asInt() < 0 && i > 0) {
			reverse();
			placeAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), b);
			placeAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), a);


			continue;
		}

		placeAt(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), b);
		placeAt(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), a);
	};
	placeAt(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(), b);
	//AI AIplayer(myBoard);
	//for (int i = 0; i < 15; i++) {
	//	for (int j = 0; j < 15; j++) {
	//		printf("%d", chess[j][i]);
	//	}
	//	printf("\n");
	//}
	//printf("%d %d", a, b);

	// �������JSON
	Json::Value ret;
	i = 0;
	if (input["requests"][i]["x"].asInt() >= 0 && responsesID == 1) {
		Json::Value action;
		action["x"] = -1;
		action["y"] = -1;
		ret["response"] = action;
	}
	else if (requestsID==1) {
		Json::Value action;
		if (input["requests"][i]["x"].asInt() == 0 && input["requests"][i]["y"].asInt() == 0) {
			action["x"] = 14;
			action["y"] = 14;

		}
		else {
			action["x"] = 0;
			action["y"] = 0;
		}
		ret["response"] = action;
	}
	else {
		ret["response"] = putChess(a);
	}

	Json::FastWriter writer;
	cout << writer.write(ret) << endl;
	return 0;
}