//
// Created by robert on 14/9/17.
//

#ifndef B_TREE_UPDATE_H
#define B_TREE_UPDATE_H
#include <vector>
#include "../trees/b_tree.h"
using namespace std;

template <typename K, typename V>
struct update_context {
    BTree<K, V> *tree;
    typename vector<pair<K, V>>::const_iterator it_start;
    typename vector<pair<K, V>>::const_iterator it_end;
};

template <typename K, typename V>
void update_building_block(BTree<K, V> *tree, typename vector<pair<K, V>>::const_iterator it_start,
            typename vector<pair<K, V>>::const_iterator it_end) {
    typename vector<pair<K, V>>::const_iterator it = it_start;
    int count = 0;
    while (it != it_end) {
        tree->insert((*it).first, (*it).second);
        ++it;
        ++count;
    }
};

template <typename K, typename V>
void* update_worker(void* arg) {
    update_context<K, V> *context = static_cast<update_context<K, V>*>(arg);
    update_building_block(context->tree, context->it_start, context->it_end);
}

template <typename K, typename V>
void update(BTree<K, V> *tree, vector<pair<K, V>> tuples, int number_of_threads) {

    pthread_t * pids = new pthread_t[number_of_threads];
    update_context<K, V> * context = new update_context<K, V>[number_of_threads];

    typename vector<pair<K, V>>::const_iterator it = tuples.cbegin();
    int tuple_per_thread = tuples.size() / number_of_threads;

    for (int i = 0; i < number_of_threads; ++i) {
        context[i].tree = tree;
        context[i].it_start = tuples.cbegin() + i * tuple_per_thread;
        context[i].it_end = (i == (number_of_threads - 1)) ? tuples.cend() : tuples.cbegin() + (i + 1) * tuple_per_thread;
        pthread_create(&pids[i], NULL, update_worker<K, V>, &context[i]);
    }

    for (int i = 0; i < number_of_threads; ++i) {
        pthread_join(pids[i], NULL);
    }
    delete[] pids;
    delete[] context;

};
#endif //B_TREE_UPDATE_H
