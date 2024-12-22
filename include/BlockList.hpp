#ifndef BLOCKLIST_hpp
#define BLOCKLIST_hpp

#include "MemoryRiver.hpp"
#include <iostream>
#include <cstring>
#include <set>


constexpr int MAX_STRING = 65;
constexpr int BLOCK_SIZE = 1000;
const std::string index_filename = "index.dat";
const std::string value_filename = "values.dat";

struct Key_Value {//存储键值对
    char key[MAX_STRING];
    int value;

    Key_Value() = default;

    Key_Value(const std::string &key, const int value) : value(value) {
        strcpy(this->key, key.c_str());
    }
};

struct Index {//存储index
    int offset;//记录偏移量
    int next_offset;//记录下一block的偏移量,用-1表示最后一个block
    char beg_key[MAX_STRING];//开始的key
    char end_key[MAX_STRING];//结束的key
    int KeyValue_num;//总共有多少对
    Index() : offset(0), next_offset(-1), KeyValue_num(0) {};
};

struct Block {
    Key_Value KeyValues[BLOCK_SIZE];
    int KeyValue_num;
    Block() : KeyValue_num(0) {}
};

inline void splitBlock(Block &block1, Block &block2) {//把block1裂块并把多余数据存放到block2中
    int mid = BLOCK_SIZE / 2;
    for (int j = mid; j < block1.KeyValue_num; ++j) {
        block2.KeyValues[j - mid] = block1.KeyValues[j];
        block1.KeyValues[j] = Key_Value();
        block2.KeyValue_num++;
    }
    block1.KeyValue_num = mid;
}
    
inline bool cmp(const Key_Value& Key_Value1, const Key_Value& Key_Value2) {
    if (strcmp(Key_Value1.key, Key_Value2.key) == 0) {
        return Key_Value1.value > Key_Value2.value;
    }
    else {
        return strcmp(Key_Value1.key, Key_Value2.key) > 0;
    }
}

inline void insertKeyvalue(Block &block1, Key_Value &kv) {
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
inline void mergeBlocks(Block &block1, Block &block2, Block &new_prev_block) {
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
class Blocklist {
private:
    std::string filename;
    MemoryRiver<Index, 2> index_file;//开头存储键值对数量和存储偏移量
    MemoryRiver<Block, 0> block_file;//直接开始存储block

public:
    Blocklist(const std::string base_filename = "KVDB_")
    : filename(base_filename), 
      index_file(base_filename + index_filename), 
      block_file(base_filename + value_filename) {
        Index init_index;
        Block init_block;
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

    void Insert(const std::string &key, const int &value) {
        Key_Value KeyValue(key, value);
        Index index;
        Block block;
        //获取索引文件中的Index数量
        int block_pos = 0, index_number;
        index_file.get_info(index_number, 1);
        //获取索引文件中的当前位置
        int index_pos = 0;
        index_file.get_info(index_pos, 2);

        for (int i = 0; i < index_number; ++i) {
            index_file.read(index, index_pos);
            // 插入键值对
            if (strcmp(index.end_key, key.c_str()) >= 0 || index.next_offset == -1) {
                block_pos = index.offset;
                block_file.read(block, block_pos);
                // 若Block满了就需要裂块
                if (block.KeyValue_num == BLOCK_SIZE) {
                    Block new_block;
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
                    strcpy(new_index.beg_key, new_block.KeyValues[0].key);
                    strcpy(new_index.end_key, new_block.KeyValues[new_block.KeyValue_num - 1].key);
                    strcpy(index.beg_key, block.KeyValues[0].key);
                    strcpy(index.end_key, block.KeyValues[block.KeyValue_num - 1].key);
                    
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
                    if (strcmp(block.KeyValues[block.KeyValue_num - 1].key, index.end_key) != 0 || block.KeyValue_num == 1) {
                        strcpy(index.end_key, block.KeyValues[block.KeyValue_num - 1].key);
                    }
    
                    if (strcmp(block.KeyValues[0].key, index.beg_key) != 0 || block.KeyValue_num == 1) {
                        strcpy(index.beg_key, block.KeyValues[0].key);
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
    void DeleteKeyValue(const std::string &key, const int &value) {
        Key_Value KeyValue(key, value);
        Index index;
        Block block;
        int index_pos = 0;
        index_file.get_info(index_pos, 2);
        int prev_pos = -1, prev_KeyValue_num = -1, index_number;
        index_file.get_info(index_number, 1);
    
        for (int i = 0; i < index_number; ++i) {
            index_file.read(index, index_pos);
            //查询直到比自己大的key
            if (strcmp(index.beg_key, key.c_str()) > 0) {
                return;
            }
            //如果在这个block内
            if (strcmp(index.end_key, key.c_str()) >= 0) {
                block_file.read(block, index.offset);
                for (int j = 0; j < block.KeyValue_num; ++j) {
                    if ((block.KeyValues[j].value == KeyValue.value) && (strcmp(KeyValue.key, block.KeyValues[j].key) == 0)) {
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
                            Block prev_block;

                            index_file.read(prev_index, prev_pos);
                            block_file.read(prev_block, prev_index.offset);

                            prev_index.next_offset = index.next_offset;
                            
                            //并块
                            Block new_prev_block;
                            
                            mergeBlocks(block, prev_block, new_prev_block);
                            prev_index.KeyValue_num = new_prev_block.KeyValue_num;

                            strcpy(prev_index.beg_key, new_prev_block.KeyValues[0].key);
                            strcpy(prev_index.end_key, new_prev_block.KeyValues[prev_index.KeyValue_num - 1].key);

                            index_file.update(prev_index, prev_pos);
                            block_file.update(new_prev_block, prev_index.offset);
                            index_file.write_info(index_number - 1, 1);
                        }
                        else {
                            if (strcmp(block.KeyValues[0].key, index.beg_key) != 0) {
                                strcpy(index.beg_key, block.KeyValues[0].key);
                            }
                            if (strcmp(block.KeyValues[block.KeyValue_num - 1].key, index.end_key) != 0) {
                                strcpy(index.end_key, block.KeyValues[block.KeyValue_num - 1].key);
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
    void FindKey(const std::string &key) {
        int start = 0, index_number = 0;
        std::set <int> result;
        index_file.get_info(start, 2);
        index_file.get_info(index_number, 1);
        int index_pos = start;
        Index index;
        Block block;
        bool if_found = false;
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
                    if_found = true;
                    result.insert(block.KeyValues[l].value);
                    l++;
                }
            }
            index_pos = index.next_offset;
        }
    
        if (!if_found) {
            std::cout << "null";
        }
        else {
            for (auto it = result.begin(); it != result.end(); it++) {
                std::cout << *it << ' ';
            }
        }
        std::cout << std::endl;
    }
};

#endif 