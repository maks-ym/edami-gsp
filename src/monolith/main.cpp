// 'outputs' and 'temp' directory must be created manually

#include <iostream>
#include "postprocessing.h"
#include "preprocessing.h"
#include "support.h"

using namespace std;

int main()
{
    string filename = "";
    cout << "Type the name if dataset: ";
    std::cin >> filename;
    //preprocessing
    preprocessing(filename, false);
    //tree
    // Some magic with hash tree

    //support count
    support(filename);
    //postprocessing
    postprocessing(filename, false);

    cout << "The result is here /outputs/" + filename + "_word_sequences.data" << endl;
}
