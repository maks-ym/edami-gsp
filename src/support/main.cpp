/**
 * Author - Maksym Bakhmut
 * Module - support counting unit
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
void show_gsp(const std::vector<gsp::gsp_element> &gsp);
bool checkGSP(std::vector<gsp::gsp_element> &gsp, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
              gsp::slong &winSize, gsp::slong &minGap, gsp::slong &maxGap);
bool checkElement(gsp::gsp_element &element, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data, gsp::slong startTime);

template <typename T> T StringToNumber ( const string &Text );

int main() {
    std::string filename_base = "test";
//    std::string in_filename = "../../outputs/processed_tags.data",
    std::string in_filename = "../../outputs/test_processed.data",
                in_params_filename = "../../outputs/test_params",
                in_cand_seq_filename = "../../outputs/test_candidate_sequences",
                out_cand_seq_filename = "../../outputs/" + filename_base + "_num_cand_seq_with_sup";

    std::string cur_gsp_line = "",
                cur_cid_line = "",
                cur_param_line  = "";
    gsp::slong winSize = 0,
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
                params[i] = StringToNumber<gsp::slong>(current_token);
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
// read GSP =================================================================================================================
        if(cur_gsp_line.length() < 4) {
            continue;
        }
        cout << "Current GSP [length = " << cur_gsp_line.length() << "]: " << cur_gsp_line << endl;
//        sleep(1);
        std::istringstream iss(cur_gsp_line);
        if(!iss) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading sequences" << endl;
//            sleep(1);
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
                cur_element.items.insert(std::make_pair(StringToNumber<gsp::slong>(current_token), -1));
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
//                sleep(1);
                current_gsp.push_back(cur_element);
                cout << "Length of items of " << element_count << " element: " << cur_element.items.size() << endl;
                cur_element.items.clear();
                ++element_count;
            }
        }
        cout << "length of read GSP (number of elements): " << current_gsp.size() << endl;

        // show current GSP (from created structure)
        show_gsp(current_gsp);



// read data for current Cid & check if current pattern is supported ==================================================
        gsp::slong cid = 0,
                   tid = 0,
                   items_num = 0,
                   last_cid = 0,
                   cur_support = 0;
        std::set<gsp::slong> tidlist;


        while(std::getline(infile, cur_cid_line)) {
            std::istringstream iss(cur_cid_line);
            if(!iss) {
                cout << "[Error]: failed to create string stream from line." << endl;
                cout << "[Error]: in reading dataset" << endl;
//                sleep(1);
                return 1;
            }
            else {
                cout << "[dataset] Openning string stream" << endl;
            }

            iss >> cid;
            cout << "cid: " << cid << " | last_cid: " << last_cid << endl;
            if(cid != last_cid) {
                cout << "OTHER Cid! SUPPORT COUNTING ROUTING" << endl;
                if( checkGSP(current_gsp, current_cid_data, winSize, minGap, maxGap) ) {
                    ++cur_support;
                }
                //clean and move to next Cid
                current_cid_data.clear();
            }

            iss >> tid >> items_num;
            current_token = "";
            while(iss >> current_token) {
                cout << "current_token items: " << current_token << endl;
                tidlist.insert(StringToNumber<gsp::slong>(current_token));
            }
            cout << "tidlist to insert:";
            for(set<gsp::slong>::const_iterator iter = tidlist.begin(); iter != tidlist.end(); ++iter) {
                cout << " " << *iter;
            }
            cout << endl;
            //push into structure
            current_cid_data.insert(std::make_pair(tid,tidlist));
            cout << "tidlist pushed" << endl;
            cout << "==check current_cid_data==" << endl;
            for(map<gsp::slong, set<gsp::slong>>::const_iterator m_it = current_cid_data.begin(); m_it != current_cid_data.end(); ++m_it) {
                cout << "cid: " << cid << " | last_cid: " << last_cid << " | tid: " << m_it->first << " | ";
                cout << "items:";
                for(set<gsp::slong>::const_iterator item_iter = m_it->second.begin(); item_iter != m_it->second.end(); ++item_iter) {
                    cout << " " << *item_iter;
                }
                cout << endl;
            }
            cout << "==========================" << endl;
//            sleep(1);
            last_cid = cid;
            tidlist.clear();
        }


        //write to file
        /** TODO: output GSP with its support */
        outseqfile << cur_gsp_line << endl << "support - " << cur_support << endl;
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

bool checkGSP(std::vector<gsp::gsp_element> &gsp, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
              gsp::slong &winSize, gsp::slong &minGap, gsp::slong &maxGap) {
    /** SUPPORT COUNTING ROUTINE **/
    gsp::slong startTime = 0;
    for(unsigned i = 0; i < gsp.size(); ++i) {
        //forward faze
        if( !checkElement(gsp[i], cid_data, startTime) ) {
            return false;
        }
        else if(i == 0) {
            continue;
        }
        if((gsp[i].end_time - gsp[i-1].start_time) > maxGap) {
            // backward faze
            for(int j = i; 0 < j; ++j) {
                if( !checkElement( gsp[j-1], cid_data, gsp[j].end_time - maxGap ) ) {
                    return false;
                }
            }
            i = 0;
        }
        startTime = gsp[i].end_time + minGap;
    }
    return true;
}

bool checkElement(gsp::gsp_element &element, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data, gsp::slong startTime) {
    cout << "CHECK ELEMENT" << endl;
    for(std::map<gsp::slong,gsp::slong>::iterator item_iter = element.items.begin(); item_iter != element.items.end(); ++item_iter) {
        for(std::map<gsp::slong,std::set<gsp::slong>>::const_iterator cid_data_iter = cid_data.begin(); cid_data_iter != cid_data.end(); ++cid_data_iter) {
            if(cid_data_iter->first < startTime) {
                continue;
            }
            for(std::set<gsp::slong>::const_iterator tidlist_iter = cid_data_iter->second.begin(); tidlist_iter != cid_data_iter->second.end(); ++tidlist_iter) {
                if(*tidlist_iter == item_iter->first) {
                    //if such item in current_cid_data found, update time for item in element
                    item_iter->second = cid_data_iter->first;
                    break;
                }
            }
            if(item_iter->second == -1) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
T StringToNumber ( const string &Text )
{
    istringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}
