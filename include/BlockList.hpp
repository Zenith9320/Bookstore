#ifndef BLOCKLIST_hpp
#define BLOCKLIST_hpp

#include "MemoryRiver.hpp"
#include <iostream>
#include <cstring>
#include <set>
#include <memory>

using std::string;

constexpr int MAX_STRING = 65;
constexpr int BLOCK_SIZE = 10000;

template <class T>
struct Key_Value {//存储键值对
    char key[MAX_STRING];
    T value;

    Key_Value & operator = (const Key_Value &other) {
        if (this != &other) {
            strcpy(key, other.key);
            value = other.value;
        }
        return *this;
    }

    Key_Value(){
        memset(key, 0, sizeof(key));
        value = T();
    };

    Key_Value(const std::string &key, const T value) : value(value) {
        strcpy(this->key, key.c_str());
    }

    bool operator > (const Key_Value<T> &other) const {
        if (strcmp(key, other.key) == 0) {
            return value > other.value;
        }
        return strcmp(key, other.key) > 0;
    }

    bool operator < (const Key_Value<T> &other) const {
        if (strcmp(key, other.key) == 0) {
            return value < other.value;
        }
        return strcmp(key, other.key) < 0;
    }

    bool operator == (const Key_Value<T> &other) const {
        return (strcmp(key, other.key) == 0 && value == other.value);
    }
};

struct Index {//存储index
    int offset;//记录偏移量
    int next_offset;//记录下一block的偏移量,用-1表示最后一个block
    char beg_key[MAX_STRING];//开始的key
    char end_key[MAX_STRING];//结束的key
    int KeyValue_num;//总共有多少对
    Index() : offset(0), next_offset(-1), KeyValue_num(0) {
        memset(beg_key, 0, sizeof(beg_key));
        memset(end_key, 0, sizeof(end_key));
    };
};

template <class T>
struct Block {
    Key_Value<T> KeyValues[BLOCK_SIZE];
    int KeyValue_num;
    Block() : KeyValue_num(0) {};
};

template <class T>
void splitBlock(Block<T> &block1, Block<T> &block2) {//把block1裂块并把多余数据存放到block2中
    int mid = BLOCK_SIZE / 2;
    for (int j = mid; j < block1.KeyValue_num; ++j) {
        block2.KeyValues[j - mid] = block1.KeyValues[j];
        block1.KeyValues[j] = (Key_Value<T>){};
        block2.KeyValue_num++;
    }
    block1.KeyValue_num = mid;
}
    
template <class T>
bool cmp(const Key_Value<T>& Key_Value1, const Key_Value<T>& Key_Value2) {
    if (strcmp(Key_Value1.key, Key_Value2.key) == 0) {
        return Key_Value1.value > Key_Value2.value;
    }
    else {
        return strcmp(Key_Value1.key, Key_Value2.key) > 0;
    }
}

template <class T>
void insertKeyvalue(Block<T> &block1, Key_Value<T> &kv) {
    block1.KeyValues[block1.KeyValue_num] = kv;
    block1.KeyValue_num++;
    if (strcmp(block1.KeyValues[block1.KeyValue_num - 1].key, kv.key) == 0) {
    }
    for (int j = block1.KeyValue_num - 1; j >= 1; --j) {//冒泡维护数据顺序
        if (cmp(block1.KeyValues[j - 1], block1.KeyValues[j])) {
            std::swap(block1.KeyValues[j], block1.KeyValues[j - 1]);
        } else {
            break;
        }
    }
    
}

template <class T>
void mergeBlocks(Block<T> &block1, Block<T> &block2, Block<T> &new_prev_block) {
    int cur = 0, cur1 = 0, cur2 = 0;
    while (cur1 < block1.KeyValue_num || cur2 < block2.KeyValue_num) {
        if (cur1 == block1.KeyValue_num) {
            new_prev_block.KeyValues[cur++] = block2.KeyValues[cur2++];
        } else if (cur2 == block2.KeyValue_num) {
            new_prev_block.KeyValues[cur++] = block1.KeyValues[cur1++];
        } else {
            int key_cmp = strcmp(block1.KeyValues[cur1].key, block2.KeyValues[cur2].key);
            if (key_cmp > 0 || (key_cmp == 0 && block1.KeyValues[cur1].value > block2.KeyValues[cur2].value)) {
                new_prev_block.KeyValues[cur++] = block2.KeyValues[cur2++];
            } else {
                new_prev_block.KeyValues[cur++] = block1.KeyValues[cur1++];
            }
        }
    }
    new_prev_block.KeyValue_num = cur;
}

//Blocklist类
template <class T>
class Blocklist {
private:
    std::string filename;
public:
    MemoryRiver<Index, 2> index_file;//开头存储键值对数量和存储偏移量
    MemoryRiver<Block<T>, 0> block_file;//直接开始存储block

    Blocklist(std::string base_filename, std::string index_filename, std::string value_filename)
    : filename(base_filename), 
      index_file(base_filename + index_filename), 
      block_file(base_filename + value_filename) {
        Index init_index;
        Block<T> init_block;
        std::ifstream file(index_file.file_name, std::ios::binary);
        //创建文件
        if (!file.is_open()) {
            index_file.initialise();
            block_file.initialise();
            init_index.offset = block_file.write(init_block);//写入第一个block
            index_file.write_info(1, 1);
            index_file.write_info(index_file.write(init_index), 2);//写入偏移量            
        }

    }

    ~Blocklist() = default;

    void Insert(const std::string &key, const T &value) {
        Key_Value<T> KeyValue(key, value);
        auto index = std::make_unique<Index>();
        auto block = std::make_unique<Block<T>>();
        //获取索引文件中的Index数量
        int block_pos = 0, index_number;
        index_file.get_info(index_number, 1);
        //获取索引文件中的当前位置
        int index_pos = 0;
        index_file.get_info(index_pos, 2);
        if (index_number == 0) {
            block->KeyValues[0] = KeyValue;
            block->KeyValue_num++;
            index->KeyValue_num++;
            strcpy(index->beg_key, key.c_str());
            strcpy(index->end_key, key.c_str());
            index->offset = block_file.write(*block);
            index_file.write_info(index_file.write(*index), 2);
            index_file.write_info(1, 1);
        }
        for (int i = 0; i < index_number; ++i) {
            index_file.read(*index, index_pos);
            // 插入键值对
            if (strcmp(index->end_key, key.c_str()) >= 0 || index->next_offset == -1) {
                block_pos = index->offset;
                block_file.read(*block, block_pos);
                // 若Block满了就需要裂块
                if (block->KeyValue_num == BLOCK_SIZE) {
                    auto new_block = std::make_unique<Block<T>>();
                    auto new_index = std::make_unique<Index>();
                    splitBlock(*block, *new_block);//裂块
                    // 插入键值对
                    if (cmp(KeyValue, new_block->KeyValues[0])) {
                        insertKeyvalue(*new_block, KeyValue);
                    } else {
                        insertKeyvalue(*block, KeyValue);
                    }

                    // 更新原数据快
                    block_file.update(*block, block_pos);
                    // 更新数据块的键值对数量
                    index->KeyValue_num = block->KeyValue_num;
                    new_index->KeyValue_num = new_block->KeyValue_num;
                    // 更新开始结束键
                    strcpy(new_index->beg_key, new_block->KeyValues[0].key);
                    strcpy(new_index->end_key, new_block->KeyValues[new_block->KeyValue_num - 1].key);
                    strcpy(index->beg_key, block->KeyValues[0].key);
                    strcpy(index->end_key, block->KeyValues[block->KeyValue_num - 1].key);
                    
                    // 更新链表
                    new_index->offset = block_file.write(*new_block);
                    new_index->next_offset = index->next_offset;
                    index->next_offset = index_file.write(*new_index);

                    // 写回更新后的原索引
                    index_file.update(*index, index_pos);

                    // 更新索引文件中存储的索引数量，增加一个新的索引
                    index_file.write_info(index_number + 1, 1);
                    break;
                } else {
                    insertKeyvalue(*block,KeyValue);
                    index->KeyValue_num = block->KeyValue_num;
                    if (strcmp(block->KeyValues[block->KeyValue_num - 1].key, index->end_key) != 0 || block->KeyValue_num == 1) {
                        strcpy(index->end_key, block->KeyValues[block->KeyValue_num - 1].key);
                    }
    
                    if (strcmp(block->KeyValues[0].key, index->beg_key) != 0 || block->KeyValue_num == 1) {
                        strcpy(index->beg_key, block->KeyValues[0].key);
                    }
    
                    index_file.update(*index, index_pos);
                    block_file.update(*block, block_pos);
                    break;
                }
            } else {
                index_pos = index->next_offset;
            }
        }
    }
    void DeleteKeyValue(const std::string &key, const T &value) {
        Key_Value<T> KeyValue(key, value);
        auto index = std::make_unique<Index>();
        auto block = std::make_unique<Block<T>>();
        int index_pos = 0;
        index_file.get_info(index_pos, 2);
        int prev_pos = -1, prev_KeyValue_num = -1, index_number;
        index_file.get_info(index_number, 1);
    
        for (int i = 0; i < index_number; ++i) {
            index_file.read(*index, index_pos);
            //查询直到比自己大的key
            if (strcmp(index->beg_key, key.c_str()) > 0) {
                return;
            }
            //如果在这个block内
            if (strcmp(index->end_key, key.c_str()) >= 0) {
                block_file.read(*block, index->offset);
                for (int j = 0; j < block->KeyValue_num; ++j) {
                    if ((block->KeyValues[j].value == KeyValue.value) && (strcmp(KeyValue.key, block->KeyValues[j].key) == 0)) {
                        //删除，键值块前移
                        for (int k = j; k < block->KeyValue_num - 1; ++k) {
                            block->KeyValues[k] = block->KeyValues[k + 1];
                        }
                        block->KeyValue_num--;
                        //如果删了唯一元素
                        if (block->KeyValue_num == 0) {
                            if (prev_pos != -1) {
                                auto prev_index = std::make_unique<Index>();
                                index_file.read(*prev_index, prev_pos);
                                prev_index->next_offset = index->next_offset;
                                index_file.update(*prev_index, prev_pos);
                            } else {
                                index_file.write_info(index->next_offset, 2);
                            }
                            index_file.write_info(index_number - 1, 1);
                        }
                        //与前一个块合并：减少读写次数
                        else if (prev_pos != -1 && block->KeyValue_num <= BLOCK_SIZE / 2 && prev_KeyValue_num <= BLOCK_SIZE / 2) {
                            auto prev_index = std::make_unique<Index>();
                            auto prev_block = std::make_unique<Block<T>>();

                            index_file.read(*prev_index, prev_pos);
                            block_file.read(*prev_block, prev_index->offset);

                            prev_index->next_offset = index->next_offset;
                            
                            //并块
                            Block<T> new_prev_block;
                            
                            mergeBlocks(*block, *prev_block, new_prev_block);
                            prev_index->KeyValue_num = new_prev_block.KeyValue_num;

                            strcpy(prev_index->beg_key, new_prev_block.KeyValues[0].key);
                            strcpy(prev_index->end_key, new_prev_block.KeyValues[prev_index->KeyValue_num - 1].key);

                            index_file.update(*prev_index, prev_pos);
                            block_file.update(new_prev_block, prev_index->offset);
                            index_file.write_info(index_number - 1, 1);
                        }
                        else {
                            if (strcmp(block->KeyValues[0].key, index->beg_key) != 0) {
                                strcpy(index->beg_key, block->KeyValues[0].key);
                            }
                            if (strcmp(block->KeyValues[block->KeyValue_num - 1].key, index->end_key) != 0) {
                                strcpy(index->end_key, block->KeyValues[block->KeyValue_num - 1].key);
                            }
                            index->KeyValue_num = block->KeyValue_num;
                            index_file.update(*index, index_pos);
                            block_file.update(*block, index->offset);
                        }
                        return;
                    }
                }
            }
            //查询下一个节点
            prev_pos = index_pos;
            index_pos = index->next_offset;
            prev_KeyValue_num = index->KeyValue_num;
        }
    }
    std::set<T> FindKey(const std::string &key) {
        int start = 0, index_number = 0;
        std::set <T> result;
        index_file.get_info(start, 2);
        index_file.get_info(index_number, 1);
        int index_pos = start;
        Index index;
        Block<T> block;
        for (int i = 0; i < index_number; ++i) {
            index_file.read(index, index_pos);

            if (strcmp(index.beg_key, key.c_str()) > 0) {
                break;
            }

            if (strcmp(index.end_key, key.c_str()) >= 0) {
                block_file.read(block, index.offset);
                int l = 0, r = block.KeyValue_num - 1;
                while (l < r) {
                    int mid = l + (r - l) / 2; 
                    if (strcmp(block.KeyValues[mid].key, key.c_str()) < 0) {
                        l = mid + 1; 
                    } else {
                        r = mid;
                    }
                }
                if (strcmp(block.KeyValues[l].key, key.c_str()) != 0) {
                    l++;
                }
                //加入所有数据
                while (l < block.KeyValue_num && (strcmp(block.KeyValues[l].key, key.c_str()) == 0)) {
                    result.insert(block.KeyValues[l].value);
                    l++;
                }
            }
            index_pos = index.next_offset;
        }
        return result;
    }
    bool if_find(const std::string &key) {
        auto result = FindKey(key);
        if (result.size() != 0) return true;
        return false;
    }
    T FindSingle(const string& key) {
        int start = 0, index_number = 0;
        T result;
        index_file.get_info(start, 2);
        index_file.get_info(index_number, 1);
        int index_pos = start;
        Index index;
        Block<T> block;
        for (int i = 0; i < index_number; ++i) {
            index_file.read(index, index_pos);

            if (strcmp(index.beg_key, key.c_str()) > 0) {
                break;
            }

            if (strcmp(index.end_key, key.c_str()) >= 0) {
                block_file.read(block, index.offset);
                int l = 0, r = block.KeyValue_num - 1;
                while (l < r) {
                    int mid = l + (r - l) / 2; 
                    if (strcmp(block.KeyValues[mid].key, key.c_str()) < 0) {
                        l = mid + 1; 
                    } else {
                        r = mid;
                    }
                }
                if (strcmp(block.KeyValues[l].key, key.c_str()) != 0) {
                    l++;
                }
                result = block.KeyValues[l].value;
                return result;
            }
            index_pos = index.next_offset;
        }
    }
    int get_KeyValue_num() {
        int tmp;
        index_file.get_info(tmp, 1);
        return tmp;
    }
};

#endif 