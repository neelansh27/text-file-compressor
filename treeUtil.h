#ifndef Print_BT
#define Print_BT

#include <iostream>
#include <string>


class Node {
public:
  int freq;
  char ch;
  Node* left;
  Node* right;

  Node(): freq(0),ch('\0'),left(nullptr), right(nullptr) {};
  // returns true if node is less than comparing node
  bool lessThan(Node b) {
    return freq < b.freq;
  }
};

void printBT(const std::string& prefix, const Node* node, bool isLeft);
void printBT(const Node* node);
void writeChar(std::ostream& out, char &ch);
#endif // printBT
