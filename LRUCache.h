#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <list>

using namespace std;

// 将string映射为size_t
// 用于下面的LRU算法, 节省存储空间
template<typename T>
size_t getHash(T str) {
  return std::hash<T>{}(str);
}

template<typename Key, typename Value>
struct CacheNode {
  template<typename U, typename V>
  CacheNode(U&& key, V&& val) : k_(std::forward<Key>(key)), v_(std::forward<Value>(val)) {}

  Key k_;
  Value v_;
};

/**
 * @class: LRUCache
 * @description:
 * ! 为LRU算法提供一个统一的模板
 * . 依赖上面的CacheNode
 * . 内部实现为hash + list
 * . 针对 key 为 string 类型的缓存, 推荐经过上面的 getHash() 后再作为 key
 * . 原因是 hash 和 list 都会存储 key, 转换后可以节省空间
 * . 非线程安全
 */
template<typename Key, typename Value>
class LRUCache {
 public:
  LRUCache(size_t capacity = 100) : capacity_(capacity) {}
  ~LRUCache() = default;

  template<typename U, typename V>
  void put(U&& key, V&& res) {
    auto iter = mmap.find(key);
    if (iter != mmap.end()) {
      iter->second->v_ = std::forward<Value>(res);
      mlist.splice(mlist.begin(), mlist, iter->second);
    } else {
      mlist.emplace_front(key, std::forward<Value>(res));
      mmap.emplace(std::forward<Key>(key), mlist.begin());
      if (mmap.size() > capacity_) {
        mmap.erase(mlist.back().k_);
        mlist.pop_back();
      }
    }
  }

  // ! 返回指针
  // 使用时应该在外面封装一层判断, 不应该让api调用者直接拿到指针
  template<typename T>
  Value* get(T&& key) {
    cout << __PRETTY_FUNCTION__ << endl;
    auto iter = mmap.find(std::forward<Key>(key));
    if (iter != mmap.end()) {
      mlist.splice(mlist.begin(), mlist, iter->second);
      return &(iter->second->v_);
    }
    return nullptr;
  }

  void RemoveTail() {
    mmap.erase(mlist.back().k_);
    mlist.pop_back();
  }

  size_t size() { return mmap.size(); }

  size_t capacity() { return capacity_; }
  void SetCapacity(size_t param) {
    capacity_ = param;
    while (mmap.size() > capacity_) {
      RemoveTail();
    }
  }

  size_t capacity_;

  std::unordered_map<Key, typename std::list<CacheNode<Key, Value>>::iterator> mmap;
  std::list<CacheNode<Key, Value>> mlist;
};
