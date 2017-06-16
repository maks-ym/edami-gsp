#pragma once

/**
 * Author - Maksym Bakhmut
 * Module - postprocessing unit
 * Date   - 2017.06.08
 *
 * Overview
 * // 'outputs' directory must be created manually
 */

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <windows.h>
#include <unistd.h>     //UNIX sleep()

using namespace std;

bool is_number(const std::string& s);
template <typename T> T StringToNumber ( const std::string &Text );

void postprocessing( std::string filename_base, bool verbose = true )
{
    cout << "=============== POSTPROCESSING ===============" << endl;
/** ===================================== initialize vars =============================================== **/
    std::string num_sequences_filename = "../../temp/" + filename_base + "_num_cand_seq_with_sup",
                word_sequences_filename = "../../outputs/" + filename_base + "_word_sequences.data",
                num2word_dict_filename = "../../temp/num2word.dict";

    string line, current_token;
    int line_counter = 1;
    //unsigned reading_errors = 0;

/** ==================================== initialize dictionary ========================================= **/
    std::vector<std::string> num2word_dict;

/** =================================== open files (input & outputs) =================================== **/
    // sequences
    ifstream insequences(num_sequences_filename.c_str());
    if(!insequences)
    {
        cout << "[Error]: failed to open file " + num_sequences_filename << endl;
        throw exception();
    }

    // dictionary
    ifstream dictfile(num2word_dict_filename.c_str());
    if(!dictfile)
    {
        cout << "[Error]: failed to open file " + num2word_dict_filename << endl;
        throw exception();
    }

    // converted to words sequences
    ofstream outsequences(word_sequences_filename.c_str(), ios::trunc | ios::out);
    if(!outsequences)
    {
        cout << "[Error]: failed to open file " + word_sequences_filename << endl;
        throw exception();
    }

/**========================================== read from file ========================================= **/
    //read dictionary
    if( verbose ) { cout << "--- read dictionary ---" << endl; }
    while( std::getline( dictfile, line ) ) {
        std::istringstream iss( line );
        if( !iss ) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading dictionary" << endl;
            Sleep( 500 );
            throw exception();
        }
        iss >> current_token >> current_token;
        if( verbose ) { cout << "- word - " << current_token << endl; }
        /**NOTE: consider INDEX shift!!!*/
        num2word_dict.push_back( current_token );
    }// WHILE END reading dictionary
    if( verbose ) { cout << "--- +++++++++++++++ ---" << endl; }

    //read sequences
    line_counter = 0;
    while ( std::getline( insequences, line ) )
    {
        if( verbose ) { cout << "--- read sequence ---" << endl; }
        bool support_line = false;
        if( verbose ) { cout << line_counter << ": " << line << endl; }
        std::istringstream iss( line );
        if(!iss) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading sequences" << endl;
            Sleep( 500 );
            throw exception();
        }
        std::string word_sequence = "";

        while( iss >> current_token ) {
            if(is_number(current_token)) {
                word_sequence += num2word_dict[StringToNumber<unsigned long>(current_token) - 1];
                word_sequence += " ";
                if( verbose ) { cout << current_token << endl; }
            }
            else if( current_token == "support" ) {
                word_sequence += current_token + " ";
                while( iss >> current_token ) {
                    word_sequence += current_token + " ";
                }
            }
            else {
                word_sequence += current_token + " ";
                if( verbose ) { cout << current_token << endl; }
            }
        }

        // output
        if( verbose ) { cout << word_sequence << endl; }
        outsequences << word_sequence << endl;
        //increment counters
        line_counter = (support_line) ? line_counter : line_counter + 1;
        if( verbose ) { cout << "--- +++++++++++++ ---" << endl; }
    }// WHILE END converting numbers to words in sequences

//  close all streams of files
    insequences.close();
    outsequences.close();
    dictfile.close();
}

/** ====================================== helping functions ====================================== */

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(),
                                      [](char c) { return !std::isdigit(c); }) == s.end();
}

template <typename T> T StringToNumber ( const std::string &Text )
{
    istringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}
