#include "skip_list.h"
using namespace std;

int main()
{
    SkipList<int, std::string> skipList(6);
    skipList.insert(1, "这");
    skipList.insert(2, "是");
    skipList.insert(7, "我的");
    skipList.insert(13, "跳表结构");
    skipList.insert(1, "验证");

    std::cout << "跳表大小:" << skipList.size() << std::endl;

    skipList.save();

    skipList.load();

    skipList.search(1);
    skipList.search(2);


    skipList.display();

    skipList.erase(2);
    skipList.erase(7);

    std::cout << "跳表大小:" << skipList.size() << std::endl;

    skipList.display();

	return 0;
}

