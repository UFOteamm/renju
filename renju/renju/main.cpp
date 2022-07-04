#include <iostream>
#include <string>
#include <sstream>
#include "jsoncpp/json.h" // C++����ʱĬ�ϰ����˿�

using namespace std;

int main()
{
    // ����JSON
    string str;
    getline(cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    // �����Լ��յ���������Լ���������������ָ�״̬
    string data = input["data"].asString(); // �öԾ��У���ǰ��Bot����ʱ�洢����Ϣ
    int turnID = input["responses"].size();
    for (int i = 0; i < turnID; i++)
    {
        istringstream in(input["requests"][i].asString()),
            out(input["responses"][i].asString());

        // ������Щ��������𽥻ָ�״̬����ǰ�غ�
    }

    // �����Լ����غ�����
    istringstream in(input["requests"][turnID].asString());

    // �������ߴ�ΪmyAction

    // �������JSON
    Json::Value ret;
    ret["response"] = myAction;
    ret["data"] = myData; // ���Դ洢һЩǰ������Ϣ���������Ծ���ʹ��
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
    return 0;
}