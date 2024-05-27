#include "Tree.hpp"
#include <iostream>

TreeNode::TreeNode() : treeCity(nullptr), left(nullptr), right(nullptr) {}

TreeNode *& TreeNode::getLeft() {
    return left;
}

TreeNode *& TreeNode::getRight() {
    return right;
}

int TreeNode::setCity(City *&city) {
    if (!city) return 0;
    treeCity = city;
    return 1;
}

City *& TreeNode::getCity() {
    return treeCity;
}

Tree::Tree() : ownerCity(nullptr), root(nullptr) {}

Tree::Tree(City *city) : ownerCity(city), root(nullptr) {}

Tree::~Tree() {
    clear();
}

Tree::Tree(const Tree &source) {
    copy(source.root, root);
}

void Tree::addToTree(City *&toAdd) {
    insert(root, toAdd);
}

void Tree::insert(TreeNode *&root, City *&toAdd) {
    if (!root) {
        root = new TreeNode;
        root->setCity(toAdd);
    } else if (root->getCity()->distanceTo(ownerCity) < ownerCity->distanceTo(toAdd)) {
        insert(root->getLeft(), toAdd);
    } else {
        insert(root->getRight(), toAdd);
    }
}

void Tree::displayTree() const {
    traverseAndDisplay(root);
}

void Tree::traverseAndDisplay(TreeNode *root) const {
    if (!root) return;
    traverseAndDisplay(root->getRight());
    std::cout << root->getCity()->distanceTo(ownerCity) << std::endl;
    traverseAndDisplay(root->getLeft());
}

void Tree::clear() {
    removeAll(root);
}

void Tree::removeAll(TreeNode *&root) {
    if (!root) return;
    removeAll(root->getLeft());
    removeAll(root->getRight());
    delete root;
    root = nullptr;
}

bool Tree::operator==(const Tree &tree) const {
    return isEqual(root, tree.root);
}

bool Tree::operator!=(const Tree &tree) const {
    return !isEqual(root, tree.root);
}

Tree & Tree::operator+=(City *toAdd) {
    addToTree(toAdd);
    return *this;
}

Tree & Tree::operator+=(const Tree &toAdd) {
    traverseAndAdd(toAdd.root, *this);
    return *this;
}

Tree & Tree::operator=(const Tree &source) {
    if (this != &source) {
        if (root) clear();
        copy(source.root, root);
    }
    return *this;
}

Tree Tree::operator+(City *toAdd) const {
    Tree temp = *this;
    temp.addToTree(toAdd);
    return temp;
}

Tree Tree::operator+(const Tree &tree) const {
    Tree temp = *this;
    temp.traverseAndAdd(tree.root, temp);
    return temp;
}

void Tree::copy(TreeNode *source, TreeNode *&dest) {
    if (!source) {
        dest = nullptr;
    } else {
        dest = new TreeNode;
        dest->setCity(source->getCity());
        copy(source->getLeft(), dest->getLeft());
        copy(source->getRight(), dest->getRight());
    }
}

void Tree::traverseAndAdd(TreeNode *source, Tree &dest) const {
    if (!source) return;
    traverseAndAdd(source->getLeft(), dest);
    traverseAndAdd(source->getRight(), dest);
    dest.addToTree(source->getCity());
}

void Tree::buildNeighborList(std::deque<City*> &neighborList, int size) {
    traverseAndBuild(root, neighborList, size);
}

void Tree::traverseAndBuild(TreeNode *root, std::deque<City*> &neighborList, int size) const {
    if (!root) return;
    traverseAndBuild(root->getRight(), neighborList, size - 1);
    neighborList.push_back(root->getCity());
    traverseAndBuild(root->getLeft(), neighborList, size - 1);
}

bool Tree::isEqual(TreeNode *rootOne, TreeNode *rootTwo) const {
    if ((!rootOne && rootTwo) || (rootOne && !rootTwo)) return false;
    if (!rootOne && !rootTwo) return true;
    return rootOne->getCity()->isEqual(rootTwo->getCity())
        && isEqual(rootOne->getLeft(), rootTwo->getLeft())
        && isEqual(rootOne->getRight(), rootTwo->getRight());
}

void traverseAndOutput(std::ostream &output, TreeNode *root) {
    if (!root) return;
    traverseAndOutput(output, root->getLeft());
    root->getCity()->outputId(output);
    output << std::endl;
    traverseAndOutput(output, root->getRight());
}
