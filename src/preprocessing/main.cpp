/**
 * Author - Maksym Bakhmut
 * Module - preprocessing unit
 * Date   - 2017.06.08
 *
 * Overview
 *
 * - creates 2 dictionaries for conversion (from words to numbers and vice versa)
 * - reads file with words, writes with numbers
 * - output number of unique items and number of unique CIds
 */

/* TODO:
 * - read
 * - dictionary wordsToNum
 * - output of dictionary to file
 * - dict numToWords
 */


#include <iostream>
#include <sstream>
#include <string>
#include <fstream>


using namespace std;

int main()
{
    cout << "Hello world!" << endl;

    string in_filename = "/home/invictus/edami-gsp/datasets/tags.data";
    std::ifstream infile(in_filename);

    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        int cid = 0, tid = 0, num_of_items = 0;
        if (!(iss >> cid >> tid >> num_of_items)) { break; } // error
        cout << "cid: " << cid << endl;
        cout << "tid: " << tid << endl;
        cout << "num_of_items: " << num_of_items << endl;

    }

    return 0;
}
