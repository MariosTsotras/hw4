#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int height(Node* root) {
    if (root == NULL) {
        return 0;
    }
    int leftH = height(root->left);
    int rightH = height(root->right);
    if (leftH > rightH) {
        return (1 + leftH);
    } else {
        return (1 + rightH);
    } 
}

bool equalPaths(Node * root)
{
    // Add your code below
    if (root == nullptr) {
		return true;
	}
    int leftHeight = height(root->left);
    int rightHeight = height(root->right);
	if (leftHeight == rightHeight || root->left == NULL || root->right == NULL) { //if the heights of each subtree are the same or a subtree on either side (exclusively) is missing, then we can check further for equal paths
		return equalPaths(root->left) && equalPaths(root->right);
	}
	else { //if the heights are not equal and both subtrees exist
		return false;
	}
}

