#pragma once

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

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <windows.h>
#include <unistd.h>     //UNIX sleep()

using namespace std;

std::string prune_line( std::string line );
void error_in_line_msg( int line_num );
template <typename T> string NumberToString ( T Number );

void preprocessing( std::string filename )
{
    cout << "=============== PREPROCESSING ===============" << endl;
/** ===================================== initialize vars =============================================== **/
    std::string in_filename = "datasets/" + filename,
                out_pruned_data_filename = "temp/" + filename + "_pruned.txt",
                out_processed_data_filename = "temp/" + filename + "_processed.txt",
                word2num_dict_filename = "temp/word2num.dict",
                num2word_dict_filename = "temp/num2word.dict",
                line;
    unsigned line_counter = 1,
             unique_words_counter = 1,
             reading_errors = 0;
    bool unknown_val_used = false;

/** ==================================== initialize dictionary ========================================= **/
    std::map<std::string, int> word2num_dict;
    std::map<int, std::string> num2word_dict;

/** =================================== open files (input & outputs) =================================== **/
    // dataset
    ifstream infile( in_filename.c_str() );
    if( !infile )
    {
        cout << "[Error]: failed to open file " + in_filename << endl;
        throw exception();
    }

    // pruned dataset
    ofstream outprunedfile(out_pruned_data_filename.c_str(), ios::trunc | ios::out);
    if( !outprunedfile )
    {
        cout << "[Error]: failed to open file " + out_pruned_data_filename << endl;
        //throw exception();
    }

    // converted to numbers dataset
    ofstream outprocessedfile(out_processed_data_filename.c_str(), ios::trunc | ios::out);
    if( !outprocessedfile )
    {
        cout << "[Error]: failed to open file " + out_processed_data_filename << endl;
        throw exception();
    }

    // dictionary word2num
    ofstream out_word2num_dict(word2num_dict_filename.c_str(), ios::trunc | ios::out);
    if( !out_word2num_dict )
    {
        cout << "[Error]: failed to open file " + word2num_dict_filename << endl;
        throw exception();
    }

    // dictionary num2word
    ofstream out_num2word_dict(num2word_dict_filename.c_str(), ios::trunc | ios::out);
    if( !out_num2word_dict )
    {
        cout << "[Error]: failed to open file " + num2word_dict_filename << endl;
        throw exception();
    }

/**========================================== read from file ========================================= **/
    while ( std::getline( infile, line ) )
    {
        cout << "--- reading line ---" << endl;
        cout << line_counter << ": read:   " << line << endl;
        line = prune_line( line );
        cout << line_counter << ": pruned: " << line << endl;  //DEGUB: show pruned line

        std::istringstream iss( line );
        if( !iss ) {
            cout << "[Error]: failed to create string stream from line." << endl;
            throw exception();
        }

        // add items to dictionary
        int cid = 0, tid = 0, num_of_items = 0;
        string processed_line = "";
        cout << "[empty] processed line : " << processed_line << endl;
        if ( !( iss >> cid >> tid >> num_of_items ) )
        {
            error_in_line_msg(line_counter);
            ++reading_errors;
            continue;
        }

        processed_line += NumberToString(cid) + " " + NumberToString(tid) + " " + NumberToString(num_of_items);
        cout << "[cid tid num] processed line : " << processed_line << endl;

        for( int i = 0; i < num_of_items; ++i )
        {
            //add word to dictionaries
            string current_word = "";
            if( !( iss >> current_word ) )
            {
                cout << "[Warning]: empty (unknown) word " << i+1 << " of " << num_of_items << endl;
                cout << "Using general value UNKNOWN..." << endl;

                Sleep( 500 );

                if( !unknown_val_used ) {
                    word2num_dict.insert( pair<string,int>( "UNKNOWN",unique_words_counter ));
                    num2word_dict.insert( pair<int,string>( unique_words_counter,"UNKNOWN" ) );
                    processed_line += " " + NumberToString( unique_words_counter );
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

            map<string,int>::iterator it = word2num_dict.find( current_word );
            if( it == word2num_dict.end() )
            {
                // word is not found
                word2num_dict.insert( pair<string,int>( current_word,unique_words_counter ) );
                num2word_dict.insert( pair<int,string>( unique_words_counter,current_word ) );
                processed_line += " " + NumberToString( unique_words_counter );
                out_word2num_dict << current_word << " " << unique_words_counter << endl;
                out_num2word_dict << unique_words_counter << " " << current_word << endl;
                ++unique_words_counter;
            }
            else {
                // word is found
                processed_line += " " + NumberToString( it->second );
            }
        }
        cout << "[complete] processed line : " << processed_line << endl;

        // output
        outprocessedfile << processed_line << endl;
        outprunedfile << line << endl;
        //increment counters
        ++line_counter;
        cout << "--- ++++++++++++ ---" << endl;
    }//while ended

    cout << "Data preprocessed with " << reading_errors << " errors." << endl;
    Sleep( 2000 );

    // close all streams of files
    infile.close();
    outprunedfile.close();
    outprocessedfile.close();
    out_word2num_dict.close();
    out_num2word_dict.close();
}

/** -------------------------------- helping functions -------------------------------- */

std::string prune_line( std::string line )
{
    line.erase( std::remove_if( line.begin(), line.end(), []( char c ) {
                                  return !( std::isspace( c ) || std::isalnum( c ) );
                                  }), line.end() );
    std::transform( line.begin(), line.end(), line.begin(), ::tolower );
    return line;
}

void error_in_line_msg( int line_num )
{
    cout << "[Error]: failed to read Cid, Tid or Number of items in line " << line_num << endl;
    cout << "Results of all previous operations saved to appropriate files..." << endl;
    Sleep( 500 );
    cout << "Further processing..." << endl;
    Sleep( 500 );
}

template <typename T>
string NumberToString( T Number )
{
    ostringstream ss;
    ss << Number;
    return ss.str();
}
