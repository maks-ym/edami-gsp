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

void postprocessing( std::string filename )
{
    cout << "=============== POSTPROCESSING ===============" << endl;
/** ===================================== initialize vars =============================================== **/
    std::string num_sequences_filename = "temp/" + filename + "_num_cand_seq_with_sup.txt",
                word_sequences_filename = "output/" + filename + "_word_sequences.txt",
                num2word_dict_filename = "temp/num2word.dict",
                in_freq_seq_filename = "temp/" + filename + "_num_freq_seq.txt",
                out_freq_seq_filename = "output/" + filename + "_word_freq_seq.txt";

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

    // number frequent sequences
    ifstream infreqseq(in_freq_seq_filename.c_str());
    if(!infreqseq)
    {
        cout << "[Error]: failed to open file " + in_freq_seq_filename << endl;
        throw exception();
    }

    // converted to words sequences
    ofstream outsequences(word_sequences_filename.c_str(), ios::trunc | ios::out);
    if(!outsequences)
    {
        cout << "[Error]: failed to open file " + word_sequences_filename << endl;
        throw exception();
    }

    // word frequent sequences
    ofstream outfreqseq(out_freq_seq_filename.c_str(), ios::trunc | ios::out);
    if(!outfreqseq)
    {
        cout << "[Error]: failed to open file " + out_freq_seq_filename << endl;
        throw exception();
    }

/**========================================== read from file ========================================= **/
    //read dictionary
    cout << "--- read dictionary ---" << endl;
    while( std::getline( dictfile, line ) ) {
        std::istringstream iss( line );
        if( !iss ) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading dictionary" << endl;
            Sleep( 500 );
            throw exception();
        }
        iss >> current_token >> current_token;
        // if( verbose ) { cout << "- word - " << current_token << endl; }
        /**NOTE: consider INDEX shift!!!*/
        num2word_dict.push_back( current_token );
    }// WHILE END reading dictionary
    cout << "--- +++++++++++++++ ---" << endl;

    cout << endl << "--- postrocess all sequences ---" << endl;
    //read sequences
    line = "";
    line_counter = 0;
    while ( std::getline( insequences, line ) )
    {
        cout << "--- read sequence ---" << endl;
        //bool support_line = false;
        cout << line_counter << ": " << line << endl;
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
                //cout << current_token << endl;
            }
            else if( current_token == "[" ) {
                word_sequence += "\t" + current_token + " ";
                while( iss >> current_token )
                {
                    word_sequence += current_token + " ";
                }
            }
            else {
                word_sequence += current_token + " ";
                //cout << current_token << endl;
            }
        }

        // output
        cout << word_sequence << endl;
        outsequences << word_sequence << endl;
        //increment counters
//        line_counter = (support_line) ? line_counter : line_counter + 1;
        line_counter = line_counter + 1;
        cout << "--- +++++++++++++ ---" << endl;
    }// WHILE END converting numbers to words in sequences

    cout << endl << "--- postrocess ONLY frequent sequences ---" << endl;
    // only frequent
    line = "";
    line_counter = 0;
    while ( std::getline( infreqseq, line ) )
    {
        cout << "--- read sequence ---" << endl;
        bool support_line = false;
        cout << line_counter << ": " << line << endl;
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
                //cout << current_token << endl;
            }
            else if( current_token == "[" ) {
                word_sequence += "\t" + current_token + " ";
                while( iss >> current_token ) {
                    word_sequence += current_token + " ";
                }
            }
            else {
                word_sequence += current_token + " ";
                //cout << current_token << endl;
            }
        }

        // output
        cout << word_sequence << endl;
        outfreqseq << word_sequence << endl;
        //increment counters
        line_counter = (support_line) ? line_counter : line_counter + 1;
        cout << "--- +++++++++++++ ---" << endl;
    }

//  close all streams of files
    insequences.close();
    outsequences.close();
    infreqseq.close();
    outfreqseq.close();
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
