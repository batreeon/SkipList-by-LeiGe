#include "CSkiplist.h"
#include <iostream>

const char* filePath = "testfile";

int main() {
	CSkipList<int, std::string> skipList(6, filePath);
	/*skipList.InsertNode(1, "ѧϰ");
	skipList.InsertNode(2, "ʹ");
	skipList.InsertNode(3, "��");
	skipList.InsertNode(4, "����");
	skipList.InsertNode(19, "�㷨");
	skipList.InsertNode(19, "����");*/
	std::cout << "skipList size:" << skipList.Size() << std::endl;
	skipList.LoadFile();
	//skipList.DumpFile();
	skipList.DisplayList();
	return 0;
}