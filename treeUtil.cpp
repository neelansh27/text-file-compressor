#include "treeUtil.h"
#include <iostream>

void writeChar(std::ostream& out, char ch){
      switch (ch)
      {
         case '\'':
            out << "\\'";
            break;

         case '\"':
            out << "\\\"";
            break;

         case '\?':
            out << "\\?";
            break;

         case '\\':
            out << "\\\\";
            break;

         case '\a':
            out << "\\a";
            break;

         case '\b':
            out << "\\b";
            break;

         case '\f':
            out << "\\f";
            break;

         case '\n':
            out << "\\n";
            break;

         case '\r':
            out << "\\r";
            break;

         case '\t':
            out << "\\t";
            break;

         case '\v':
            out << "\\v";
            break;

         default:
            out << ch;
      }
}
void printBT(const std::string& prefix, const Node* node, bool isLeft)
{
    if( node != nullptr )
    {
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──" );

        // print the value of the node
        writeChar(std::cout,node->ch);
        std::cout << ":" << node->freq << std::endl;

        // enter the next tree level - left and right branch
        printBT( prefix + (isLeft ? "│   " : "    "), node->left, true);
        printBT( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}

void printBT(const Node* node)
{
    printBT("", node, false);    
}

