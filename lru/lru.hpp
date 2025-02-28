#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "utility.hpp"
#include "exceptions.hpp"
#include "class-integer.hpp"
#include "class-matrix.hpp"
#include <iostream>
void innerflag() {
	std::cout << "MAPOK" << std::endl;
}
class Hash {
public:
	unsigned int operator () (Integer lhs) const {
		int val = lhs.val;
		return std::hash<int>()(val);
	}
};
class Equal {
public:
	bool operator () (const Integer &lhs, const Integer &rhs) const {
		return lhs.val == rhs.val;
	}
};

namespace sjtu {
template<class T> class double_list{
private:
	struct Node{
		T item;
		Node *prev = nullptr, *next = nullptr;
		Node(const T& x): item(x) {}
		~Node() {
			prev = nullptr, next = nullptr;
		}
	};
public:
	Node *head = nullptr, *tail = nullptr;
	
	double_list(){
	}
	double_list(const double_list<T> &other) {
		Node *cur = other.head;
		while (cur) {
			insert_tail(cur->item);
			cur = cur->next;
		}
	}
	double_list & operator=(const double_list<T> & other) {
		if (this == &other) {return *this; }
		clear();
		Node *cur = other.head;
		while (cur) {
			insert_tail(cur->item);
			cur = cur->next;
		}
		return *this;
	}
	~double_list(){
		clear();
	}

	void clear() {
		//std::cout << "list_clear" << std::endl;
		Node *cur = head, *nxt;
		while (cur) {
			nxt = cur->next;
			delete cur;
			cur = nxt;
		}
		head = nullptr;
		tail = nullptr;
	}

	class iterator{
	public:
		Node* it = nullptr;

		iterator(Node* node = nullptr): it(node) {}
		iterator(const iterator &t): it(t.it) {
		}
		~iterator(){it = nullptr; }

		iterator operator++(int) {
			Node* cur = it;
			if (it == nullptr) {throw index_out_of_bound(); }
			it = it->next;
			return iterator(cur);
		}
 
		iterator &operator++() {
			if (it == nullptr) {throw index_out_of_bound(); }
			it = it->next;
			return *this;
		}

		iterator operator--(int) {
			Node* cur = it;
			if (it == nullptr || it->prev == nullptr) {throw index_out_of_bound();}
			it = it->prev;
			return iterator(cur);
		}
     
		iterator &operator--() {
			if (it == nullptr || it->prev == nullptr) {throw index_out_of_bound(); }
			it = it->prev;
			return *this;
		}
		/**
		 * if the iter didn't point to a value
		 * throw " invalid"
		*/
		T &operator*() const {
			if (it == nullptr) {throw invalid_iterator(); }
			return it->item;
		}
        /**
         * other operation
        */
		T *operator->() const noexcept {
			return &(it->item);
		}
		bool operator==(const iterator &rhs) const {
			return it == rhs.it;
    	}
		bool operator!=(const iterator &rhs) const {
			return it != rhs.it;
		}
	};

	class const_iterator {
		public:
			const Node* it = nullptr;
			const_iterator(Node* node = nullptr) : it(node) {}
			const_iterator(const const_iterator &t) : it(t.it) {}
			~const_iterator() { it = nullptr; }

		const_iterator operator++(int) {
			const Node* cur = it;
			if (it == nullptr) { throw index_out_of_bound(); }
			it = it->next;
			return const_iterator(cur);
		}

		const_iterator &operator++() {
			if (it == nullptr) { throw index_out_of_bound(); }
			it = it->next;
			return *this;
		}

		const_iterator operator--(int) {
			const Node* cur = it;
			if (it == nullptr || it->prev == nullptr) { throw index_out_of_bound(); }
			it = it->prev;
			return const_iterator(cur);
		}

		const_iterator &operator--() {
			if (it == nullptr || it->prev == nullptr) { throw index_out_of_bound(); }
			it = it->prev;
			return *this;
		}

		const T &operator*() const {
            if (it == nullptr) { throw invalid_iterator(); }
            return it->item;
        }

        const T *operator->() const noexcept {
            return &(it->item);
        }

        bool operator==(const const_iterator &rhs) const {
            return it == rhs.it;
        }

        bool operator!=(const const_iterator &rhs) const {
            return it != rhs.it;
        }
	};

	/**
	 * return an iterator to the beginning
	 */
	iterator begin(){
		return iterator(head);
	}
	/**
	 * return an iterator to the ending
	 * in fact, it returns the iterator point to nothing,
	 * just after the last element.
	 */
	iterator end(){
		return iterator(tail ? tail->next : nullptr);
	}

	const_iterator cbegin() const {
        return const_iterator(head);
    }

    const_iterator cend() const {
        return const_iterator(tail ? tail->next : nullptr);
    }

	iterator back() {
		return iterator(tail);
	}
	/**
	 * if the iter didn't point to anything, do nothing,
	 * otherwise, delete the element pointed by the iter
	 * and return the iterator point at the same "index"
	 * e.g.
	 * 	if the origin iterator point at the 2nd element
	 * 	the returned iterator also point at the
	 *  2nd element of the list after the operation
	 *  or nothing if the list after the operation
	 *  don't contain 2nd elememt.
	*/
	iterator erase(iterator pos){
		//std::cout << "list_erase" << std::endl;
		if (pos.it == nullptr) {return iterator(nullptr); }
		Node *nxt = pos.it->next, *pre = pos.it->prev;
		//relink
		if (pre != nullptr) {
			pre->next = nxt;
		}
		if (nxt != nullptr) {
			nxt->prev = pre;
		}
		if (pos.it == head) {
			head = nxt;
		}
		if (pos.it == tail) {
			tail = pre;
		}
		delete pos.it;
		//innerflag();
		return iterator(nxt);
	}

	void move_to_tail(iterator pos) {
		if (pos.it == nullptr || pos.it == tail) {return; }
		Node *nxt = pos.it->next, *pre = pos.it->prev;
		// relink
		if (pre != nullptr) {
			pre->next = nxt;
		}
		if (nxt != nullptr) {
			nxt->prev = pre;
		}
		if (pos.it == head) {
			head = nxt;
		}
		if (pos.it == tail) {
			tail = pre;
		}
		// to tail
		if (tail != nullptr) {tail->next = pos.it; }
		pos.it->next = nullptr;
		pos.it->prev = tail;
		tail = pos.it;
	}

	/**
	 * the following are operations of double list
	*/
	void insert_head(const T &val){
		Node* new_node = new Node(val);
		if (head == nullptr) {
			head = new_node;
			tail = head;
			return;
		}
		new_node->next = head;
		if (head != nullptr) {head->prev = new_node; }
		head = new_node;
	}
	void insert_tail(const T &val){
		//std::cout << "insert_tail" << std::endl;
		Node* new_node = new Node(val);
		if (head == nullptr) {
			head = new_node;
			tail = head;
			return;
		}
		new_node->prev = tail;
		if (tail != nullptr) {tail->next = new_node; }
		tail = new_node;
	}
	void delete_head(){
		if (head == nullptr) {throw container_is_empty(); }
		Node* new_head = head->next;
		delete head;
		head = new_head;
		if (head != nullptr) {
			head->prev = nullptr;
		} else {
			tail = nullptr;
		}
	}
	void delete_tail(){
		if (head == nullptr) {throw container_is_empty(); }
		Node* new_tail = tail->prev;
		delete tail;
		tail = new_tail;
		if (tail != nullptr) {
			tail->next = nullptr;
		} else {
			head = nullptr;
		}
	}
	/**
	 * if didn't contain anything, return true, 
	 * otherwise false.
	 */
	bool empty() const{
		return head == nullptr;
	}

	void print() {
		for (iterator it(head); it != end(); ++it) {
			std::cout << it.it->item.first << " " <<it.it->item.second<< std::endl;
		}
	}
};

template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class hashmap{
public:
	using value_type = pair<const Key, T>;

	std::vector<double_list<value_type>* > vec;
	size_t size, curL;  // capacity and current load
	static double loadFactor;
// --------------------------

	hashmap(size_t s = 1024): size(s), curL(0) {
		vec.resize(size, nullptr);
	}
	hashmap(const hashmap &other): size(other.size), curL(other.curL) {
		vec.resize(other.vec.size(), nullptr);
		for (size_t i = 0; i < other.vec.size(); ++i) {
			if (other.vec[i]) {
				vec[i] = new double_list<value_type>(*other.vec[i]);
			}
		}
	}
	~hashmap(){
		clear();
	}
	hashmap & operator=(const hashmap &other){
		//std::cout << "hashmap_copy_assign" << std::endl;
		if (this == &other) {return *this; }
		clear();
		//innerflag();
		vec.resize(other.vec.size(), nullptr);
		for (size_t i = 0; i < other.vec.size(); ++i) {
			if (other.vec[i]) {
				vec[i] = new double_list<value_type>(*other.vec[i]);
			}
		}
		size = other.size;
		curL = other.curL;
		return *this;
	}

	class iterator{
	public:
		// using VI = typename std::vector<double_list<value_type>* >::iterator;  // to iterate vec
		using EI = typename double_list<value_type>::iterator;  // to iterate element in each vec-->list
		using CVI = typename std::vector<double_list<value_type>* >::const_iterator;
		CVI vecIt, vecEnd;
		EI eleIt;
		void moveNextList() {
			while (vecIt != vecEnd && eleIt == (*vecIt)->end()) {
				++vecIt;
				if (vecIt != vecEnd) {
					eleIt = (*vecIt)->begin();
				}
			}
		}
	public:
		iterator(const CVI v1, const CVI ve, const EI e): vecIt(v1), vecEnd(ve), eleIt(e) {
			moveNextList();
		}
		iterator(const iterator &t): vecIt(t.vecIt), vecEnd(t.vecEnd), eleIt(t.eleIt) {
		}
		~iterator(){}

        /**
		 * if point to nothing
		 * throw 
		*/
		value_type &operator*() const {
			if (eleIt == nullptr) {throw invalid_iterator(); }
			return *eleIt;
		}

        /**
		 * other operation
		*/
		value_type *operator->() const noexcept {
			return &(*eleIt);
		}
		bool operator==(const iterator &rhs) const {
			return vecIt == rhs.vecIt && (vecIt == vecEnd || eleIt == rhs.eleIt);
    	}
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
	};

	void print() {
		for (auto v: vec) {
			if (v != nullptr) {
				//std::cout << "notNULL" << std::endl;
				v->print();
			}
		}
	}

	void clear(){
		//std::cout << "hashmap_clear" << std::endl;
		int cnt = 0;
		for (auto& v: vec) {
			//std::cout << ++cnt << std::endl;
			delete v;
			v = nullptr;
		}
		curL = 0;
	}
	/**
	 * you need to expand the hashmap dynamically
	*/
	void expand(){
		//std::cout << "hashmap_expand" << std::endl;
		size_t new_size = size * 2;
		std::vector<double_list<value_type>* > new_vec(new_size, nullptr);
		// rehash
		Hash hash;
		for (auto cur: vec) {  // cur(dbly_lst<value_type>*)
			for (auto node = cur?cur->head:nullptr; node != nullptr; node = node->next) {
				size_t index = hash(node->item.first) % new_size;
				if (new_vec[index] == nullptr) {new_vec[index] = new double_list<value_type>(); }
				new_vec[index]->insert_tail(node->item);
				//innerflag();
			}
			delete cur;
			cur = nullptr;
		}
		vec = std::move(new_vec);
		size = new_size;
	}

    /**
     * the iterator point at nothing
    */
	iterator end() const{
		//std::cout << "hashmap_end" << std::endl;
		return iterator(vec.end(), vec.end(), vec.size()>0?(vec.back()?vec.back()->end():nullptr):nullptr);
	}
	/**
	 * find, return a pointer point to the value
	 * not find, return the end (point to nothing)
	*/
	iterator find(const Key &key)const{
		//std::cout << "hashmap_find" << std::endl;
		Hash hash;
		size_t index = hash(key) % size;
		for (auto curp = vec[index]?vec[index]->head:nullptr; curp != nullptr; curp = curp->next) {  // current pair (dbly_lst<value_type>*)
			//innerflag();
			Equal equal;
			if (equal(curp->item.first, key)) {return iterator(vec.begin() + index, vec.end(), typename double_list<value_type>::iterator(curp)); }
		}
		//innerflag();
		return end();
	}
	/**
	 * already have a value_pair with the same key
	 * -> just update the value, return false
	 * not find a value_pair with the same key
	 * -> insert the value_pair, return true
	*/
	sjtu::pair<iterator,bool> insert(const value_type &value_pair){
		//std::cout << "hashmap_insert" << std::endl;
		//std::cout << curL << std::endl;

		Hash hash;
		iterator f(find(value_pair.first));
		//innerflag();
		if (f != end()) {
			// update
			f.eleIt.it->item.second = value_pair.second;
			return sjtu::pair<iterator, bool>(f, false);
		}
		// insert
		if (++curL > size_t(loadFactor * size)) {expand(); }
		size_t index = hash(value_pair.first) % size;
		if (vec[index] == nullptr) {
			vec[index] = new double_list<value_type>();
		}
		vec[index]->insert_tail(value_pair);
		return sjtu::pair<iterator, bool>(iterator(vec.begin() + index, vec.end(), vec[index]->back()), true);
	}
	/**
	 * the value_pair exists, remove and return true
	 * otherwise, return false
	*/
	bool remove(const Key &key){
		iterator f(find(key));
		if (f == end()) {return false; }
		size_t index = std::distance(vec.cbegin(), f.vecIt);
		if (vec[index] == nullptr) {return false; }

		vec[index]->erase(f.eleIt);
		if (vec[index]->empty()) {
			delete vec[index];
			vec[index] = nullptr;
		}
		
		--curL;
		curL = curL < 0? 0: curL;
		return true;
	}
};

template<class Key, class T, class Hash, class Equal>
double hashmap<Key, T, Hash, Equal>::loadFactor = 0.5;


template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class linked_hashmap :public hashmap<Key,typename double_list<sjtu::pair<const Key, T>>::iterator ,Hash,Equal>{
	
public:
	typedef pair<const Key, T> value_type;
	double_list<value_type> list;
	using LIT = typename double_list<value_type>::iterator;
	using lhash = hashmap<Key,typename double_list<value_type>::iterator ,Hash,Equal>;
// --------------------------
	class const_iterator;
	class iterator{
	public:
    	/**
         * elements
         * add whatever you want
        */
		LIT listIt;
    // --------------------------
		iterator(const LIT &it = LIT()): listIt(it) {}
        iterator(const iterator &other): listIt(other.listIt) {}
		iterator(const const_iterator &other): listIt(other.listIt) {}
		~iterator(){
		}

		/**
		 * iter++
		 */
		iterator operator++(int) {
			iterator cur = *this;
			++listIt;
			return cur;
		}
		/**
		 * ++iter
		 */
		iterator &operator++() {
			++listIt;
			return *this;
		}
		/**
		 * iter--
		 */
		iterator operator--(int) {
			iterator cur = *this;
			--listIt;
			return cur;
		}
		/**
		 * --iter
		 */
		iterator &operator--() {
			--listIt;
			return *this;
		}

		/**
		 * if the iter didn't point to a value
		 * throw "star invalid"
		*/
		value_type &operator*() const {
			if (listIt == nullptr) {throw invalid_iterator(); }
			return *listIt;
		}
		value_type *operator->() const noexcept {
			return &(*listIt);
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return listIt == rhs.listIt;
		}
		bool operator!=(const iterator &rhs) const {
			return listIt != rhs.listIt;
		}
		bool operator==(const const_iterator &rhs) const {
			return listIt == rhs.listIt;
		}
		bool operator!=(const const_iterator &rhs) const {
			return listIt != rhs.listIt;
		}
	};
 
	class const_iterator {
		public:
        	/**
             * elements
             * add whatever you want
            */
			using CLIT = typename double_list<value_type>::const_iterator;
			CLIT listIt;
    // --------------------------   
		const_iterator(const CLIT &it = CLIT()) : listIt(it) {}
		const_iterator(const iterator &other): listIt(other.listIt) {}

		/**
		 * iter++
		 */
		const_iterator operator++(int) {
			const_iterator cur = *this;
			++listIt;
			return cur;
		}
		/**
		 * ++iter
		 */
		const_iterator &operator++() {
			++listIt;
			return *this;
		}
		/**
		 * iter--
		 */
		const_iterator operator--(int) {
			const_iterator cur = *this;
			--listIt;
			return cur;
		}
		/**
		 * --iter
		 */
		const_iterator &operator--() {
			--listIt;
			return *this;
		}

		/**
		 * if the iter didn't point to a value
		 * throw 
		*/
		const value_type &operator*() const {
			if (listIt == nullptr) {throw invalid_iterator(); }
			return *listIt;
		}
		const value_type *operator->() const noexcept {
			return &(*listIt);
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return listIt == rhs.listIt;
		}
		bool operator!=(const iterator &rhs) const {
			return listIt != rhs.listIt;
		}
		bool operator==(const const_iterator &rhs) const {
			return listIt == rhs.listIt;
		}
		bool operator!=(const const_iterator &rhs) const {
			return listIt != rhs.listIt;
		}
	};            
 
	linked_hashmap() {
	}
	linked_hashmap(const linked_hashmap &other): lhash(other), list(other.list) {
		for (auto it = list.begin(); it != list.end(); ++it) {
			auto result = lhash::find(it.it->item.first);
			if (result != lhash::end()) {
				result->second = it;
			}
		}
	}
	~linked_hashmap() {
		clear();
	}
	linked_hashmap & operator=(const linked_hashmap &other) {
		if (this == &other) { return *this; }
		clear();
		lhash::operator=(other);
		list = other.list;
		for (auto it = list.begin(); it != list.end(); ++it) {
			auto result = lhash::find(it.it->item.first);
			if (result != lhash::end()) {
				result->second = it;
			}
		}
		return *this;
	}
	

 	/**
	 * return the value connected with the Key(O(1))
	 * if the key not found, throw 
	*/
	T & at(const Key &key) {
		auto it = lhash::find(key);
		if (it == lhash::end()) {
			throw index_out_of_bound();
		}
		return it.eleIt.it->item.second.it->item.second;
	}
	const T & at(const Key &key) const {
		auto it = lhash::find(key);
        if (it == lhash::end()) {
            throw index_out_of_bound();
        }
		return it.eleIt.it->item.second.it->item.second;
	}
	T & operator[](const Key &key) {
		auto it = lhash::find(key);
		if (it == lhash::end()) {
			throw index_out_of_bound();
		}
		return it.eleIt.it->item.second.it->item.second;
	}
	const T & operator[](const Key &key) const {
		return at(key);
	}

	/**
	 * return an iterator point to the first 
	 * inserted and existed element
	 */
	iterator begin() {
		return iterator(list.begin());
	}
	const_iterator cbegin() const {
		return const_iterator(list.cbegin());
	}
    /**
	 * return an iterator after the last inserted element
	 */
	iterator end() {
		return iterator(list.end());
	}

	iterator back() {
		return iterator(list.back());
	}

	const_iterator cend() const {
		return const_iterator(list.cend());
	}
  	/**
	 * if didn't contain anything, return true, 
	 * otherwise false.
	 */
	bool empty() const {
		return list.empty();
	}

    void clear(){
		lhash::clear();
        list.clear();
	}

	size_t size() const {
		return lhash::curL;
	}
 	/**
	 * insert the value_piar
	 * if the key of the value_pair exists in the map
	 * update the value instead of adding a new element，
     * then the order of the element moved from inner of the 
     * list to the head of the list
	 * and return false
	 * if the key of the value_pair doesn't exist in the map
	 * add a new element and return true
	*/
	pair<iterator, bool> insert(const value_type &value) {
		auto result = lhash::find(value.first);
		bool exist = true;
		if (result == lhash::end()) {
			list.insert_tail(value);
			lhash::insert({value.first, LIT(list.empty()? nullptr: list.back())});
		} else {
			result.eleIt.it->item.second.it->item.second = value.second;
			list.move_to_tail(result.eleIt.it->item.second);
			//result.eleIt.it->item.second = list.back();
			exist = false;
		}
		return {iterator(list.empty()? nullptr: list.back()), exist};
	}
 	/**
	 * erase the value_pair pointed by the iterator
	 * if the iterator points to nothing
	 * throw 
	*/
	void remove(iterator pos) {
		if (pos == end()) {throw invalid_iterator(); }
		lhash::remove(pos->first);
		list.erase(pos.listIt);
	}
	/**
	 * return how many value_pairs consist of key
	 * this should only return 0 or 1
	*/
	size_t count(const Key &key) const {
		return lhash::find(key) != lhash::end() ? 1 : 0;
	}
	/**
	 * find the iterator points at the value_pair
	 * which consist of key
	 * if not find, return the iterator 
	 * point at nothing
	*/
	iterator find(const Key &key) {
		auto it = lhash::find(key);
		if (it == lhash::end()) {return end(); }
		return it.eleIt.it->item.second;
	}
};

class lru{
    using lmap = sjtu::linked_hashmap<Integer,Matrix<int>,Hash,Equal>;
    using value_type = sjtu::pair<const Integer, Matrix<int> >;
	lmap cache;
	const size_t capacity;
public:
    lru(int size): capacity(size) {
    }
    ~lru(){
    }
    /**
     * save the value_pair in the memory
     * delete something in the memory if necessary
    */
    void save(const value_type &v) {
		if (cache.size() > 0 && cache.size() >= capacity) {
			// delete sth.
			cache.remove(cache.begin());
        }
		cache.insert(v);
    }
    /**
     * return a pointer contain the value
    */
    Matrix<int>* get(const Integer &v) {
		auto result = cache.find(v);
		if (result == cache.end()) {
			return nullptr;
		}
		// update seq
		cache.list.move_to_tail(result.listIt);
		return &(result->second);
		// return &(cache.list.back().it->item.second);
    }
    /**
     * just print everything in the memory
     * to debug or test.
     * this operation follows the order, but don't
     * change the order.
    */
	void print() {
		for (typename lru::lmap::iterator it = cache.begin(); it != cache.end(); ++it) {
			std::cout << it->first.val << " " << it->second << std::endl;
		}
	}
	
};
}                                                                                                                         

#endif