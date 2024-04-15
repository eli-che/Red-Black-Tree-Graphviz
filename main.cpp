#include <iostream>

#include "RBTree.hpp"

int main() {
  // Replace with your own code for learning and debugging purposes
  RBTree<int> rb;
  for (int i = 0; i < 11; ++i) {
    rb.addNode(i);
  }

  std::cout << rb.ToGraphviz() << "\n\n";
}
