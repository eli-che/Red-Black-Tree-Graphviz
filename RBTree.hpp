#ifndef RBTREE_HPP
#define RBTREE_HPP

#include <string>
#include <vector>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

enum class Colour { RED, BLACK };

namespace std {
    std::string to_string(const std::string& str) { return str; }
    std::string to_string(const Colour& colour) {
        return (colour == Colour::RED) ? "RED" : "BLACK";
    }
}  
template <typename V>
class RBReader;

template <typename T>
class RBTree {
    friend RBReader<T>;

private:
    struct Node {
        Node* parent = nullptr;
        Node* leftChild = nullptr;
        Node* rightChild = nullptr;
        Colour colour = Colour::RED;
        T element = T();
    };

    Node* root = nullptr;
    Node* nilNode = nullptr;

    int GzAddNode(std::string& nodes, std::string& connections, const Node* curr,
        size_t to);
    int GzAddChild(std::string& nodes, std::string& connections,
        const Node* child, size_t from, size_t to,
        const std::string& color);
    template <typename V>
    std::string GzNode(size_t to, const V& what, const std::string& style,
        const std::string& fillColor,
        const std::string& fontColor);
    std::string GzConnection(size_t from, size_t to, const std::string& color,
        const std::string& style);

    bool check = false;
    int heightHelper(Node* z);
    void inOrderHelper(Node* roots);
    bool findHelper(Node* roots, const T& element);
    bool pathFromRootHelper(Node* roots, const T& element);
    void deleteNodeHelper(Node* p);
    void delete_fixup(Node* x);
    bool deleteNodeHelperFinder(Node* roots, const T& element);
    void RB_INSERT_FIXUP(Node* z);
    void LEFT_ROTATE(Node* z);
    void RIGHT_ROTATE(Node* z);
    bool isEmpty() const { return root == nilNode; }
    std::vector<T> path;
    std::vector<T> inOrderPath;

public:
    RBTree();
    ~RBTree();

    RBTree(const RBTree& other) = delete;
    RBTree& operator=(const RBTree& other) = delete;
    RBTree(RBTree&& other) = delete;
    RBTree& operator=(RBTree&& other) = delete;

    void rb_transplant(Node* u, Node* v);

    bool addNode(const T& element);
    bool deleteNode(const T& element);
    bool find(const T& element);

    const T& min();
    const T& max();

    std::vector<T> inOrder();
    int height();
    std::vector<T> pathFromRoot(const T& element);

    std::string ToGraphviz();
};

template <typename T>
RBTree<T>::RBTree() {
	this->nilNode = new Node;
	this->nilNode->colour = Colour::BLACK;
	this->root = nilNode;
}

template <typename T>
RBTree<T>::~RBTree() {}


template <typename T>
void RBTree<T>::rb_transplant(Node* u, Node* v) {
    if (u->parent == nilNode) {
        root = v;
    }
    else if (u == u->parent->leftChild) {
        u->parent->leftChild = v;
    }
    else {
        u->parent->rightChild = v;
    }
    v->parent = u->parent;
}

template <typename T>
bool RBTree<T>::addNode(const T& element) {
    if (find(element) == false) {
        Node* leaf = new Node;
        Node* y = nilNode;
        T k = element;
        leaf->element = k;
        leaf->colour = Colour::RED;
        leaf->leftChild = nilNode;
        leaf->rightChild = nilNode;


        

        Node* ptr;
        ptr = root;
        while (ptr != nilNode) {
            y = ptr;
            if (k > ptr->element) {
                ptr = ptr->rightChild;
            }
            else if(k < ptr->element) {
                ptr = ptr->leftChild;
            }
            else
                return false;
        }
        if (y == nilNode) {
            leaf->parent = y;
            root = leaf;
        }
        else {
            

            if (k < y->element) {
                y->leftChild = leaf;
            }
            else if(k > y->element) {
                y->rightChild = leaf;
            }
        }
        leaf->parent = y;
        RB_INSERT_FIXUP(leaf);
        return true;

    }
    return false;
}


template <typename T>
void RBTree<T>::RB_INSERT_FIXUP(Node* z) {
    Node* y;

    while (z->parent != nilNode && z->parent->parent != nilNode && z->parent->colour == Colour::RED) {


        if (z->parent == z->parent->parent->leftChild) {
            y = z->parent->parent->rightChild;

            if (y != nilNode && y->colour == Colour::RED) {
                z->parent->colour = Colour::BLACK;
                z->parent->parent->rightChild->colour = Colour::BLACK;
                
                z->parent->parent->colour = Colour::RED;
                z = z->parent->parent;

            }
            else if (z == z->parent->rightChild) {
                z = z->parent;
                LEFT_ROTATE(z);

                z->parent->colour = Colour::BLACK;
                z->parent->parent->colour = Colour::RED;

                RIGHT_ROTATE(z->parent->parent);
            }
            else {
                z->parent->colour = Colour::BLACK;
                z->parent->parent->colour = Colour::RED;

                RIGHT_ROTATE(z->parent->parent);
            }

        }


        else if (z->parent == z->parent->parent->rightChild) {
            y = z->parent->parent->leftChild;

            if (y != nilNode && y->colour == Colour::RED) {
                z->parent->colour = Colour::BLACK;
                z->parent->parent->leftChild->colour = Colour::BLACK;
                z->parent->parent->colour = Colour::RED;
                z = z->parent->parent;

            }
            else if (z == z->parent->leftChild) {
                z = z->parent;
                RIGHT_ROTATE(z);
                z->parent->colour = Colour::BLACK;
                z->parent->parent->colour = Colour::RED;

                LEFT_ROTATE(z->parent->parent);
            }
            else {

                z->parent->colour = Colour::BLACK;
                z->parent->parent->colour = Colour::RED;

                LEFT_ROTATE(z->parent->parent);
            }
        }

    }
    root->colour = Colour::BLACK;
}


template <typename T>
void RBTree<T>::LEFT_ROTATE(Node* x) {
    Node* y = x->rightChild;
	if (y == nilNode || x == nilNode)
		return;
    x->rightChild = y->leftChild;

    if (y->leftChild != nilNode) {
        y->leftChild->parent = x;
    }
    y->parent = x->parent;

    if (x->parent == nilNode) {
        root = y;
    }
    else if (x == x->parent->leftChild) {
        x->parent->leftChild = y;
    }
    else {
        x->parent->rightChild = y;
    }
    y->leftChild = x;
    x->parent = y;
}

template <typename T>
void RBTree<T>::RIGHT_ROTATE(Node* x) {
    Node* y = x->leftChild;
	if (y == nilNode || x ==nilNode)
		return;
    x->leftChild = y->rightChild;

    if (y->rightChild != nilNode) {
        y->rightChild->parent = x;
    }
    y->parent = x->parent;

    if (x->parent == nilNode) {
        root = y;
    }
    else if (x == x->parent->rightChild) {
        x->parent->rightChild = y;
    }
    else {
        x->parent->leftChild = y;
    }
    y->rightChild = x;
    x->parent = y;
}


template <typename T>
void RBTree<T>::deleteNodeHelper(Node* z) {
    Node* y = z;
    Node* x;
    Node* b;
    Colour y_original_color = y->colour;
    if (z->leftChild == nilNode) {
        x = z->rightChild;
        rb_transplant(z, z->rightChild);
    }
    else if (z->rightChild == nilNode) {
        x = z->leftChild;
        rb_transplant(z, z->leftChild);
    }
    else {


        b = z->rightChild;
        while (b->leftChild != nilNode) {
            b = b->leftChild;
        }
        y = b;

        y_original_color = y->colour;
        x = y->rightChild;
        if (y->parent == z) {
            x->parent = y;
        }
        else {
            rb_transplant(y, y->rightChild);
            y->rightChild = z->rightChild;
            y->rightChild->parent = y;
        }
        rb_transplant(z, y);
        y->leftChild = z->leftChild;
        y->leftChild->parent = y;
        y->colour = z->colour;
    }

    if (y_original_color == Colour::BLACK) {
		if(x != nilNode)
			delete_fixup(x);
		
    }
}

template <typename T>
void RBTree<T>::delete_fixup(Node* x)
{
    Node* w;
    while (x != root && x->colour == Colour::BLACK) {
        if (x == x->parent->leftChild) {
            w = x->parent->rightChild;
            if (w->colour == Colour::RED) {
                w->colour = Colour::BLACK;
                x->parent->colour = Colour::RED;
                LEFT_ROTATE(x->parent);
                w = x->parent->rightChild;
            }
			if (w != nilNode) 
			{
				if (w->leftChild->colour == Colour::BLACK && w->rightChild->colour == Colour::BLACK) {
					w->colour = Colour::RED;
					x = x->parent;
				}
				else if (w->rightChild->colour == Colour::BLACK) {
					w->leftChild->colour = Colour::BLACK;
					w->colour = Colour::RED;
					RIGHT_ROTATE(w);
					w = x->parent->rightChild;
				}
				w->colour = x->parent->colour;
				x->parent->colour = Colour::BLACK;
				w->rightChild->colour = Colour::BLACK;
				LEFT_ROTATE(x->parent);
			}
        }
        else {
            w = x->parent->leftChild;
            if (w->colour == Colour::RED) {
                w->colour = Colour::BLACK;
                x->parent->colour = Colour::RED;
                RIGHT_ROTATE(x->parent);
                w = x->parent->leftChild;
            }
			if (w != nilNode) 
			{
				if (w->rightChild->colour == Colour::BLACK && w->leftChild->colour == Colour::BLACK) {
					w->colour = Colour::RED;
					x = x->parent;
				}
				else if (w->leftChild->colour == Colour::BLACK) {
					w->rightChild->colour = Colour::BLACK;
					w->colour = Colour::RED;
					LEFT_ROTATE(w);
					w = x->parent->leftChild;
				}

				w->colour = x->parent->colour;
				x->parent->colour = Colour::BLACK;
				w->leftChild->colour = Colour::BLACK;
				RIGHT_ROTATE(x->parent);
			}
        }
		x = root;
    }
    x->colour = Colour::BLACK;
}


template <typename T>
bool RBTree<T>::deleteNodeHelperFinder(Node* roots, const T& element) {
    Node* p = roots;
    if (!p || p == nilNode) {
        return false;
    }
    if (p->element == element && p != nilNode) {
        deleteNodeHelper(p);
        return true;
    }
    if (p->element > element && p != nilNode) {
        deleteNodeHelperFinder(p->leftChild, element);

    }
    deleteNodeHelperFinder(p->rightChild, element);

}



template <typename T>
bool RBTree<T>::deleteNode(const T& element) {
    return deleteNodeHelperFinder(root, element);
}




template <typename T>
bool RBTree<T>::findHelper(Node* roots, const T& element) {
    Node* p = roots;
    if (!p || p == nilNode) {
        return false;
    }
    if (p->element == element) {
        if (p->colour == Colour::BLACK || p->colour == Colour::RED) {
            check = true;
            return true;
        }
    }
    if (p->element > element) {
        (findHelper(p->leftChild, element));

    }
    (findHelper(p->rightChild, element));

    if (check == true)
        return true;
    else
        return false;
}


template <typename T>
bool RBTree<T>::find(const T& element) {
    check = false;
    check = findHelper(this->root, element);
    if (check)
        return true;
    else
        return false;
}

template <typename T>
const T& RBTree<T>::min() {
    static T tmp;
    Node* node = this->root;

        if (node == nilNode) {
            throw std::invalid_argument("Did not find a minimum");
            return 0;
        }
        while (node->leftChild != nilNode) {
            node = node->leftChild;
        }
        tmp = node->element;
        return tmp;

}

template <typename T>
const T& RBTree<T>::max() {
    static T tmp;
    Node* node = this->root;

        if (node == nilNode) {
            throw std::invalid_argument("Did not find a maximum");
            return 0;
        }
        if (node == nilNode) {
            return 0;
        }
        while (node->rightChild != nilNode) {
            node = node->rightChild;
        }
        tmp = node->element;
        return tmp;

}


template <typename T>
void RBTree<T>::inOrderHelper(Node* roots)
{
    Node* ptr = roots;
    if (ptr != nilNode) {
        if (ptr->leftChild) {
            inOrderHelper(ptr->leftChild);
        }
		if(ptr != nilNode)
			inOrderPath.push_back(ptr->element);
        if (ptr->rightChild) {
            inOrderHelper(ptr->rightChild);
        }
    }
    else {
        return;
    }
}

template <typename T>
std::vector<T> RBTree<T>::inOrder() {
    inOrderPath.clear();
    inOrderHelper(this->root);
    return { inOrderPath };
}


template <typename T>
int RBTree<T>::heightHelper(Node* root)
{
    int left_height, right_height;

    if (root == nullptr || root == nilNode)
    {
        return -1;
    }
    else
    {
        left_height = heightHelper(root->leftChild);
        right_height = heightHelper(root->rightChild);
    }
    int total = left_height > right_height ? (left_height + 1) : (right_height + 1);
	if (abs(left_height - right_height) > 3)
		total--;
    return total;
}


template <typename T>
int RBTree<T>::height() {
    return { heightHelper(this->root) };
}


template <typename T>
bool RBTree<T>::pathFromRootHelper(Node* roots, const T& element) {

    Node* p = roots;
    if (!p || p == nilNode) {
        return false;
    }
    if (p->element == element) {
        path.push_back(p->element);
        return true;
    }
    if (p->element > element) {
        path.push_back(p->element);
        pathFromRootHelper(p->leftChild, element);

	}
	else {
		path.push_back(p->element);
		pathFromRootHelper(p->rightChild, element);
	}
    return false;
    
}

template <typename T>
std::vector<T> RBTree<T>::pathFromRoot(const T& element) {
    path.clear();
    if (pathFromRootHelper(this->root, element))
        return { path };
    else
        return{};
}

template <typename T>
std::string RBTree<T>::ToGraphviz() 
{
	std::string toReturn = std::string("digraph {\n");
	if (root != nullptr &&
		root != nilNode)
	{
		std::string nodes;
		std::string connections = "\t\"Root\" -> 0;\n";
		GzAddNode(nodes, connections, root, 0);
		toReturn += nodes;
		toReturn += connections;
	}
	toReturn += "}";
	return toReturn;
	//return "";
}

template <typename T>
int RBTree<T>::GzAddNode(std::string& nodes, std::string& connections,
    const Node* curr, size_t to) {
    size_t from = to;
    nodes += GzNode(from, curr->element, "filled",
        curr->colour == Colour::RED ? "tomato" : "black",
        curr->colour == Colour::RED ? "black" : "white");

    to = GzAddChild(nodes, connections, curr->leftChild, from, ++to, "blue");
    to = GzAddChild(nodes, connections, curr->rightChild, from, ++to, "gold");
    return to;
}

template <typename T>
int RBTree<T>::GzAddChild(std::string& nodes, std::string& connections,
    const Node* child, size_t from, size_t to,
    const std::string& color) {
    if (child != nilNode) {
        connections += GzConnection(from, to, color, "");
        to = GzAddNode(nodes, connections, child, to);
    }
    else if (child == nullptr) {
        nodes += GzNode(to, "null", "filled", "orange", "black");
        connections += GzConnection(from, to, "", "");
    }
    else {
        nodes += GzNode(to, child->element, "invis", "", "");
        connections += GzConnection(from, to, "", "invis");
    }
    return to;
}

template <typename T>
template <typename V>
std::string RBTree<T>::GzNode(size_t to, const V& what,
    const std::string& style,
    const std::string& fillColor,
    const std::string& fontColor) {
    return fmt::format(
        "\t{} [label=\"{}\", fillcolor=\"{}\", fontcolor=\"{}\", style=\"{}\"]\n",
        to, what, fillColor, fontColor, style);
}

template <typename T>
std::string RBTree<T>::GzConnection(size_t from, size_t to,
    const std::string& color,
    const std::string& style) {
    return fmt::format("\t{} -> {} [color=\"{}\" style=\"{}\"]\n", from, to,
        color, style);
}
#endif
