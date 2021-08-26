#include "CSkiplist.h"
#include <iostream>

const char* filePath = "testfile";

int main() {
	CSkipList<int, std::string> skipList(6, filePath);
	//skipList.InsertNode(1, "1");
	/*skipList.InsertNode(1, "ѧ");
	skipList.InsertNode(3, "�㷨");
	skipList.InsertNode(7, "��׼");
	skipList.InsertNode(8, "΢�Ź��ںţ���������¼");
	skipList.InsertNode(9, "ѧϰ");
	skipList.InsertNode(19, "�㷨����·");
	skipList.InsertNode(19, "�Ͽ��ע����ᷢ���������");*/
	std::cout << "skipList size:" << skipList.Size() << std::endl;

	//skipList.DisplayList();
	//skipList.DumpFile();
	skipList.LoadFile();
	skipList.DisplayList();
	// printf("sizeof unsigned short:%d", sizeof(unsigned short));
	return 0;
}