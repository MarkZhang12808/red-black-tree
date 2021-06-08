/*
13 May 2021 - Mark Zhang - Red Black Tree Part 1 + 2: 
Red Black Tree Part 1: Insertion - creates a red black tree from user input, user can choose to manually enter numbers or create the tree using numbers in a file
Red Black Tree Part 2: Deletion - added search and delete functionality
REFERENCES FOR INSERTION:
  Videos on Canvas
  https://courses.cs.duke.edu/spring05/cps130/lectures/skiena.lectures/lecture10.pdf 
  http://pages.cs.wisc.edu/~cs400/readings/Red-Black-Trees/ 
  https://www.cs.auckland.ac.nz/software/AlgAnim/red_black.html
REFERENCES FOR DELETION:
  https://web.eecs.umich.edu/~sugih/courses/eecs281/f11/lectures/11-Redblack.pdf
  http://www.cs.otago.ac.nz/cosc242/pdf/L17.pdf
*/
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

//each node in a binary tree can be connected to up to 2 child nodes and a parent node, represented in this node struct

struct node{
    int data;
    bool isRed = true;
    node* left = NULL;
    node* right = NULL;
    node* parent = NULL;
};

//function declarations

void printTree(node* root, int spacing);
void addNode(node* root, node* newNode);
void leftRotate(node*&root, node* n);
void rightRotate(node*&root, node* n);
void insert(node*&root, node*n);
node* searchTree(node* root, int a);
bool deleteNode(node*&root, node*n);
node* findSuccessor(node* n);
node* findSibling(node* n);
//cases where 1 child of sibling is red
void ll(node*&root, node* n);
void lr(node*&root, node* n);
void rr(node*&root, node* n);
void rl(node*&root, node* n);
//percolates upwards to the root of the tree, removes "double black" node, fixes red-black tree properties
void percolate(node*&root, node* n);

int main(){
    bool running = true;
    while(running){ //allows the user to empty and recreate the tree as many times as they wish
        node* root;
        bool rootInitialized = false;
        bool treeSession = true;
        while(treeSession == true){ //takes commands to edit 1 tree, when all nodes in the tree are deleted, everything is re-initialized to start a new tree session
            cout << "Enter a command: [add] a series of numbers, [read] from a file, [print] tree, [search] for a node, [delete] nodes, or [quit] the program" << endl;
            char* input = new char[10];
            int* array = new int[100];
            int index = 0;
            bool addCommand = false;
            cin.clear();
            cin.getline(input, 10);
            if(strcmp(input, "add") == 0){
                char* temp = new char[500];
                cout << "enter integers separated with spaces" << endl;
                cin.getline(temp, 500);
                char* token = strtok(temp, " ");
                while(token != NULL){
                    array[index] = atoi(token);
                    token = strtok(NULL, " ");
                    index++;
                }
                addCommand = true;
            }
            else if(strcmp(input, "read") == 0){
                cout << "enter file name (ex: numbers.txt) containing space-separated numbers" << endl;
                string fileName;
                cin.clear();
                cin >> fileName;
                fstream file;
                int temp = 0;
                file.open(fileName, ios::in);
                while(file >> temp){
                    array[index] = temp;
                    index++;
                }
                addCommand = true;
            }
            else if(strcmp(input, "print") == 0){
                printTree(root, 0);
            }
            else if(strcmp(input, "search") == 0){
                int temp = 0;
                cout << "enter number to search" << endl;
                cin >> temp;
                node* n = searchTree(root, temp);
                if(n == NULL){
                    cout << "node could not be found" << endl;
                }
                else{
                    cout << "node found: " << n->data << endl;
                    cout << "parent: " << n->parent->data << endl;
                    if(n->left != NULL){
                        cout << "left child: " << n->left->data << endl;
                    }
                    if(n->right != NULL){
                        cout << "right child: " << n->right->data << endl;
                    }
                }
            }
            else if(strcmp(input, "delete") == 0){ //calls delete function by allowing user to enter a series of numbers
                char* temp = new char[500];
                cout << "enter integers separated with spaces" << endl;
                cin.getline(temp, 500);
                char* token = strtok(temp, " ");
                while(token != NULL){
                    array[index] = atoi(token);
                    token = strtok(NULL, " ");
                    index++;
                }
                bool treeNotEmpty = true;
                int i = 0;
                while(i < index){
                    node* x = searchTree(root, array[i]);
                    if(x != NULL){
                        cout << "deleting: " << array[i]<< endl;
                        treeNotEmpty = deleteNode(root, x); //call delete on the node if it exists
                        if(treeNotEmpty == false){
                            cout << "all nodes deleted" << endl;
                            treeSession = false; //starts new tree when current tree is emptied
                            break;
                        }
                    }
                    else{ //if a value the user entered couldn't be found
                        cout << "node with value " << array[i] << " could not be found, attempting to delete next node" << endl;
                    }
                    i++;
                }
                delete [] array;
            }
            else if(strcmp(input, "quit") == 0){
                cout << "quitting..." << endl;
                running  = false;
                treeSession = false;
                exit(0);
            }
            else{
                cout << "please enter valid command" << endl;
            }
            if(addCommand == true){
                int i = 0;
                if(rootInitialized == false){ 
                    //INSERT CASE 1: tree is empty, root is set as a black node
                    root = new node;
                    root->data = array[0];
                    root->isRed = false;
                    i++;
                    rootInitialized = true;
                }
                while(i < index){
                    node* n = new node;
                    n->data = array[i];
                    insert(root, n);
                    i++;
                }
                delete [] array;
            }
        }
    }
    return 0;
}

void printTree(node* root, int spacing){
    //recursively prints the red black tree, r denotes a red node, b denotes a black node
    if(root == NULL){
        return;
    }
    spacing += 7;
    printTree(root->right, spacing);
    cout << endl;
    for(int i = 7; i < spacing; i++){
        cout << " ";
    }
    if(root->isRed){
        cout << "r-";
    }
    else{
        cout << "b-";
    }
    cout << root->data << '\n';
    printTree(root->left, spacing);
}

void addNode(node* root, node* n){
    //function that adds a new node in a way that keeps the in-order traversal property of a binary search tree
    //but does not update the node's color to maintain a red-black tree
    node* traverse = root;
    while(1){
        //traverses the tree until it reaches the end of the appropriate branch and adds the new node
        if(n->data >= traverse->data){
            if(traverse->right == NULL){
                traverse->right = n;
                n->parent = traverse;
                return;
            }
            else{
                traverse = traverse->right;
            }
        }
        else if(n->data < traverse->data){
            if(traverse->left == NULL){
                traverse->left = n;
                n->parent = traverse;
                return;
            }
            else{
                traverse = traverse->left;
            }
        }
    }
}

void leftRotate(node*& root, node* n){
    //function for left rotation around node n
    cout << "left rotation started" << " on node: " << n->data << endl;
    node* right = n->right;
    node* temp = right->left;
    node* parent = n->parent;
    if(n == root){
        cout << "root changed" << endl;
        root = right;    
    }
    else if(n == n->parent->left){
        n->parent->left = right;
    }
    else if(n == n->parent->right){
        n->parent->right = right;
    }
    n->parent = right;
    n->right = temp;
    if(temp != NULL) temp->parent = n;
    right->left = n;
    right->parent = parent;
    cout << "left rotation completed" << endl;
}

void rightRotate(node*&root, node* n){
    //function for right rotation around node n
    cout << "right rotation started" << " on node: " << n->data << endl;
    node* left = n->left;
    node* temp = left->right;
    node* parent = n->parent;
    if(n->parent == NULL){
        cout << "root changed" << endl;
        root = left;
        root->parent = NULL;
    }
    else if(n == n->parent->left){
        n->parent->left = left;
    }
    else if(n == n->parent->right){
        n->parent->right = left;
    }
    n->parent = left;
    n->left = temp;
    if(temp != NULL) temp->parent = n;
    left->parent = parent;
    left->right = n;
    cout << "right rotation completed" << endl;
}

void insert(node*& root, node* n){
    //function that adds the node and then maintains the properties of the red-black tree
    addNode(root, n);
    //CASE 2: parent is black, node is red, nothing happens
    if(n->parent->isRed){
        //red black tree property is violated, checks for cases 3, 4, and 5
        node* uncle = NULL;
        bool uncleIsRight = false;
        while((n != root) && (n->parent->isRed)){ //loop goes up the tree until it reaches the root and if the node it is examining has red parent
            //finds the uncle node and its location from the grandparent
            if(n->parent != root && n != root){
                if(n->parent == n->parent->parent->left){
                    uncle = n->parent->parent->right;
                    uncleIsRight = true;
                }
                else{
                    uncle = n->parent->parent->left;
                    uncleIsRight = false;
                }
            }
            else if(n == root->left){
                uncleIsRight = true;
            }
            else if(n == root->right){
                uncleIsRight = false;
            }

            //CASE 3: uncle is red
            if((uncle != NULL) && (uncle->isRed)){
                n->parent->isRed = false;
                uncle->isRed = false;
                n->parent->parent->isRed = true;
                n = n->parent->parent;
            }
            else if((uncle == NULL) || (uncle->isRed == false)){
                if(((n == n->parent->right) && (n->parent == n->parent->parent->left)) || ((n == n->parent->left) && (n->parent == n->parent->parent->right))){
                    //CASE 4: uncle is black, node and its parent are not in-line
                    if(n == n->parent->left){
                        n = n->parent;
                        rightRotate(root, n);
                    }
                    else{
                        n = n->parent;
                        leftRotate(root, n);
                    }
                }
                n->parent->parent->isRed = true;
                n->parent->isRed = false;
                    //CASE 5: uncle is black and node and its parent are aligned
                if(uncleIsRight){
                    rightRotate(root, n->parent->parent);
                }
                else{
                    leftRotate(root, n->parent->parent);
                }
            }
        }
    }
    root->isRed = false;
}

node* searchTree(node* root, int a){
    //function can be called to check the existence of a node in the tree based on its integer value
    //if the node exists, it is returned, if not, the function returns NULL
    node* traverse = new node;
    traverse = root;
    while(1){
        if(a > traverse->data){
            if(traverse->right == NULL){
                return NULL;
            }
            traverse = traverse->right;
        }
        else if(a < traverse->data){
            if(traverse->left == NULL){
                return NULL;
            }
            traverse = traverse->left;
        }
        else if(traverse->data == a){
            return traverse;
        }
    }
}

node* findSuccessor(node* n){ //function to find in-order successor to a node
    node* t = n;
    if(t->left != NULL){
        t = t->left;
        while(t->right != NULL){
            t = t->right;
        }
    }
    else if((t->left == NULL) && (t->right != NULL)){
        t = t->right;
        while(t->left != NULL){
            t = t->left;
        }
    }
    return t;
}

node* findSibling(node* n){ //finds the sibling of the given node
    if(n == n->parent->left){
        return n->parent->right;
    }
    else{
        return n->parent->left;
    }
}

bool deleteNode(node*& root, node* n){ //function that deletes the given node, n, with help from other functions used in specific cases
    if(n == root && root->right == NULL && root->left == NULL){ //only 1 node in the tree -> delete root, call for new tree
        root = NULL;
        delete root;
        return false;
    }
    if(n->left != NULL && n->right != NULL){ //if node has 2 children, find successor, replace data, and point to it
        cout << "finding successor" << endl;
        node*s = findSuccessor(n);
        n->data = s->data;
        n = s;
    }
    //after finding successor, n can only have 1 or 0 children

    if(!n->isRed){ //if n is a black node, there are 3 cases:
        if(n->left != NULL){ //1) single child on the left must be red
            cout << "n is black, child on left" << endl;
            n->data = n->left->data;
            n = n->left;
            deleteNode(root, n);
        }
        else if(n->right != NULL){//2) single child on the right must be red
            cout << "n is black, child on right" << endl;
            n->data = n->right->data;
            n = n->right;
            deleteNode(root, n);
        }
        else if(n->right == NULL && n->left == NULL){ //3) black leaf node
        //Deleting black leaf node based on sibling:
            node* sibling = findSibling(n);
            if(n == n->parent->left){
            //sibling on the right
                if(sibling->isRed){
                //red sibling
                    cout << "red sibling on right" << endl;
                    sibling->isRed = false;
                    leftRotate(root, n->parent);
                    sibling = findSibling(n);
                    sibling->isRed = true;
                    if(n == n->parent->left){
                        n->parent->left = NULL;
                    }
                    else{
                        n->parent->right = NULL;
                    }
                    delete n;
                    return true;
                }
                if(!sibling->isRed){
                //sibling is black
                    if(sibling->right != NULL && sibling->right->isRed){
                    //sibling's right child is red
                        cout << "sibling on right, red nephew on right" << endl;
                        rr(root, n);
                    }
                    else if(sibling->left != NULL && sibling->left->isRed){
                    //if the right child is black but left is red,
                        cout << "sibling on right, red nephew on left" << endl;
                        rl(root, n);
                    }
                    else{
                    //black sibling with black children
                        cout << "black sibling on right with black/NULL children" << endl;
                        sibling->isRed = true;
                        node* p = n->parent;
                        if(n == n->parent->left){
                            p->left = NULL;
                            delete n;
                        }
                        else if(n == n->parent->right){
                            p->right = NULL;
                            delete n;
                        }
                        //n is double black, extra blackness is passed on to parent, blackness can be absorbed by red parent or rotation
                        percolate(root, p);
                    }
                }
            }
            else{
            //sibling on the left
            cout << "sibling on left" << endl;
                if(sibling->isRed){
                //red sibling
                    cout << "red sibling on left" << endl;
                    sibling->isRed = false;
                    rightRotate(root, n->parent);
                    sibling = findSibling(n);
                    sibling->isRed = true;
                    if(n == n->parent->left){
                        n->parent->left = NULL;
                    }
                    else{
                        n->parent->right = NULL;
                    }
                    delete n;
                    return true;
                }
                if(!sibling->isRed){
                //sibling is black
                    cout << "sibling is black" << endl;
                    if(sibling->left != NULL && sibling->left->isRed){
                    //sibling's left child is red
                        cout << "sibling on left, red nephew on left" << endl;
                        ll(root, n);
                    }
                    else if(sibling->left != NULL && sibling->right->isRed){
                    //if sibling's right child is red, rotate so it is on left
                        cout << "sibling on left, red nephew on right" << endl;
                        lr(root, n);
                    }
                    else{
                    //black sibling with black or NULL children
                        cout << "black sibling on left with black/NULL children" << endl;
                        sibling->isRed = true;
                        node* p = n->parent;
                        if(n == n->parent->left){
                            p->left = NULL;
                            delete n;
                        }
                        else if(n == n->parent->right){
                            p->right = NULL;
                            delete n;
                        }
                        //resolve double black node
                        percolate(root, p);
                    }
                }
            }
        }
    }
    else if(n->isRed){ //if n is red, there are 3 simple cases:
        if(n->left != NULL){ //black child on left moves into n's place
            cout << "n is red, child on left" << endl;
            n->data = n->left->data;
            n->isRed = false;
            n->left = NULL;
        }
        else if(n->right != NULL){ //black child on right moves into n's place
            cout << "n is red, child on right" << endl;
            n->data = n->right->data;
            n->isRed = false;
            n->right = NULL;
        }
        else{ //n is a red leaf node, which can simply be deleted
            cout << "n is a red leaf" << endl;
            if(n == n->parent->left){
                n->parent->left = NULL;
            }
            else if(n == n->parent->right){
                n->parent->right = NULL;
            }
            return true;
        }
    }
    return true;
}

//case specific functions for black sibling, red nephew:
void ll(node*&root, node* n){ //sibling on left, red nephew left of sibling
    cout << "ll" << endl;
    findSibling(n)->left->isRed = false;
    rightRotate(root, n->parent);
    n->parent->right = NULL;
    delete n;
}
void lr(node*&root, node* n){ //sibilng on left, red nephew right of sibling
    cout << "lr" << endl;
    findSibling(n)->left->isRed = false;
    findSibling(n)->right->isRed = true;
    leftRotate(root, findSibling(n));
    rightRotate(root, n->parent);
    n->parent->right = NULL;
    delete n;
}
void rr(node*&root, node* n){ //sibilng on right, red nephew right of sibling
    cout << "rr" << endl;
    findSibling(n)->right->isRed = false;
    leftRotate(root, n->parent);
    n->parent->left = NULL;
    delete n;
}
void rl(node*&root, node* n){ //sibling on right, red nephew left of sibling
    cout << "rl" << endl;
    findSibling(n)->left->isRed = false;
    findSibling(n)->right->isRed = true;
    rightRotate(root, findSibling(n));
    leftRotate(root, n->parent);
    n->parent->left = NULL;
    delete n;
}

void percolate(node*&root, node* n){ //percolate up the tree to remove double black node
    while(n != root && !n->isRed){
        if(n == n->parent->left){
            node* w = n->parent->right;
            if(w->isRed){
                w->isRed = false;
                n->parent->isRed = true;
                leftRotate(root, n->parent);
                w = n->parent->right;
            }
            if(( w->left == NULL || !w->left->isRed) && (w->right == NULL || !w->right->isRed)){
                w->isRed = true;
                n = n->parent;
            }
            else if(w->left->isRed){
                w->left->isRed = false;
                w->isRed = true;
                rightRotate(root, w);
                w = n->parent->right;
            }
            if(w->right->isRed){
                w->isRed = n->parent->isRed;
                n->parent->isRed = false;
                w->right->isRed = false;
                leftRotate(root, n->parent);
                break;
            }
        }
        else{
            node* w = n->parent->left;
            if(w->isRed){
                w->isRed = false;
                n->parent->isRed = true;
                rightRotate(root, n->parent);
                w = n->parent->left;
            }
            if((w->left == NULL || !w->left->isRed) && (w->right == NULL || !w->right->isRed)){
                w->isRed = true;
                n = n->parent;
            }
            else if(w->right->isRed){
                w->left->isRed = false;
                w->isRed = true;
                leftRotate(root, w);
                w = n->parent->left;
            }
            if(w->left->isRed){
                w->isRed = n->parent->isRed;
                n->parent->isRed = false;
                w->left->isRed = false;
                rightRotate(root, n->parent);
                break;
            }
        }
    }
    n->isRed = false;
}
