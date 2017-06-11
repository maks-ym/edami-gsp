/**
 * Author - Maksym Bakhmut
 * Module - postprocessing unit
 * Date   - 2017.06.08
 *
 * Overview
 *
 */

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <unistd.h>     //UNIX sleep()

#include <iostream>

using namespace std;

bool is_number(const std::string& s);

int main()
{
// initialize vars =================================================================================================
    std::string num_sequences_filename = "../../outputs/num_sequences.data",
                word_sequences_filename = "../../outputs/word_sequences.data",
                num2word_dict_filename = "../../outputs/num2word.dict";

    string line, current_token;
    int line_counter = 1;
    //unsigned reading_errors = 0;

// initialize dictionary =========================================================================================
    std::vector<std::string> num2word_dict;

// open files (input & outputs) ==================================================================================
    // read from
    ifstream insequences(num_sequences_filename.c_str());
    if(!insequences)
    {
        cout << "[Error]: failed to open file " + num_sequences_filename << endl;
        return 1;
    }
    // read dictionary
    ifstream dictfile(num2word_dict_filename.c_str());
    if(!dictfile)
    {
        cout << "[Error]: failed to open file " + num2word_dict_filename << endl;
        return 1;
    }

    // output pruned
    ofstream outsequences(word_sequences_filename.c_str(), ios::trunc | ios::out);
    if(!outsequences)
    {
        cout << "[Error]: failed to open file " + word_sequences_filename << endl;
        return 1;
    }

// read from file ==================================================================================================
    //read dictionary
    while(std::getline(dictfile, line)) {
        std::istringstream iss(line);
        if(!iss) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading dictionary" << endl;
            sleep(1);
            return 1;
        }
        iss >> current_token >> current_token;
        cout << current_token << endl;
        /**NOTE: consider INDEX shift!!!*/
        num2word_dict.push_back(current_token);
    }

    //read sequences
    line_counter = 0;
    while (std::getline(insequences, line))
    {
        cout << line_counter << ": " << line << endl;
        std::istringstream iss(line);
        if(!iss) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading sequences" << endl;
            sleep(1);
            return 1;
        }
        std::string word_sequence = "";
//        if (!(iss >> )) {
//            error_in_line_msg(line_counter);
//            ++reading_errors;
//            continue;
//        }
        while(iss >> current_token) {
            if(!is_number(current_token)) {
                word_sequence += current_token + " ";
                cout << current_token << endl;
            }
            else {
                word_sequence += num2word_dict[std::stoi(current_token) - 1];
                word_sequence += " ";
                cout << current_token << endl;
            }
        }

        // output
        outsequences << word_sequence << endl;
        //increment counters
        ++line_counter;

    }//while ended
//    cout << "Data processed with " << reading_errors << " errors." << endl;


//  close all streams of files
    insequences.close();
    outsequences.close();
    dictfile.close();

    return 0;
}

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}
