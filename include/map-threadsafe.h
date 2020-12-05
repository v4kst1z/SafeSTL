//
// Created by V4kst1z.
//

#ifndef SAFESTL_MAP_THREADSAFE_H
#define SAFESTL_MAP_THREADSAFE_H

#include <shared_mutex>
#include <vector>
#include <list>
#include <utility>
#include <map>

namespace std {
    namespace threadsafe {
        template <typename Key, typename Value, typename Hash = std::hash<Key> >
        class SafeUnorderedMap {
        public:
            SafeUnorderedMap(unsigned numBucket = 19, const Hash& hasher_ = Hash()) :  buckets(numBucket), hasher(hasher_) {
                for(unsigned id = 0; id < numBucket; id++)
                {
                    buckets[id].reset(new Bucket);
                }
            }
            SafeUnorderedMap(const SafeUnorderedMap& other) = delete;
            SafeUnorderedMap& operator=(const SafeUnorderedMap& other) = delete;

            Value valueFor(const Key& key, Value const& default_value=Value()) const {
                return getBucket(key).valueFor(key, default_value);
            }

            void addOrUpdate(const Key& key, const Value& val) {
                getBucket(key).addOrUpdate(key, val);
            }

            void removeMap(const Key& key) {
                getBucket(key).removeMap(key);
            }

        private:
            class Bucket {
            private:
                typedef std::pair<Key, Value> bucketValue;
                typedef std::list<bucketValue> bucketData;
                typedef typename bucketData::iterator bucketIter;
                typedef typename bucketData::const_iterator constBucketIter;

                bucketData data;
                mutable std::shared_mutex mut;

                bucketIter findEntry(const Key& key)  {
                    return std::find_if(data.begin(), data.end(), [&](const bucketValue& item) -> bool { return item.first == key;});
                }
            public:
                Value valueFor(const Key &key, const Value& default_vale)  {
                    std::shared_lock<std::shared_mutex> lk(mut);
                    const bucketIter entry = findEntry(key);
                    return (entry != data.end()) ? entry->second : default_vale;
                }

                void addOrUpdate(const Key& key, const Value& val) {
                    std::unique_lock<std::shared_mutex> lk(mut);
                    const bucketIter entry = findEntry(key);
                    if (entry == data.end()) {
                        data.push_back(bucketValue(key, val));
                    } else {
                        entry->second = val;
                    }
                }

                void removeMap(const Key &key) {
                    std::unique_lock<std::shared_mutex> lk(mut);
                    bucketIter entry = findEntry(key);
                    if (entry != data.end()) {
                        data.erase(key);
                    }
                }
            };

            Bucket& getBucket(const Key &key) const {
                std::size_t id = hasher(key) % buckets.size();
                return *buckets[id];
            }

            std::vector<std::unique_ptr<Bucket>> buckets;
            Hash hasher;
        };
    }
}

#endif //SAFESTL_MAP_THREADSAFE_H
