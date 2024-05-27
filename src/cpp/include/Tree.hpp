#pragma once

#include <deque>
#include "City.hpp"

class TreeNode {
public:
    TreeNode();
    TreeNode *& getLeft();
    TreeNode *& getRight();
    int setCity(City *& city);
    City *& getCity();

private:
    City *treeCity;
    TreeNode *left;
    TreeNode *right;
};

class Tree {
public:
    Tree();
    Tree(City *city);
    ~Tree();
    Tree(const Tree &source);
    void addToTree(City *&toAdd);
    void displayTree() const;
    void clear();
    void buildNeighborList(std::deque<City*> &neighborList, int size);
    bool operator==(const Tree &tree) const;
    bool operator!=(const Tree &tree) const;
    Tree &operator+=(City *toAdd);
    Tree &operator+=(const Tree &toAdd);
    Tree &operator=(const Tree &source);
    Tree operator+(City *toAdd) const;
    Tree operator+(const Tree &tree) const;

private:
    City *ownerCity;
    TreeNode *root;
    void copy(TreeNode *source, TreeNode *&dest);
    void insert(TreeNode *&root, City *&toAdd);
    void traverseAndDisplay(TreeNode *root) const;
    void traverseAndAdd(TreeNode *source, Tree &dest) const;
    void removeAll(TreeNode *&root);
    void traverseAndBuild(TreeNode *root, std::deque<City*> &neighborList, int size) const;
    bool isEqual(TreeNode *rootOne, TreeNode *rootTwo) const;
};

void traverseAndOutput(std::ostream &output, TreeNode *root);
