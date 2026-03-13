/**
 *  This file contains the contents of the huffman tree warm up and assignment, where we
 *  analyze huffman trees by constructing, manipulating, and traversing binary trees.
 */

#include "bits.h"
#include "treenode.h"
#include "huffman.h"
#include "map.h"
#include "vector.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "SimpleTest.h"  // IWYU pragma: keep (needed to quiet spurious warning)
#include "filelib.h"
using namespace std;

/**
 * Given a Queue<Bit> containing the compressed message bits and the encoding tree
 * used to encode those bits, decode the bits back to the original message text.
 *
 * You can assume that tree is a well-formed non-empty encoding tree and
 * messageBits queue contains a valid sequence of encoded bits.
 *
 * Your implementation may change the messageBits queue however you like. There
 * are no requirements about what it should look like after this function
 * returns. The encoding tree should be unchanged.
 *
 * The function takes two parameters, an EncodingTreeNode and a queue of bits, and returns
 * a string of the text that is decoded. This is done iterating through the bits and moving
 * the node down the tree. When a leaf is reached, the associated character is added to the
 * message and the node is brought back to the top of the tree to restart.
 */
string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits) {
    string message = "";
    EncodingTreeNode* root = tree;

    while (messageBits.size() != 0) {

        // move to next node/leaf
        Bit move = messageBits.dequeue();

        if (move == 0)
            tree = tree->zero;
        else
            tree = tree->one;

        // at the end, no more leaves
        if (tree->zero == nullptr && tree->one == nullptr) {

            message += tree->ch;

            // return to top of tree to restart
            tree = root;
        }
    }

    return message;

}


/**
 * Reconstruct encoding tree from flattened form Queue<Bit> and Queue<char>.
 *
 * You can assume that the queues are well-formed and represent
 * a valid encoding tree.
 *
 * Your implementation may change the queue parameters however you like. There
 * are no requirements about what they should look like after this function
 * returns.
 *
 * This function takes who parameter passed by references, a queue of bits describing
 * tree shape and a queue of characters describing the tree leaves. It recursively
 * moves down the tree shape queue and returns an encodingtreenode of the unflattened
 * structure of the tree, by first recursively makeing the left and then the right side
 * of the tree structure recursively from the leaves to the root node.
 */
EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves) {

    // base case: we're at a leaf
    if (treeShape.dequeue() == 0) {
        return new EncodingTreeNode(treeLeaves.dequeue());
    }

    // if not leaf
    EncodingTreeNode* left = unflattenTree(treeShape, treeLeaves);
    EncodingTreeNode* right = unflattenTree(treeShape, treeLeaves);
    return new EncodingTreeNode(left, right);
}

/**
 * Decompress the given EncodedData and return the original text.
 *
 * You can assume the input data is well-formed and was created by a correct
 * implementation of compress.
 *
 * Your implementation may change the data parameter however you like. There
 * are no requirements about what it should look like after this function
 * returns.
 *
 * This function decompresses its parameter, an encodedData, and returns a string
 * of the resulting decoded text. unflattenTree is called to produce an
 * EncodingTreeNode, which is then decoded by decodeText. The function deallocates
 * the tree before returning the resulting string to prevent memory leaks.
 */
string decompress(EncodedData& data) {

    EncodingTreeNode* tree = unflattenTree(data.treeShape, data.treeLeaves);
    string result = decodeText(tree, data.messageBits);
    deallocateTree(tree);

    return result;
}

/**
 * This function is a helper function to build the Huffman tree. It iterates
 * through a string, and returns a map of characters and integers corresponding
 * to how often each character appears in the text. It also checks that the input
 * contains at least two distince characters, and calls an error if this isn't true.
 */

Map<char, int> huffmanHelper(string text) {
    Map<char, int> result;
    for (char c : text)
        result[c]++;

    if (result.size() < 2)
        error("Input string doesn't contain at least two distinct characters");

    return result;
}

/**
 * Constructs an optimal Huffman coding tree for the given text, using
 * the algorithm described in lecture.
 *
 * Reports an error if the input text does not contain at least
 * two distinct characters.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 *
 * This function takes a string of text as input and builds an EncodingTreeNode
 * that is a Huffman tree. By calling the helper function to store a map of frequencies,
 * creating a priority queue of EncodingTreeNodes, and creating a map of weightings for
 * each node, we iterate through the list of characters by their priorities, to build
 * the tree from leaves up. the more frequent a character is, the more  higher up it will
 * end up in the tree, as we adjust the weightings of the nodes and leaves accordingly.
 * When only one node is left in the queue, we know that it contains the entire tree and
 * return it as the result.
 */
EncodingTreeNode* buildHuffmanTree(string text) {
    Map<char, int> frequencies = huffmanHelper(text);
    PriorityQueue<EncodingTreeNode*> priority;
    Map<EncodingTreeNode*, int> weightings;

    // build from ground up, leaves first
    for (char c : frequencies) {
        EncodingTreeNode* leaf = new EncodingTreeNode(c);   // create leaf
        priority.enqueue(leaf, frequencies[c]);             // add into priority queue with frequency as priority
        weightings.put(leaf, frequencies[c]);                // add into weightings map
    }

    // use iteration to merge from bottom up until only one tree/node remains
    while (priority.size() != 1) {

        // start with highest priority at front of queue
        EncodingTreeNode* left = priority.dequeue();
        EncodingTreeNode* right = priority.dequeue();
        EncodingTreeNode* node = new EncodingTreeNode(left, right);

        // edit weightings to include new nodes
        int weight = weightings[left] + weightings[right];
        weightings.put(node, weight);
        priority.enqueue(node, weight);
    }

    // only 1 returns, this is the final result
    return priority.dequeue();
}

/**
 * This function contains the helper function for encodeText. It takes an EncodingTreeNode, a
 * map of characters and queues of bits as the result, and a queue of bits as the current sequence.
 * This function recursively moves through the tree to generate the sequence of bits associated with
 * the tree structure. When the encodingTreeNode is at a node, we simply move down. When it's at a leaf,
 * we put the current sequence and character associated into the result.
 */

void encodeHelper(EncodingTreeNode* tree, Map<char, Queue<Bit>>& result, Queue<Bit> sequence) {

    // base case: we're at a node, no character associated
    if (tree == nullptr)
        return;

    // base case: we're at a leaf
    if (tree->zero == nullptr && tree->one == nullptr) {
        result.put(tree->ch, sequence);
        return;
    }

    // left
    Queue<Bit> left = sequence;
    left.enqueue(0);
    encodeHelper(tree->zero, result, left);

    // right
    Queue<Bit> right = sequence;
    right.enqueue(1);
    encodeHelper(tree->one, result, right);
}

/**
 * Given a string and an encoding tree, encode the text using the tree
 * and return a Queue<Bit> of the encoded bit sequence.
 *
 * You can assume tree is a valid non-empty encoding tree and contains an
 * encoding for every character in the text.
 *
 * This function encodes a string parameter of text into an encodingTreeNode,
 * returning a queue of bits that represents the encoded text. This function
 * calls the helper function to traverse through the tree and store the asosciated
 * bit sequence for each character in a Map, and then iterates through each character
 * in the text to generate the entire sequence for the string.
 */
Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {

    // use helper function to traverse through tree
    Map<char, Queue<Bit>> map;
    Queue<Bit> sequence;
    encodeHelper(tree, map, sequence);

    // analyze each character in the text
    Queue<Bit> result;
    for (char c : text) {
        // use map to help
        Queue<Bit> bits = map[c];
        // queue up onto the result
        while (!bits.isEmpty())
            result.enqueue(bits.dequeue());
    }

    return result;
}

/**
 * Flatten the given tree into a Queue<Bit> and Queue<char> in the manner
 * specified in the assignment writeup.
 *
 * You can assume the input queues are empty on entry to this function.
 *
 * You can assume tree is a valid well-formed encoding tree.
 *
 * This function takes an EncodingTreeNode, a queue of bits passed by reference as the treeShape,
 * and a queue of characters passed by reference as the treeLeaves, and edits the tree shape and
 * tree leaves to flatten the tree. It recursively moves down the left and then right side of the tree,
 * and when it reaches leaves at the end, enqueues the associated characters to the tree leaves and the
 * bit representing leaves to the tree shape.
 */
void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves) {

    // base case: we're at a leaf, head back to top by returning
    if (tree->zero == nullptr && tree->one == nullptr) {
        treeLeaves.enqueue(tree->ch);
        treeShape.enqueue(0);
        return;
    }

    // we're at a node
    treeShape.enqueue(1);
    // left
    flattenTree(tree->zero, treeShape, treeLeaves);
     // right
    flattenTree(tree->one, treeShape, treeLeaves);
}

/**
 * Compress the input text using Huffman coding, producing as output
 * an EncodedData containing the encoded message and flattened
 * encoding tree used.
 *
 * Reports an error if the message text does not contain at least
 * two distinct characters.
 *
 * This function takes a parameter of a string message and returns the encodedData that
 * comes from compressing the text. It first builds the huffman tree from the text, and then
 * flattens the tree, before encoding the text from the tree. Before returning the result, it
 * deallocates the tree to prevent memory leaks.
 */
EncodedData compress(string messageText) {    
    EncodedData result;

    EncodingTreeNode* huffman = buildHuffmanTree(messageText);
    flattenTree(huffman, result.treeShape, result.treeLeaves);
    result.messageBits = encodeText(huffman, messageText);

    deallocateTree(huffman);
    return result;
}

/* * * * * * Testing Helper Functions Below This Point * * * * * */

EncodingTreeNode* createExampleTree() {
    /* Example encoding tree used in multiple test cases:
     *                *
     *              /   \
     *             T     *
     *                  / \
     *                 *   E
     *                / \
     *               R   S
     */

    EncodingTreeNode* leafT = new EncodingTreeNode('T');
    EncodingTreeNode* leafE = new EncodingTreeNode('E');
    EncodingTreeNode* leafR = new EncodingTreeNode('R');
    EncodingTreeNode* leafS = new EncodingTreeNode('S');

    EncodingTreeNode* node1 = new EncodingTreeNode(leafR, leafS);
    EncodingTreeNode* node2 = new EncodingTreeNode(node1, leafE);
    EncodingTreeNode* root = new EncodingTreeNode(leafT, node2);

    return root;
}

void deallocateTree(EncodingTreeNode* t) {

    // at the end, beyond the leaf
    if (t == nullptr)
        return;

    deallocateTree(t->zero);
    deallocateTree(t->one);

    delete t;
}

bool areEqual(EncodingTreeNode* a, EncodingTreeNode* b) {

    // check empty trees, end of trees
    if (a == nullptr || b == nullptr)
        return a == b;

    // check characters are same and rest of tree is correct
    return (a->ch == b-> ch && areEqual(a->zero, b->zero) && areEqual(a->one, b->one));
}

/* * * * * * Test Cases Below This Point * * * * * */

/* TODO: Write your own student tests. */

STUDENT_TEST("Validate that deallocateTree works as intended") {
    EncodingTreeNode* tree = createExampleTree();
    deallocateTree(tree);
}

STUDENT_TEST("test that areEqual works as intended on identical trees") {
    EncodingTreeNode* tree1 = createExampleTree();
    EncodingTreeNode* tree2 = createExampleTree();
    EXPECT(areEqual(tree1, tree2));
    deallocateTree(tree1);
    deallocateTree(tree2);
}

STUDENT_TEST("test that areEqual works as intended on different trees") {
    EncodingTreeNode* tree1 = createExampleTree();
    EncodingTreeNode* tree2 = createExampleTree();
    tree2->zero->ch = 'O';
    EXPECT(!areEqual(tree1, tree2));
    deallocateTree(tree1);
    deallocateTree(tree2);
}

STUDENT_TEST("test that areEqual works as intended on simple tree created from scratch") {
    EncodingTreeNode* tree1 = createExampleTree();

    EncodingTreeNode* simple1 = new EncodingTreeNode('A');
    EncodingTreeNode* simple2 = new EncodingTreeNode('B');
    EncodingTreeNode* simple = new EncodingTreeNode(simple1, simple2);

    EXPECT(!areEqual(tree1, simple));

    EncodingTreeNode* tree2 = createExampleTree();

    EXPECT(areEqual(tree1, tree2));
    EXPECT(!areEqual(tree1, tree1->zero));

    deallocateTree(tree1);
    deallocateTree(tree2);
    deallocateTree(simple);

}

STUDENT_TEST("test that decodeText works as intended with example tree: duplicate letters") {
    EncodingTreeNode* tree = createExampleTree();
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1};

    EXPECT_EQUAL(decodeText(tree, messageBits), "RRSS");
    deallocateTree(tree);
}

STUDENT_TEST("test that decodeText works as intended with example tree: edge letters") {
    EncodingTreeNode* tree = createExampleTree();
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = {0, 1, 1};

    EXPECT_EQUAL(decodeText(tree, messageBits), "TE");
    deallocateTree(tree);
}

STUDENT_TEST("test that decodeText works as intended with example tree: all letters") {
    EncodingTreeNode* tree = createExampleTree();
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = {0, 1, 0, 0, 1, 0, 1, 1, 1};

    EXPECT_EQUAL(decodeText(tree, messageBits), "TRSE");
    deallocateTree(tree);
}

STUDENT_TEST("test that decompress works as intended with warmup tree 1") {
    EncodedData data = {
        { 1, 0, 1, 0, 0 }, // treeShape
        { 'E', 'W', 'K' },  // treeLeaves
        { 0, 1, 0, 1, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "EWK");
}

STUDENT_TEST("test that decompress works as intended with warmup tree 1") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeShape
        { 'A', 'D', 'B', 'N' },  // treeLeaves
        { 0, 1, 0, 0, 1, 0, 1, 1, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "ADBN");
}

STUDENT_TEST("test that decompress works as intended with warmup tree 1") {
    EncodedData data = {
        { 1, 1, 0, 1, 0, 0, 0 }, // treeShape
        { 'N', 'M', 'S', 'O' },  // treeLeaves
        { 0, 0, 0, 1, 0, 0, 1, 1, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "NMSO");
}

STUDENT_TEST("test that encodeText works as intended alongside decodeText example tree: duplicate letters") {
    EncodingTreeNode* tree = createExampleTree();
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1};
    string text = "RRSS";

    Queue<Bit> bits = messageBits;
    EXPECT_EQUAL(encodeText(tree, decodeText(tree, messageBits)), bits);
    deallocateTree(tree);
}

STUDENT_TEST("test that encodeText works as intended alongside decodeText example tree: edge letters") {
    EncodingTreeNode* tree = createExampleTree();
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = {0, 1, 1};
    string text = "TE";

    Queue<Bit> bits = messageBits;
    EXPECT_EQUAL(encodeText(tree, decodeText(tree, messageBits)), bits);
    deallocateTree(tree);
}

STUDENT_TEST("test that encodeText works as intended alongside decodeText example tree: all letters") {
    EncodingTreeNode* tree = createExampleTree();
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = {0, 1, 0, 0, 1, 0, 1, 1, 1};
    string text = "TRSE";

    Queue<Bit> bits = messageBits;
    EXPECT_EQUAL(encodeText(tree, decodeText(tree, messageBits)), bits);
    deallocateTree(tree);
}

STUDENT_TEST("test that flattenTree works as intended alongisde unflattenTree with example tree") {
    EncodingTreeNode* original = createExampleTree();
    Queue<Bit> treeShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };

    EncodingTreeNode* modified = unflattenTree(treeShape, treeLeaves);
    flattenTree(modified, treeShape, treeLeaves);

    EXPECT(areEqual(original, modified));

    deallocateTree(original);
    deallocateTree(modified);
}

STUDENT_TEST("test that flattenTree works as intended alongisde unflattenTree with example tree queues of shapes and leaves") {
    Queue<Bit> originalShape = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> originalLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit> shape1 = originalShape;
    Queue<Bit> newShape;

    Queue<char> leaf1 = originalLeaves;
    Queue<char> newLeaves;

    EncodingTreeNode* tree = unflattenTree(shape1, leaf1);
    flattenTree(tree, newShape, newLeaves);

    EXPECT_EQUAL(newShape, originalShape);
    EXPECT_EQUAL(newLeaves, originalLeaves);

    deallocateTree(tree);
}

STUDENT_TEST("test that buildHuffManTree raises error for text of less than 2 unique characters") {
    string text = "ccc";
    EXPECT_ERROR(buildHuffmanTree(text));
}

STUDENT_TEST("test that buildHuffmanTree works as intended on example alongisde encodeText and decodeText") {
    EncodingTreeNode* tree = buildHuffmanTree("BOOKKEEPER");
    Queue<Bit> messageBits = encodeText(tree, "BOOKKEEPER");
    string decoded = decodeText(tree, messageBits);

    EXPECT_EQUAL(decoded, "BOOKKEEPER");

    deallocateTree(tree);
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("decodeText, small example encoding tree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(decodeText(tree, messageBits), "SET");

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}; // STREETS
    EXPECT_EQUAL(decodeText(tree, messageBits), "STREETS");

    deallocateTree(tree);
}

PROVIDED_TEST("unflattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  treeShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

PROVIDED_TEST("decompress, small example input") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeShape
        { 'T', 'R', 'S', 'E' },  // treeLeaves
        { 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "TRESS");
}

PROVIDED_TEST("buildHuffmanTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    EncodingTreeNode* tree = buildHuffmanTree("STREETTEST");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

PROVIDED_TEST("encodeText, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(encodeText(reference, "E"), messageBits);

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(encodeText(reference, "SET"), messageBits);

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1 }; // STREETS
    EXPECT_EQUAL(encodeText(reference, "STREETS"), messageBits);

    deallocateTree(reference);
}

PROVIDED_TEST("flattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  expectedShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

PROVIDED_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    Queue<Bit>  treeShape   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeChars   = { 'T', 'R', 'S', 'E' };
    Queue<Bit>  messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0 };

    EXPECT_EQUAL(data.treeShape, treeShape);
    EXPECT_EQUAL(data.treeLeaves, treeChars);
    EXPECT_EQUAL(data.messageBits, messageBits);
}

PROVIDED_TEST("Test end-to-end compress -> decompress") {
    Vector<string> inputs = {
        "HAPPY HIP HOP",
        "Nana Nana Nana Nana Nana Nana Nana Nana Batman",
        "Research is formalized curiosity. It is poking and prying with a purpose. – Zora Neale Hurston",
    };

    for (string input: inputs) {
        EncodedData data = compress(input);
        string output = decompress(data);

        EXPECT_EQUAL(input, output);
    }
}
