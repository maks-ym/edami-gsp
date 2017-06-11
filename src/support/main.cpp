/**
 * Author - Maksym Bakhmut
 * Module - support counting unit
 * Date   - 2017.06.08
 *
 * Overview
 *
 * - creates 2 dictionaries for conversion (from words to numbers and vice versa)
 * - reads file with words, writes with numbers
 * - output number of unique items and number of unique CIds
 */

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
#include <unistd.h>     //UNIX sleep()
#include <typeinfo>     //DUBUG info about vartypes

namespace gsp {
    typedef long long slong;
    struct gsp_element {
        std::map<slong,slong> items;
        slong start_time;
        slong end_time;
    };
}

using namespace std;

bool is_number(const std::string& s);
void show_gsp(const vector<gsp::gsp_element> &gsp);


int main() {
    std::string in_filename = "../../datasets/tags.data",
           in_params_filename = "../../outputs/params",
           in_cand_seq_filename = "../../outputs/candidate_sequences",
           out_cand_seq_filename = "../../outputs/num_cand_seq_with_sup";

    std::string cur_gsp_line = "",
                cur_cid_line = "",
                cur_param_line  = "";
    gsp::slong line_counter = 1,//TODO: is needed ?
         last_cid = 0,      //TODO: is needed ?
         reading_errors = 0,//TODO: is needed ?
         winSize = 0,
         minGap = 0,
         maxGap = 0;

// initialize structures (dataset & sequences) =====================================================================

    std::vector<gsp::gsp_element> current_gsp;
    std::map<gsp::slong, std::set<gsp::slong>> current_cid_data;

// open files (input & outputs) ====================================================================================
    // data input
    ifstream infile(in_filename.c_str());
    if(!infile)
    {
        cout << "[Error]: failed to open file " + in_filename << endl;
        return 1;
    }
    // params input
    ifstream inparamsfile(in_params_filename.c_str());
    if(!inparamsfile)
    {
        cout << "[Error]: failed to open file " + in_params_filename << endl;
        return 1;
    }
    // params input
    ifstream inseqfile(in_cand_seq_filename.c_str());
    if(!inseqfile)
    {
        cout << "[Error]: failed to open file " + in_cand_seq_filename << endl;
        return 1;
    }
    // sequences with support output
    ofstream outseqfile(out_cand_seq_filename.c_str(), ios::trunc | ios::out);
    if(!outseqfile)
    {
        cout << "[Error]: failed to open file " + out_cand_seq_filename << endl;
        return 1;
    }

    //read params: first 3 lines - winSize, minGap, maxGap
    gsp::slong params[3];
    for(int i = 0; i < 3; ++i) {
        std::getline(inparamsfile, cur_param_line  );
        std::istringstream iss(cur_param_line  );
        if(!iss) {
            cout << "[Error]: failed to create string stream from cur_param_line  ." << endl;
            cout << "[Error]: in reading sequences" << endl;
//            sleep(1);
            return 1;
        }
        std::string current_token = "";
        while(iss >> current_token) {
            if(is_number(current_token)) {
                params[i] = std::stoi(current_token);
                cout << current_token << endl;
                break;
            }
        }
    }
    winSize = params[0];
    minGap  = params[1];
    maxGap  = params[2];

// main routine: read cand sequences line by line, for each check do the check in data
    while(std::getline(inseqfile, cur_gsp_line)) {
        if(cur_gsp_line.length() < 4) {
            continue;
        }
        cout << "Current GSP [length = " << cur_gsp_line.length() << "]: " << cur_gsp_line << endl;
//        sleep(1);
        std::istringstream iss(cur_gsp_line);
        if(!iss) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading sequences" << endl;
            sleep(1);
            return 1;
        }
        else {
            cout << "Openning string stream" << endl;
        }
        // create data structure for current gsp
        cout << "Start reading from string stream." << endl;
        std::string current_token = "";
        gsp::gsp_element cur_element;
        cur_element.start_time = -1;
        cur_element.end_time = -1;


        gsp::slong element_count = 0;        //DEBUG
        while(iss >> current_token) {
            cout << "Element index = " << element_count << endl;
            cout << "Length of GSP vector: " << current_gsp.size();
//            cout << " Type of gsp: " << typeid(current_gsp).name() << endl;
            if(is_number(current_token)) {
                cout << "[read] item for element. TOKEN = " << current_token << endl;
                cur_element.items.insert(std::make_pair(std::stoi(current_token), -1));
                cout << "current element's items: ";
                for (std::map<gsp::slong,gsp::slong>::iterator item_it = cur_element.items.begin(); item_it != cur_element.items.end(); ++item_it) {
                    cout << item_it->first << " ";
                }
                cout << endl;
//                cout << "Type of the element: " << typeid(cur_element).name() << endl;
//                sleep(1);
//                Sleep(1000);      //Windows
            }
            else if(current_token == "<(") {
                cout << "[read] beginning of the sequence. TOKEN = " << current_token << endl;
//                sleep(1);
                continue;
            }
            else {
                cout << "[read] the end of element. TOKEN = " << current_token << endl;
                cout << " push element to the vector." << endl;
                sleep(1);
                current_gsp.push_back(cur_element);
                cout << "Length of items of " << element_count << " element: " << cur_element.items.size() << endl;
                cur_element.items.clear();
                ++element_count;
            }
        }
        cout << "length of read GSP (number of elements): " << current_gsp.size() << endl;

        // show current GSP (from created structure)
        show_gsp(current_gsp);








//        while(std::getline(inseqfile, cur_cid_line)) {
//            //read data for current Cid
//            //Q: HOW TO STOP READING for current Cid??? -- next line is read, where to save;
//        }



        //write to file
        /** TODO: output GSP with its support */

        //clean current gsp structure
        current_gsp.clear();
    }


    //close file streams
    infile.close();
    inparamsfile.close();
    inseqfile.close();
    outseqfile.close();

    return 0;
}


bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void show_gsp(const vector<gsp::gsp_element> &gsp) {
    cout << "ITEMS: <";
    for(vector<gsp::gsp_element>::const_iterator gsp_iter = gsp.begin(); gsp_iter != gsp.end(); ++gsp_iter) {
        cout << "(";
//        cout << "[" <<typeid(*gsp_iter).name() << "]";
        for(std::map<gsp::slong,gsp::slong>::const_iterator item_iter = gsp_iter->items.begin(); item_iter != gsp_iter->items.end(); ++item_iter) {
            cout << " " << item_iter->first;
        }
        cout << " )";
    }
    cout << ">" << endl;
}
