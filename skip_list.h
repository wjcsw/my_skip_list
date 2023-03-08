
#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <sstream>
#include <mutex>
#include <cstring>
#define FILE_PATH "save/dumpFile"
// TODO: 在此处引用程序需要的其他标头。

std::mutex mtx;  //互斥信号量

template<typename K, typename V>
class Node
{
public:
	int level;
	Node():key(),value(),level(0),next(nullptr) { }
	Node(int);
	Node(K, V, int);
	~Node();
	K get_key() const;
	V get_value() const;
	void set_value(V);
	Node** next;


private:
	K key;
	V value;
};

template<typename K, typename V>
Node<K, V>::Node(int l) :key(), value(), level(l) {
	this->next = new Node * [l + 1];
	//初始化填充0
	memset(this->next, 0, sizeof(Node*) * (l + 1));
}

template<typename K, typename V>
Node<K, V>::Node(K key, V val, int l) :key(key), value(val), level(l)
{
	this->next = new Node * [l + 1];
	//初始化填充0
	memset(this->next, 0, sizeof(Node*) * (l + 1));

}

template<typename K, typename V>
Node<K, V>::~Node()
{
	delete[]next;
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
	return this->key;
}

template<typename K, typename V>
V Node<K, V>::get_value() const {
	return this->value;
}

template<typename K, typename V>
void Node<K, V>::set_value(V val) {
	this->value = val;
}


template<typename K, typename V>
class SkipList {
public:
	typedef Node<K, V> node;
	typedef size_t size_type;
	SkipList() :_max_level(0), _cur_level(0), _size(0) , _head(nullptr) { }
	SkipList(int);
	~SkipList();
	int get_random_level();
	bool insert(K, V);
	node* search(K);
	void erase(K);
	void display();
	void save(const std::string& save_path = FILE_PATH);
	void load(const std::string& load_path = FILE_PATH);
	size_type size();

private:
	void get_key_value_from_string(const std::string& str, std::string& key, std::string& value);
	bool is_valid_string(const std::string& str);

	int _max_level; //最大高度
	int _cur_level; //当前高度

	node* _head;  //头节点

	//文件流
	std::ifstream _in;
	std::ofstream _out;

	size_type _size;
};

template<typename K, typename V>
SkipList<K, V>::SkipList(int max_l) :_max_level(max_l), _cur_level(0), _size(0) {
	_head = new node(max_l);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
	if (_in.is_open()) _in.close();
	if (_out.is_open()) _in.close();
	delete _head;
}

//用于获得跳表插入的层数  应当尽可能保持索引与原始链表的大小平衡
//即第一层为0.5 第二层为0.25 第三层为 0.125 以此类推 用奇偶性来完成
template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
	int k = 1;
	while (rand() % 2) {
		k++;
	}
	k = (k < _max_level) ? k : _max_level;
	return k;
}

template<typename K, typename V>
bool SkipList<K, V>::insert(const K key, const V val) {
	mtx.lock();
	node* cur = this->_head;

	node* update[50];
	memset(update, 0, sizeof(node*) * (_max_level + 1));

	//循环下沉链表直至将要插入的位置
	for (int i = _cur_level; i >= 0; i--) {
		while (cur->next[i] != nullptr && cur->next[i]->get_key() < key) cur = cur->next[i];
		update[i] = cur;
	}

	//此时cur要么是与当前插入值相同 要么cur的右边就是需要插入的地方
	cur = cur->next[0];

	//判断cur是否与当前插入值相同
	if (cur != NULL && cur->get_key() == key) {
		mtx.unlock();
		return false;
	}

	//若不相同 则插入新节点
	int random_level = get_random_level();

	//若随机的层数大于当前层数 开辟新层并初始化
	if (random_level > _cur_level) {
		for (int i = _cur_level + 1; i <= random_level; i++) {
			update[i] = _head;
		}
		_cur_level = random_level;
	}

	node* new_node = new node(key, val, random_level);

	for (int i = 0; i <= random_level; i++) {
		new_node->next[i] = update[i]->next[i];
		update[i]->next[i] = new_node;
	}
	_size++;
	mtx.unlock();
	return true;
}

template<typename K, typename V>
Node<K,V>* SkipList<K, V>::search(K key) {
	node* cur = _head;

	for (int i = _cur_level; i >= 0; i--) {
		while (cur->next[i] != nullptr && cur->next[i]->get_key() < key) cur = cur->next[i];
	}

	cur = cur->next[0];

	if (cur && cur->get_key() == key) {
		return cur;
	}

	return nullptr;
}

template<typename K, typename V>
void SkipList<K, V>::erase(K key) {
	mtx.lock();
	node* cur = this->_head;

	node* update[50];
	memset(update, 0, sizeof(node*) * (_max_level + 1));

	//循环下沉链表直至将要插入的位置
	for (int i = _cur_level; i >= 0; i--) {
		while (cur->next[i] != nullptr && cur->next[i]->get_key() < key) cur = cur->next[i];
		update[i] = cur;
	}

	cur = cur->next[0];
	if (cur && cur->get_key() == key) {
		// 自底而上 每层删除该节点 直至没有该节点索引
		for (int i = 0; i <= _cur_level; i++) {
			if (update[i]->next[i] != cur) break;
			update[i]->next[i] = cur->next[i];
		}
		delete cur;
		while (_cur_level > 0 && _head->next[_cur_level] == nullptr) _cur_level--;
		_size--;
	}
	mtx.unlock();
	return;
}


template<typename K, typename V>
void SkipList<K, V>::display() {
	std::cout << std::endl;
	std::cout << "<-----------------当前跳表内容----------------->" << std::endl;
	for (int i = 0; i <= _cur_level; i++) {
		node* n = this->_head->next[i];
		std::cout << "第" << i << "层: ";
		while (n) {
			std::cout << n->get_key() << ":" << n->get_value() << ";";
			n = n->next[i];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

template<typename K, typename V>
void SkipList<K, V>::save(const std::string& save_path) {
	std::cout << "<-----------------保存文件----------------->" << std::endl;
	_out.open(save_path);
	node* cur = _head->next[0];

	while(cur) {
		_out << cur->get_key() << ":" << cur->get_value() << "\n";
		std::cout << cur->get_key() << ":" << cur->get_value() << ";\n";
		cur = cur->next[0];
	}

	_out.flush();
	_out.close();
}

template<typename K, typename V>
void SkipList<K, V>::load(const std::string& load_path) {
	_in.open(load_path);
	std::cout << "<-----------------载入文件----------------->" << std::endl;
	std::string line, key, value;
	while (getline(_in, line)) {
		std::istringstream  ss;
		K k;
		V v;
		get_key_value_from_string(line, key, value);
		if (key.empty() || value.empty()) {
			continue;
		}
		ss.str(key);
		ss >> k;
		ss.str(value);
		ss >> v;
		insert(k, v);
		std::cout << "键值:" << key << "对应值:" << value << std::endl;
	}
	_out.close();
}

template<typename K, typename V>
size_t SkipList<K, V>::size() {
	return _size;
}

template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string& key, std::string& value) {

	if (!is_valid_string(str)) {
		return;
	}
	auto pos = str.find(":");
	key = str.substr(0, pos);
	value = str.substr(pos + 1, str.size());
}

template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {

	if (str.empty()) {
		return false;
	}
	if (str.find(":") == std::string::npos) {
		return false;
	}
	return true;
}


