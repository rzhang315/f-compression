//
// Created by Boa-Lin Lai on 4/14/18.
//
// C++ program for Huffman Coding


// refactored from

// https://cs.calvin.edu/activities/books/c++/ds/2e/WebItems/Chapter15/Huffman.pdf
// https://www.geeksforgeeks.org/greedy-algorithms-set-3-huffman-coding/
// http://www.umich.edu/~umfandsf/other/ebooks/alice30.txt

#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <bitset>

using namespace std;
map<char, string> codes;  // a map that stores character : Huffman code pair
map<char, int> freq;  // a map that stores character : frequency pair

// A Huffman tree node
struct MinHeapNode {

    // One of the input characters
    char data;
    // Frequency of the character
    unsigned freq;

    // Left and right child
    MinHeapNode *left, *right;

    MinHeapNode(char data, unsigned freq) {
        left = right = NULL;
        this->data = data;
        this->freq = freq;
    }
};

// For comparison of
// two heap nodes (needed in min heap)
struct compare {

    bool operator()(MinHeapNode* l, MinHeapNode* r)

    {
        return (l->freq > r->freq);
    }
};

// Prints huffman codes from
// the root of Huffman Tree.
void printCodes(struct MinHeapNode* root, string str)
{

    if (!root)
        return;

    if (root->data != '$')
        cout << root->data << ": " << str << "\n";

    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
}

void storeCodes(struct MinHeapNode* root, string str)
{
    if (root==NULL)
        return;
	
    if (root->data != '$')
        codes[root->data]=str;
    storeCodes(root->left, str + "0");
    storeCodes(root->right, str + "1");
}

// Create a min heap & inserts all characters of data[]
priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> minHeap;

void HuffmanCodes()
{
    struct MinHeapNode *left, *right, *top;

    for (auto v = freq.begin(); v!=freq.end(); v++)
        minHeap.push(new MinHeapNode(v->first, v->second));

    // Iterate while size of heap doesn't become 1
    while (minHeap.size() != 1) {

        // Extract the two minimum
        // freq items from min heap
        left = minHeap.top();
        minHeap.pop();

        right = minHeap.top();
        minHeap.pop();

        // Create a new internal node with
        // frequency equal to the sum of the
        // two nodes frequencies. Make the
        // two extracted node as left and right children
        // of this new node. Add this node
        // to the min heap '$' is a special value
        // for internal nodes, not used
        top = new MinHeapNode('$', left->freq + right->freq);

        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    // Print Huffman codes using
    // the Huffman tree built above
    printCodes(minHeap.top(), "");
	storeCodes(minHeap.top(), "");
}

// function iterates through the encoded string s
// if s[i]=='1' then move to node->right
// if s[i]=='0' then move to node->left
// if leaf node append the node->data to our output string
string decodefile(struct MinHeapNode* root, string s)
{
    string ans = "";
    struct MinHeapNode* curr = root;
    for (int i=0;i<s.size();i++)
    {
        if (s[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        // reached leaf node
        if (curr->left==NULL and curr->right==NULL)
        {
            ans += curr->data;
            curr = root;
        }
    }

    return ans;
}

void convertStrToBin(string source_file, string decode_file)
{
	ifstream codestream(source_file);
	
	ofstream decodestream;
	
	decodestream = ofstream(decode_file, ios::out | ios::binary); //
	
    codestream >> noskipws; // read space?
	
    char ch; 
	string code;
	string encoded_text;
	int counter = 0;
			
    for (;;) {
		
        codestream >> ch;
		
		counter++;
		
        code = codes[ch];
		
		//cout << ch << ": " << code << "\n";

		encoded_text += code;
	
        if (codestream.eof()) break;
      
    }
	
	int e_size = encoded_text.size();

	const int l_size = sizeof(unsigned long)*8; //8 bits

	cout << "Decode Finishes.\n" << l_size << "\n" ;
	
	string sub_str;
	
	for (int i=0;i<e_size;i+=l_size) {
		
		sub_str = encoded_text.substr(i, l_size);
		
		//cout << i << "i: " << sub_str << "size" << l_size << "\n";
		
		std::bitset<l_size> b_sets(sub_str);
			
		unsigned long n = b_sets.to_ulong() ;
		
		//cout << i << "i: " << b_sets << "\n";
	
		decodestream.write(reinterpret_cast<const char*>(&n), sizeof(n)) ;
		//decodestream.write((char*)&n, sizeof(unsigned long));
		
    }
	
	//decodestream.write((char*)&encoded_text, sizeof(char)*encoded_text.size());
	
	decodestream.close();
	
}

void ReadSourceFile(string filename) {

    // map<char, unsigned> m;

    ifstream codestream(filename);

    if (!codestream.is_open()) {
        cout << "Cannot open code file.\n";
        exit(1);
    }
    codestream >> noskipws; // read space?
    char ch; // a character
    for (;;) {
        codestream >> ch;
        if (codestream.eof()) return;
        freq[ch] += 1;
        //cout << ch << endl;
    }
}

string buildEncodedStr(string filename) {
    ifstream codestream(filename);

    if (!codestream.is_open()) {
        cout << "Cannot open code file.\n";
        exit(1);
    }
    codestream >> noskipws;
    char ch;
    string encodedMsg;
    while (!codestream.eof()) {
        codestream >> ch;
        encodedMsg += codes[ch];
    }
    return encodedMsg;
}


// Driver program to test above functions
int main()
{
	string source_file = "alice.txt";
	string decode_file = "alice_decode.txt";
	// string resource_file = "alice_reconstruct.binary";

    ReadSourceFile(source_file);  // frequency map is built

    HuffmanCodes();  // codes map is built

    // string inputMsg = "aaddo";
    // string outputMsg = decodefile(minHeap.top(), inputMsg);
    string encodedStr = buildEncodedStr(source_file);
    // cout << "The decoded message is " << outputMsg << endl;
    string decodedStr = decodefile(minHeap.top(), encodedStr);
	ofstream out("DecodedMessage.txt");
    out << decodedStr;
    out.close();
	// convertStrToBin(source_file, decode_file);

	//convertBinToStr(m, re_source_file, decode_file);

    return 0;
}


// This code is contributed by Aditya Goel, Refactored with STL + stream by Boa-Lin
