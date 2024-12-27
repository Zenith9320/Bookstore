#ifndef BLOCKLIST_hpp
#define BLOCKLIST_hpp

#include "MemoryRiver.hpp"
#include <iostream>
#include <cstring>
#include <set>

using std::string;

constexpr int BLOCK_SIZE = 10000;

template<typename T>
struct Key_Value {//存储键值对
    string key;
    T value;
    Key_Value() {
        key = "";
    };

    Key_Value(const std::string &key, const T value) : value(value) {
        this->key = key;
    }
    Key_Value<T>& operator = (const Key_Value<T>& other) {
      if (this != &other) {
        this->key = other.key;
        value = other.value;
      }
      return *this;
    }
};

struct Index {//存储index
    int offset;//记录偏移量
    int next_offset;//记录下一block的偏移量,用-1表示最后一个block
    string beg_key;
    string end_key;
    int KeyValue_num;//总共有多少对
    Index() : offset(0), next_offset(-1), KeyValue_num(0) {
        beg_key = "";
        end_key = "";
    };
};

template<typename T>
struct Block {
    Key_Value<T> KeyValues[BLOCK_SIZE];
    int KeyValue_num;
    Block() : KeyValue_num(0) {
        for (int i = 0; i < BLOCK_SIZE; i++) {
            KeyValues[i] = Key_Value<T>();
        }
    }
    Block& operator = (const Block& other) {
      if (this != &other) {
        KeyValue_num = other.KeyValue_num;
        for (int i = 0; i < KeyValue_num && i < BLOCK_SIZE; i++) {
          KeyValues[i] = other.KeyValues[i];
        }
      }
      return *this;
    }
};

template<typename T>
inline void splitBlock(Block<T> &block1, Block<T> &block2) {//把block1裂块并把多余数据存放到block2中
    int mid = BLOCK_SIZE / 2;
    for (int j = mid; j < block1.KeyValue_num; ++j) {
        block2.KeyValues[j - mid] = block1.KeyValues[j];
        block1.KeyValues[j] = Key_Value<T>();
        block2.KeyValue_num++;
    }
    block1.KeyValue_num = mid;
}
    
template<typename T>
inline bool cmp(const Key_Value<T>& Key_Value1, const Key_Value<T>& Key_Value2) {
    if (Key_Value1.key == Key_Value2.key) {
        return Key_Value1.value > Key_Value2.value;
    }
    else {
        return Key_Value1.key > Key_Value2.key;
    }
}

template<typename T>
inline void insertKeyvalue(Block<T> &block1,const Key_Value<T> &kv) {
    if (block1.KeyValue_num >= BLOCK_SIZE) {
        std::cout << "1" << std::endl;
        return;
    }
    std::cout << block1.KeyValues[block1.KeyValue_num].key << " " << std::endl;
    block1.KeyValues[block1.KeyValue_num] = kv;
    block1.KeyValue_num++;
    for (int j = block1.KeyValue_num - 1; j >= 1; --j) {//冒泡维护数据顺序
        if (cmp(block1.KeyValues[j - 1], block1.KeyValues[j])) {
            std::swap(block1.KeyValues[j], block1.KeyValues[j - 1]);
        } else {
            break;
        }
    }
}

//块内有序，块间未必
template<typename T>
inline void mergeBlocks(Block<T> &block1, Block<T> &block2, Block<T> &new_prev_block) {
    int cur = 0, cur1 = 0, cur2 = 0;
    while (cur1 < block1.KeyValue_num || cur2 < block2.KeyValue_num) {
        if (cur1 == block1.KeyValue_num) {
            new_prev_block.KeyValues[cur++] = block2.KeyValues[cur2++];
        } else if (cur2 == block2.KeyValue_num) {
            new_prev_block.KeyValues[cur++] = block1.KeyValues[cur1++];
        } else {
            if (block1.KeyValues[cur1].key > block2.KeyValues[cur2].key || (block1.KeyValues[cur1].key == block2.KeyValues[cur2].key && block1.KeyValues[cur1].value > block2.KeyValues[cur2].value)) {
                new_prev_block.KeyValues[cur++] = block2.KeyValues[cur2++];
            } else {
                new_prev_block.KeyValues[cur++] = block1.KeyValues[cur1++];
            }
        }
    }
    new_prev_block.KeyValue_num = cur;
}

//Blocklist类
template<typename T>
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
        if (!file.is_open()) {//创建文件
            index_file.initialise();
            block_file.initialise();
            init_index.offset = block_file.write(init_block);//写入第一个block
            index_file.write_info(1, 1);
            index_file.write_info(index_file.write(init_index), 2);//写入偏移量
        }
    }
    ~Blocklist() = default;
    void clearall() {
        index_file.clear();
        block_file.clear();
    }
    void Insert(const std::string &key, const T &value) {
        Key_Value KeyValue(key, value);
        Index index;
        Block<T> block;
        //获取索引文件中的Index数量
        int block_pos = 0, index_number;
        index_file.get_info(index_number, 1);
        //获取索引文件中的当前位置
        int index_pos = 0;
        index_file.get_info(index_pos, 2);

        for (int i = 0; i < index_number; ++i) {
            index_file.read(index, index_pos);
            // 插入键值对
            if (index.end_key >= key || index.next_offset == -1) {
                block_pos = index.offset;
                block_file.read(block, block_pos);
                // 若Block满了就需要裂块
                if (block.KeyValue_num == BLOCK_SIZE) {
                    Block<T> new_block;
                    Index new_index;
                    splitBlock(block, new_block);//裂块
                    // 插入键值对
                    if (cmp(KeyValue, new_block.KeyValues[0])) {
                        insertKeyvalue(new_block, KeyValue);
                    } else {
                        insertKeyvalue(block, KeyValue);
                    }

                    // 更新原数据快
                    block_file.update(block, block_pos);
                    // 更新数据块的键值对数量
                    index.KeyValue_num = block.KeyValue_num;
                    new_index.KeyValue_num = new_block.KeyValue_num;
                    // 更新开始结束键
                    new_index.beg_key = new_block.KeyValues[0].key;
                    new_index.end_key = new_block.KeyValues[new_block.KeyValue_num - 1].key;
                    index.beg_key = block.KeyValues[0].key;
                    index.end_key = block.KeyValues[block.KeyValue_num - 1].key;
                    
                    // 更新链表
                    new_index.offset = block_file.write(new_block);
                    new_index.next_offset = index.next_offset;
                    index.next_offset = index_file.write(new_index);

                    // 写回更新后的原索引
                    index_file.update(index, index_pos);

                    // 更新索引文件中存储的索引数量，增加一个新的索引
                    index_file.write_info(index_number + 1, 1);
                    break;
                } else {
                    insertKeyvalue(block, KeyValue);
                    index.KeyValue_num = block.KeyValue_num;
                    if (block.KeyValues[block.KeyValue_num - 1].key != index.end_key || block.KeyValue_num == 1) {
                        index.end_key = block.KeyValues[block.KeyValue_num - 1].key;
                    }
    
                    if (block.KeyValues[0].key != index.beg_key || block.KeyValue_num == 1) {
                        index.beg_key = block.KeyValues[0].key;
                    }
    
                    index_file.update(index, index_pos);
                    block_file.update(block, block_pos);
                    break;
                }
            } else {
                index_pos = index.next_offset;
            }
        }
    }
    void DeleteKeyValue(const std::string &key, const T &value) {
        Key_Value KeyValue(key, value);
        Index index;
        Block<T> block;
        int index_pos = 0;
        index_file.get_info(index_pos, 2);
        int prev_pos = -1, prev_KeyValue_num = -1, index_number;
        index_file.get_info(index_number, 1);
    
        for (int i = 0; i < index_number; ++i) {
            index_file.read(index, index_pos);
            //查询直到比自己大的key
            if (index.beg_key > key) {
                return;
            }
            //如果在这个block内
            if (index.end_key >= key) {
                block_file.read(block, index.offset);
                for (int j = 0; j < block.KeyValue_num; ++j) {
                    if ((block.KeyValues[j].value == KeyValue.value) && (KeyValue.key == block.KeyValues[j].key)) {
                        //删除，键值块前移
                        for (int k = j; k < block.KeyValue_num - 1; ++k) {
                            block.KeyValues[k] = block.KeyValues[k + 1];
                        }
                        block.KeyValue_num--;
                        //如果删了唯一元素
                        if (block.KeyValue_num == 0) {
                            if (prev_pos != -1) {
                                Index prev_index;
                                index_file.read(prev_index, prev_pos);
                                prev_index.next_offset = index.next_offset;
                                index_file.update(prev_index, prev_pos);
                            } else {
                                index_file.write_info(index.next_offset, 2);
                            }
                            index_file.write_info(index_number - 1, 1);
                        }
                        //与前一个块合并：减少读写次数
                        else if (prev_pos != -1 && block.KeyValue_num <= BLOCK_SIZE / 2 && prev_KeyValue_num <= BLOCK_SIZE / 2) {
                            Index prev_index;
                            Block<T> prev_block;

                            index_file.read(prev_index, prev_pos);
                            block_file.read(prev_block, prev_index.offset);

                            prev_index.next_offset = index.next_offset;
                            
                            //并块
                            Block<T> new_prev_block;
                            
                            mergeBlocks(block, prev_block, new_prev_block);
                            prev_index.KeyValue_num = new_prev_block.KeyValue_num;

                            prev_index.beg_key = new_prev_block.KeyValues[0].key;
                            prev_index.end_key = new_prev_block.KeyValues[prev_index.KeyValue_num - 1].key;

                            index_file.update(prev_index, prev_pos);
                            block_file.update(new_prev_block, prev_index.offset);
                            index_file.write_info(index_number - 1, 1);
                        }
                        else {
                            if (block.KeyValues[0].key != index.beg_key) {
                                index.beg_key = block.KeyValues[0].key;
                            }
                            if (block.KeyValues[block.KeyValue_num - 1].key != index.end_key) {
                                index.end_key = block.KeyValues[block.KeyValue_num - 1].key;
                            }
                            index.KeyValue_num = block.KeyValue_num;
                            index_file.update(index, index_pos);
                            block_file.update(block, index.offset);
                        }
                        return;
                    }
                }
            }
            //查询下一个节点
            prev_pos = index_pos;
            index_pos = index.next_offset;
            prev_KeyValue_num = index.KeyValue_num;
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

            if (index.beg_key > key) {
                break;
            }

            if (index.end_key >= key) {
                block_file.read(block, index.offset);
                int l = 0, r = block.KeyValue_num - 1;
                while (l < r) {
                    int mid = l + (r - l) / 2; 
                    if (block.KeyValues[mid].key < key) {
                        l = mid + 1; 
                    } else {
                        r = mid;
                    }
                }
                if (block.KeyValues[l].key != key) {
                    l++;
                }
                //加入所有数据
                while (l < block.KeyValue_num && (block.KeyValues[l].key == key)) {
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
        auto result = FindKey(key);
        auto it = result.begin();
        return *it;
    }
};

#endif 