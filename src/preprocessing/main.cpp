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
#include <algorithm>
#include <map>
#include <unistd.h>     //UNIX sleep()


using namespace std;

std::string prune_line(std::string line);
void error_in_line_msg(int line_num);
//ifstream open_inputfile(std::string filename);    //TODO: sugar input
//ofstream open_outputfile(std::string filename);   //TODO: sugar output

int main()
{
// initialize vars =================================================================================================
    std::string in_filename = "../../datasets/tags.data",
                out_pruned_data_filename = "../../outputs/pruned_tags.data", // 'putputs' directory must be created manually
                out_processed_data_filename = "../../outputs/processed_tags.data",
                word2num_dict_filename = "../../outputs/word2num.dict",
                num2word_dict_filename = "../../outputs/num2word.dict";

    string line;
    unsigned int line_counter = 1,
                 last_cid = 0,
                 unique_words_counter = 1;
    unsigned reading_errors = 0;

// initialize dictionaries =========================================================================================
    std::map<std::string, int> word2num_dict;
    std::map<int, std::string> num2word_dict;

// open files (input & outputs) ====================================================================================
    // read from
    ifstream infile(in_filename.c_str());
    if(!infile)
    {
        cout << "[Error]: failed to open file " + in_filename << endl;
        return 1;
    }
    // output pruned
    ofstream outprunedfile(out_pruned_data_filename.c_str(), ios::trunc | ios::out);
    if(!outprunedfile)
    {
        cout << "[Error]: failed to open file " + out_pruned_data_filename << endl;
        return 1;
    }
    // output processed
    ofstream outprocessedfile(out_processed_data_filename.c_str(), ios::trunc | ios::out);
    if(!outprocessedfile)
    {
        cout << "[Error]: failed to open file " + out_processed_data_filename << endl;
        return 1;
    }
    // output dictionary word2num
    ofstream out_word2num_dict(word2num_dict_filename.c_str(), ios::trunc | ios::out);
    if(!out_word2num_dict)
    {
        cout << "[Error]: failed to open file " + word2num_dict_filename << endl;
        return 1;
    }
    // output dictionary num2word
    ofstream out_num2word_dict(num2word_dict_filename.c_str(), ios::trunc | ios::out);
    if(!out_num2word_dict)
    {
        cout << "[Error]: failed to open file " + num2word_dict_filename << endl;
        return 1;
    }

// read from file ==================================================================================================
    //cout << "cid\t" << "tid\t" << "num_of_items\t" << "items" << endl;  //DEGUB: show pruned line
    while (std::getline(infile, line))
    {
        cout << line_counter << ": " << line << endl;
        line = prune_line(line);
        cout << line_counter++ << ": " <<  line << endl;    //DEGUB: show pruned line

        std::istringstream iss(line);
        if(!iss) {
            cout << "[Error]: failed to create string stream from line." << endl;
            return 1;
        }

        //TODO HERE: read cid, tid, numofites one by one
        // LOOP items and add to dictionaries
        int cid = 0, tid = 0, num_of_items = 0;
        string processed_line = "";
        cout << "[empty] processed line " << processed_line << endl;
        if (!(iss >> cid >> tid >> num_of_items)) {
            error_in_line_msg(line_counter);
            ++reading_errors;
            continue;
        }
        processed_line += to_string(cid) + " " + to_string(tid) + " " + to_string(num_of_items);
        cout << "[cid tid num] processed line " << processed_line << endl;
//        bool error_reading_words = false;
        bool unknown_val_used = false;
        for(int i = 0; i < num_of_items; ++i) {
            //add word to dicts
            string current_word = "";
            if(!(iss >> current_word)) {
                cout << "[Warning]: empty (unknown) word " << i+1 << " of " << num_of_items << endl;
                cout << "Using general value UNKNOWN..." << endl;
                sleep(1);
                if(!unknown_val_used) {
                    word2num_dict.insert(pair<string,int>("UNKNOWN",unique_words_counter));
                    num2word_dict.insert(pair<int,string>(unique_words_counter,"UNKNOWN"));
                    processed_line += " " + to_string(unique_words_counter);
                    out_word2num_dict << "UNKNOWN" << " " << unique_words_counter << endl;
                    out_num2word_dict << unique_words_counter << " " << "UNKNOWN" << endl;
                    ++unique_words_counter;
                    unknown_val_used = true;
                }
                else {
                    current_word = "UNKNOWN";
                }
                break;
            }
            map<string,int>::iterator it = word2num_dict.find(current_word);
            if(it == word2num_dict.end()) {
                // no such word in dictionary yet
//                ?? отсюда!!
//                TODO: implement UNKNOWN value
                word2num_dict.insert(pair<string,int>(current_word,unique_words_counter));
                num2word_dict.insert(pair<int,string>(unique_words_counter,current_word));
                processed_line += " " + to_string(unique_words_counter);
                out_word2num_dict << current_word << " " << unique_words_counter << endl;
                out_num2word_dict << unique_words_counter << " " << current_word << endl;
                ++unique_words_counter;
            }
            else {
                //  word is found
                processed_line += " " + to_string(it->second);
            }
        }
        cout << "[complete] processed line " << processed_line << endl;
//        if(error_reading_words) {
//            continue;
//        }

        // output
        outprocessedfile << processed_line << endl;
        outprunedfile << line << endl;
        //increment counters
        ++line_counter;

    }//while ended
    cout << "Data processed with " << reading_errors << " errors." << endl;
    // close all streams of files
    infile.close();
    outprunedfile.close();
    outprocessedfile.close();
    out_word2num_dict.close();
    out_num2word_dict.close();

    return 0;
}


std::string prune_line(std::string line) {
    line.erase(std::remove_if(line.begin(), line.end(), [](char c) {
                                  return !(std::isspace(c) || std::isalnum(c)) ;
                                  }), line.end());
    std::transform(line.begin(), line.end(), line.begin(), ::tolower);
    return line;
}

void error_in_line_msg(int line_num) {
    cout << "[Error]: failed to read Cid, Tid or Number of items in line " << line_num << endl;
    sleep(1);
    //TODO: check saving of that operations (writing to pruned file, created and written dictionaries, number of items && num of Cids)
    cout << "Results of all previous operations saved to appropriate files..." << endl;
    sleep(1);
    cout << "Further processing..." << endl;
    sleep(1);
}
