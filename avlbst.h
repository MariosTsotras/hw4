#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here

    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);

    void removeFix(AVLNode<Key,Value>* n, int diff);

    void rotateRight(AVLNode<Key,Value>* y);
    void rotateLeft(AVLNode<Key,Value>* x);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    
    if (static_cast<AVLNode<Key,Value>*>(this->root_) == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key,Value>*>(this->root_);
    Key insertKey = new_item.first;
    int balanceUpdate = 0;
    while (temp != NULL) {
        Key tempKey = temp->getKey();
        if (insertKey > tempKey) { //go right
            if (temp->getRight() != NULL) {
                temp = temp->getRight();
            } else {
                temp->setRight(new AVLNode<Key, Value>(new_item.first, new_item.second, temp));
                balanceUpdate = 1;
                break;
            }
        } else if (insertKey < tempKey) { //go left
            if (temp->getLeft() != NULL) {
                temp = temp->getLeft();
            } else {
                temp->setLeft(new AVLNode<Key, Value>(new_item.first, new_item.second, temp));
                balanceUpdate = -1;
                break;
            }
        } else { //update value
            temp->setValue(new_item.second);
            return;
        }
    }

    if (temp->getBalance() == 0) {
        temp->setBalance(balanceUpdate);
        if (balanceUpdate == 1) { //right leaning child
            insertFix(temp, temp->getRight());
        } else { //left leaning child
            insertFix(temp, temp->getLeft());
        }
        return;
    }
    temp->setBalance(0);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix (AVLNode<Key,Value>* p, AVLNode<Key,Value>* n) {
    if (p == NULL || p->getParent() == NULL) {
        return;
    }
    AVLNode<Key,Value>* g = p->getParent();
    int diff = 0;
    bool isLeft = false;
    if (g->getLeft() == p) { //p is left of g
        diff = -1;
        isLeft = true;
    } else { //p is right of g
        diff = 1;
        isLeft = false;
    }

    // std::cout << g->getBalance() << std::endl;
    g->updateBalance(diff);
    // std::cout << g->getBalance() << std::endl;
    if (g->getBalance() == 0) {
        return;
    } else if (abs (g->getBalance()) == 1) {
        insertFix(g, p);
    } else {
        if (p->getBalance() == diff) {//zig-zig
            if (isLeft) {
                rotateRight(g);
            } else {
                rotateLeft(g);
            }
            g->setBalance(0);
            p->setBalance(0);
            return;
        } else if (p->getBalance() == -1*(diff)) { //zig-zag
            if (isLeft) {
                rotateLeft(p);
                rotateRight(g);
                
            } else {
                rotateRight(p);
                rotateLeft(g);
            }

            if (n->getBalance() == diff) { //Case 3a
                p->setBalance(0);
                g->setBalance(-(diff));
            } else if (n->getBalance() == 0) { //Case 3b
                p->setBalance(0);
                g->setBalance(0);
            } else { //balance of n is 1  //Case 3c
                p->setBalance(diff);
                g->setBalance(0);
            }
            n->setBalance(0);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* y) {
    if (y == NULL) {
        return;
    }
    if (y->getLeft() == NULL) {
        return;
    }

    AVLNode<Key, Value>* x = y->getLeft();
    AVLNode<Key, Value>* b = x->getRight(); //x's right

    if (static_cast<AVLNode<Key,Value>*>(this->root_) == y) { //update y's parent (root)
        this->root_ = x;
    } else { //update y's parent (child)
        if (y->getParent()->getLeft() == y) { //y is parent's left child
            y->getParent()->setLeft(x); 
        } else { //y is parent's right child
            y->getParent()->setRight(x);
        }
    }
    x->setParent(y->getParent()); //x's parent is y's parent
    y->setLeft(b); //y's left is x's right
    if (b != NULL) { //x's right's parent is y
        b->setParent(y);
    }
    x->setRight(y); //x's right is y
    y->setParent(x); //y's parent is x

}
    
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* x) {
    if (x == NULL) {
        return;
    }
    if (x->getRight() == NULL) {
        return;
    }

    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* b = y->getLeft(); //y's left

    if (static_cast<AVLNode<Key,Value>*>(this->root_) == x) { //update y's parent (root)
        this->root_ = y;
    } else { //update x's parent (child)
        if (x->getParent()->getLeft() == x) { //x is parent's left child
            x->getParent()->setLeft(y); 
        } else { //x is parent's right child
            x->getParent()->setRight(y);
        }
    }
    y->setParent(x->getParent()); //x's parent is y's parent
    x->setRight(b); //x's right is y's right
    if (b != NULL) { 
        b->setParent(x); //y's left's parent is x
    }
    y->setLeft(x); //y's left is x
    x->setParent(y); //x's parent is y
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (temp == NULL) {
        return;
    }

    if (temp->getLeft() != NULL && temp->getRight() != NULL) { //if both children exist
        this->nodeSwap(temp, static_cast<AVLNode<Key, Value>*>(this->predecessor(temp))); //swap node with its predecessor
    }

    AVLNode<Key, Value>* p = temp->getParent(); //LET p = parent(n)
    int diff;
    
    if (p != NULL) { //If p is not NULL 
        if (temp == p->getLeft()) { //If temp is left child of parent
            diff = 1;
        } else  { //temp is right child of parent
            diff = -1;
        }
    }
    

    if (temp->getLeft() != NULL) { //delete temp and update pointers
        this->leftChildRemove(temp);
    } else if (temp->getRight() != NULL) {
        this->rightChildRemove(temp);
    } else { //no children
        this->noChildRemove(temp);
    }

    removeFix(p, diff);
    
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int diff) {
    if (n == NULL) {
        return;
    }
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff;
    if (p != NULL) {
        if (n == p->getLeft()) {
            ndiff = 1;
        } else {
            ndiff = -1;
        }
    }

    if (diff == -1) {
        if (n->getBalance() + diff == -2) {
            AVLNode<Key, Value>* c = n->getLeft();
            if (c->getBalance() == -1) {
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            } else if (c->getBalance() == 0) {
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            } else {
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);

                if (g->getBalance() == 1) {
                    n->setBalance(0);
                    c->setBalance(-1);
                } else if (g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                } else if (g->getBalance() == -1) {
                    n->setBalance(1);
                    c->setBalance(0);
                }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        } else if (n->getBalance() + diff == 0) {
            n->setBalance(0);
            removeFix(p, ndiff);
        } else {
            n->setBalance(-1);
            return;
        }
    } else { // ndiff == 1
        if (n->getBalance() + diff == 2) {
            AVLNode<Key, Value>* c = n->getRight();
            if (c->getBalance() == 1) {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            } else if (c->getBalance() == 0) {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            } else {
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);

                if (g->getBalance() == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                } else if (g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                } else if (g->getBalance() == 1) {
                    n->setBalance(-1);
                    c->setBalance(0);
                }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        } else if (n->getBalance() + diff == 0) {
            n->setBalance(0);
            removeFix(p, ndiff);
        } else {
            n->setBalance(1);
            return;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
