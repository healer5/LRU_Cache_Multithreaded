#pragma once

#include <list>
#include <unordered_map>
#include <mutex>
#include <utility> 

template <typename K, typename V>
class LRUCache {
public:
    explicit LRUCache(size_t capacity) : m_capacity(capacity) {}

    bool Get(const K& key, V& value) {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_map.find(key);
        if (it == m_map.end()) {
            return false;
        }

        m_list.splice(m_list.begin(), m_list, it->second);
        value = it->second->second;
        return true;
    }

    void Put(const K& key, const V& value) {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_map.find(key);
        if (it != m_map.end()) {
            it->second->second = value;
            m_list.splice(m_list.begin(), m_list, it->second);
            return;
        }

        if (m_map.size() >= m_capacity) {
            K last_key = m_list.back().first;
            m_map.erase(last_key);
            m_list.pop_back();
        }

        m_list.emplace_front(key, value);
        m_map[key] = m_list.begin();
    }

private:
    size_t m_capacity;
    std::list<std::pair<K, V>> m_list;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> m_map;
    std::mutex m_mutex;
};