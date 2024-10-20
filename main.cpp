#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include "treeUtil.h"
#include <iterator>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#define DEBUG
// A min priority queue
class MinHeap {
  private:
    int parent(int i){
      return (i-1)/2;
    }
    int left(int i){
      return 2*i+1;
    }
    int right(int i){
      return 2*i+2; 
    }

    void MinHeapify (int i){
      int smallest=i;
      int l = left(i);
      int r = right(i);
      if (l<q.size() && q[l].lessThan(q[smallest])) {
        smallest = l;
      }
      if (r<q.size() && q[r].lessThan(q[smallest])){
        smallest = r;
      }
      if (smallest!=i) {
        std::swap(q[i],q[smallest]);
        MinHeapify(smallest);
      }
    }

    void UpHeap(int i){
      int p = parent(i);
      if (p < 0) return;
      // swapping if child is less than parent
      if (q[i].lessThan(q[p])) {
        std::swap(q[p],q[i]);
        UpHeap(p);
      }
    }

    void BuildMinHeap() {
      for (int i=q.size()/2-1; i>=0;i--) { // traversing all levels except last one
        MinHeapify(i);
      }   
    }

  public:
    std::vector<Node> q;
    MinHeap(Node nodes[], int n) : q(nodes, nodes+n){
      BuildMinHeap();
    }
    void Print() {
      for (Node x: q) {
        std::cout << x.freq << ' ';
      }
      std::cout << '\n';
    }
    Node* ExtractMin() {
      if (q.size()==0){
        std::cerr << "Queue is empty\n";
        std::exit(1);
      }
      Node * node = new Node(); // Storing last address because 1st node will be swapped with last node
      node->freq=q.front().freq;
      node->ch=q.front().ch;
      node->left=q.front().left;
      node->right=q.front().right;
      q[0] = q.back();
      q.pop_back();
      MinHeapify(0);
      return node;
    };

    void InsertNode(Node *node) {
      q.push_back(*node);
      UpHeap(q.size()-1);
    }
    int size() {return q.size();}
};

Node* BuildHuffmanTree(MinHeap &h, int size) {
  for (int i=0;i<size-1;i++){
    Node *x = h.ExtractMin();
    Node *y = h.ExtractMin();
    Node *z = new Node();
    z->freq = x->freq + y->freq;
    z->left=x;
    z->right=y;

    h.InsertNode(z);
  }
  return h.ExtractMin();
}

void GenerateEncodings(std::string prefix, Node *node, std::unordered_map<char, std::string> &encMap) {
  if (node->left==nullptr && node->right==nullptr){
    encMap[node->ch] = prefix;
    return;
  }
  if (node->left!=nullptr) GenerateEncodings(prefix+"0", node->left, encMap);
  if (node->right!=nullptr) GenerateEncodings(prefix+"1", node->right, encMap);
};

std::unordered_map<char, int> freq_map(std::ifstream &f){
  std::unordered_map<char,int> freq;
  std::vector<char> buffer(1024);
  while (f.read(buffer.data(), buffer.size()) || f.gcount()){
    for (int i=0; i<f.gcount();i++){
      freq[buffer[i]]+=1;
    }
  }
  return freq;
}

std::string encodeText(std::string &filename, std::unordered_map<char,std::string> &encMap) {
  std::ifstream f(filename);
  std::vector<char>buff(1024);
  std::string enc;
  while (f.read(buff.data(), buff.size()) || f.gcount()) {;
    for (int i=0;i<f.gcount();i++){
      enc+=encMap[buff[i]];
    }
  }
  f.close();
  return enc;
}

void storeTree(std::string &s,Node *node){
  if (node->left==nullptr && node->right==nullptr){
    s+="1"+std::bitset<8>(node->ch).to_string();
    return;
  } else {
    s+='0';
    storeTree(s, node->left);
    storeTree(s, node->right);
  }
};

Node* constructTree(std::string &s, int *p, int &treeEnd){
    Node *node = new Node();
    if (s[*p]=='1') {
      std::bitset<8> bin(0);

      for (int i=1;i<9 && *p+i<treeEnd;i++){
        if (s[*p+i]=='0'){
          bin[8-i]=0;
        } else {
          bin[8-i]=1;
        };
      }
      node->ch=(char)bin.to_ulong();
      *p+=9;
    } else if (*p+1<treeEnd) {
      *p+=1;
      node->left = constructTree(s, p, treeEnd);
      node->right = constructTree(s, p, treeEnd);
    }
    return node;
}

void dumpEncoding(std::ofstream &out, std::string &s){
  const char * const c = s.c_str();
  std::vector<unsigned char> bytes_code;
  int pos;
  for (pos=0; pos/8 < s.size()/8;pos+=8) {
    unsigned byte = 0x00;
    for (auto b=0; b<8; b++) {
      byte=byte<<1;
      if (*(c+b+pos)=='1') {
        byte|=0x01;
      }
    }
    bytes_code.push_back(byte);
  }
  // checking if bits are remaining
  if (pos<s.size()) {
    unsigned byte =0x00;
    for (auto b=pos;b<pos+8;b++){
      byte=byte<<1;
      if (b<s.size() && *(c+b)=='1'){
        byte |=0x01;
      }
    }
    bytes_code.push_back(byte);
  }
  copy(
      bytes_code.begin(),
      bytes_code.end(),
      std::ostream_iterator<unsigned char>(out));
}

std::string binarytoString(std::string &filename, int max_per_cycle) {
  std::ifstream input(filename,std::ios::binary);
  if(!input.is_open()) {
    std::cerr << "COuld not open file";
    exit(1);
  }
  char * const buffer = new char[max_per_cycle];
  std::vector<unsigned char> container(max_per_cycle);
  std::stringstream byte_stream;

  const auto transform_to_string = [&byte_stream](unsigned char byte){
    byte_stream << std::bitset<8>(byte).to_string();
  };

  while( input.read(buffer, max_per_cycle) || input.gcount()){;
    container.insert(container.begin(), buffer, buffer + input.gcount());
    std::for_each(container.begin(),container.begin()+input.gcount(),transform_to_string);
    memset(buffer, 0, max_per_cycle);
  }

  delete[] buffer;
  input.close();
  return byte_stream.str();
}
// TODO: remove main later

int main(int argc, char *argv[]) {
  if (argc<2) {
    std::cerr << "Pass filename\n";
    return 1;
  }

  // ENCODING
  std::cout << "========================= Starting to Encode =====================\n\n";
  std::string input = argv[1];
  std::ifstream f(input);
  std::string output("Output.bin");
  if (!f.is_open()) {
    std::cerr << "Error opening file\n";
    return 1;
  }
  std::unordered_map<char,int> freq = freq_map(f);
  Node nodes[freq.size()];
  
  int i=0;
  for (const auto &[k,v]: freq){
    // Inserting values in Nodes
    nodes[i].ch = k;
    nodes[i].freq=v;
    i++;
  }
  MinHeap h(nodes, freq.size());
  Node *hTree = BuildHuffmanTree(h, h.size());

#ifdef DEBUG
  std::cout << "<------------------- Huffman Tree -------------------->\n";
  printBT(hTree);
#endif // DEBUG

  std::unordered_map<char,std::string> encMap;

  GenerateEncodings("", hTree, encMap);
  
#ifdef DEBUG
  std::cout << "\n<------------------- Code Words -------------------->\n";
  for(const auto &[k,v] : encMap) {
    std::cout << k << ":" << v << "\n";
  }
#endif // DEBUG

  std::string treeEncoding;
  storeTree(treeEncoding, hTree);
  
#ifdef DEBUG
  std::cout << "\n<------------------- Encoded Tree -------------------->\n";
  std::cout << "Length: "<< treeEncoding.size() << " " << std::bitset<16>(treeEncoding.size()) << "\n";
  std::cout << treeEncoding << std::endl;
#endif // DEBUG
  
  std::string textEncoding = encodeText(input, encMap);
  std::bitset<16> treeSize(treeEncoding.size());
  std::ofstream out(output,std::ios::binary);
  std::string final = treeSize.to_string() + treeEncoding + textEncoding;
  int paddingSize = 8-final.size()%8;
  final = std::bitset<8>(paddingSize).to_string() + final;
  /* std::string final = treeSize.to_string() + treeEncoding; */
  dumpEncoding(out, final);

  f.close();
  out.close();

#ifdef DEBUG
  std::cout << "\n<------------------- Final Encoding -------------------->\n";
  std::cout << final << "\n";
  std::cout << "Padding: " << paddingSize << '\n';
#endif // DEBUG


  // DECODING
  std::cout << "\n========================= Starting to Decode =====================\n\n";
  std::string decoded = binarytoString(output,512);
#ifdef DEBUG
  std::cout << "\n<------------------- Decoded Bits -------------------->\n";
  std::cout << decoded << "\n";
#endif // DEBUG
    

  // Getting size of tree from first 2 bits.
  int decodedPaddingSize = 0;
  for (int i=7;i>-1;i--){
    if (decoded[i]=='1') {
      decodedPaddingSize+=std::pow(2,7-i);
    }
  }

  int decodedTreeSize = 0;
  for (int i=23; i>7; i--) {
    if (decoded[i]=='1') {
      decodedTreeSize += std::pow(2, 23-i);
    }
  }

  std::cout << "Size of Decoded Tree: " << decodedTreeSize << "\n";



  // 2. Constructing tree from the tree bits.
  // 3. Reconstructing text and outputting it to the file

  int pos = 24;
  int treeEnd = pos+decodedTreeSize;
  Node *root;
  root = constructTree(decoded, &pos, treeEnd);
#ifdef DEBUG
  /* printBT(hTree); */
  std::cout <<"\n<------------------- Reconstructed Tree -------------------->\n";
  printBT(root);
#endif // DEBUG 

  Node* temp = root;
  int curr = 24+decodedTreeSize;
  std::string outname = "decomp-"+ (std::string)argv[1];
  std::ofstream dump(outname);
  
#ifdef DEBUG
  std::cout<<  "Decoded Padding: " << decodedPaddingSize<< "\n";
  std::cout<<  "Curr: " << curr << '\n';
#endif // DEBUG

  int textSize = decoded.size() - decodedPaddingSize + 1;
  while (curr<textSize && temp!=nullptr){
    if  (temp->ch!='\0'){
      dump << temp->ch;
      temp=root;
    } else {
      temp = (decoded[curr]=='0') ? temp->left: temp->right;
      curr++;
    }
  }
  dump.close();
  return 0;
}
