#pragma once
/**
 * Author - Maksym Bakhmut
 * Module - support counting unit
 * Date   - 2017.06.08
 */

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <climits>
#include <map>
#include <set>
#include <unistd.h>     //UNIX sleep()
#include <windows.h>
#include <typeinfo>     //DUBUG info about vartypes
#include <time.h>

namespace gsp {
    typedef long long slong;
    struct gsp_element {
        std::map<slong,slong> items;
        slong start_time;
        slong end_time;
    };
}

using namespace std;

bool is_number( const std::string& s );
void show_gsp( const std::vector<gsp::gsp_element> &gsp, std::ofstream& logFile );
bool checkGSP( std::vector<gsp::gsp_element> &gsp, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
              gsp::slong &winSize, gsp::slong &minGap, gsp::slong &maxGap, std::ofstream& logFile );
bool checkElement( gsp::gsp_element &element, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
                   gsp::slong startTime, gsp::slong &winSize, std::ofstream& logFile );
bool is_number(const std::string& s);

template <typename T> T StringToNumber ( const std::string &Text );

void support( std::string filename_base, bool verbose = true )
{
    ofstream log( ("../../outputs/SUPPORT_LOG_" + filename_base).c_str(), ios::app | ios::out );
    if( !log )
    {
        cout << "[Error]: failed to open file ../../outputs/SUPPORT_LOG_" + filename_base << endl;
        throw exception();
    }

    log << endl << "//////////////////////////////////////////////////////////////////////////////////////" << endl;
    time_t _tm = time(NULL );
    struct tm * curtime = localtime ( &_tm );
    log << "date/time ------- " << asctime(curtime) << endl;
    log << "//////////////////////////////////////////////////////////////////////////////////////" << endl;


    cout << "=============== SUPPORT ===============" << endl;
    log << "=============== SUPPORT ===============" << endl;
/** ===================================== initialize vars =============================================== **/
    std::string in_filename = "../../temp/" + filename_base + "_processed.data",
           in_params_filename = "../../temp/params",
           in_cand_seq_filename = "../../temp/" + filename_base + "_candidate_sequences",
           out_cand_seq_filename = "../../temp/" + filename_base + "_num_cand_seq_with_sup",
           out_freq_seq_filename = "../../temp/" + filename_base + "_num_freq_seq";

    std::string cur_gsp_line = "",
                cur_cid_line = "",
                cur_param_line  = "";
    gsp::slong winSize = 0,
               minGap = 0,
               maxGap = 0,
               minSup = 0;

/** ========================= initialize structures (dataset & sequences) =============================== **/
    std::vector<gsp::gsp_element> current_gsp;
    std::map<gsp::slong, std::set<gsp::slong>> current_cid_data;

/** ==================================== open files (input & outputs) =================================== **/

    // params input
    ifstream inparamsfile( in_params_filename.c_str() );
    if( !inparamsfile )
    {
        cout << "[Error]: failed to open file " + in_params_filename << endl;
        log << "[Error]: failed to open file " + in_params_filename << endl;
        throw exception();
    }

    // params input
    ifstream inseqfile( in_cand_seq_filename.c_str() );
    if( !inseqfile )
    {
        cout << "[Error]: failed to open file " + in_cand_seq_filename << endl;
        log << "[Error]: failed to open file " + in_cand_seq_filename << endl;
        throw exception();
    }

    // sequences with support output
    ofstream outseqfile( out_cand_seq_filename.c_str(), ios::trunc | ios::out );
    if( !outseqfile )
    {
        cout << "[Error]: failed to open file " + out_cand_seq_filename << endl;
        log << "[Error]: failed to open file " + out_cand_seq_filename << endl;
        throw exception();
    }

/** ============================ read parameters (winSize, minGap, maxGap) ============================== **/
    cout << "--- parameters ---" << endl;
    log << "--- parameters ---" << endl;
    unsigned num_of_params = 4;
    gsp::slong params[num_of_params] = { 0 };
    for( unsigned i = 0; i < num_of_params; ++i )
    {
        std::getline( inparamsfile, cur_param_line );
        std::istringstream iss( cur_param_line );
        if( !iss )
        {
            cout << "[Error]: failed to create string stream from cur_param_line  ." << endl;
            cout << "[Error]: in reading sequences" << endl;
            log << "[Error]: failed to create string stream from cur_param_line  ." << endl;
            log << "[Error]: in reading sequences" << endl;
            Sleep( 600 );
            throw exception();
        }
        std::string current_token = "";
        while( iss >> current_token )
        {
            if( is_number( current_token ) )
            {
                params[i] = StringToNumber< unsigned long >( current_token );
                break;
            }
        }
    }
    winSize = params[0];
    minGap  = params[1];
    maxGap  = params[2];
    minSup  = params[3];
    if( verbose )
    {
        cout << "winSize: " << winSize << endl;
        cout << "minGap:  " << minGap  << endl;
        cout << "maxGap:  " << maxGap  << endl;
        cout << "minSup:  " << minSup<< endl;
        log << "winSize: " << winSize << endl;
        log << "minGap:  " << minGap  << endl;
        log << "maxGap:  " << maxGap  << endl;
        log << "minSup:  " << minSup  << endl;
    }

/** ======================== MAIN LOOP ( read & check support of GSP one-by-one ) ======================== */
    while( std::getline( inseqfile, cur_gsp_line ) )
    {
    /* ------------------------------------------- read gsp ------------------------------------------- */
        if( verbose ) {
                cout << "---- reading GSP ----" << endl;
                log << "---- reading GSP ----" << endl;
        }

        if( cur_gsp_line.length() < 4 ) {
            cout << "[read gsp]: too short line.\nSkip it." << endl;
            log << "[read gsp]: too short line.\nSkip it." << endl;
            continue;
        }

        cout << "Current GSP (read line): " << cur_gsp_line << endl;
        log << "Current GSP (read line): " << cur_gsp_line << endl;
        Sleep( 500 );

        std::istringstream iss( cur_gsp_line );
        if( !iss ) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading sequences" << endl;
            log << "[Error]: failed to create string stream from line." << endl;
            log << "[Error]: in reading sequences" << endl;
            Sleep( 500 );
            throw exception();
        }
        else {
            cout << "[ss]: opened" << endl;
            log << "[ss]: opened" << endl;
        }

        // create data structure for current gsp
        cout << "[ss] start reading." << endl;
        log << "[ss] start reading." << endl;
        std::string current_token = "";
        gsp::gsp_element cur_element;
        cur_element.start_time = LLONG_MAX;
        cur_element.end_time = LLONG_MIN;

        gsp::slong element_count = 1;        //DEBUG
        while( iss >> current_token )
        {
            cout << "Current element index: " << element_count << endl;
            cout << "Length of GSP (vector of elements): " << current_gsp.size() << endl;
            cout << "[token]: " << current_token << endl;
            log << "Current element index: " << element_count << endl;
            log << "Length of GSP (vector of elements): " << current_gsp.size() << endl;
            log << "[token]: " << current_token << endl;
//            cout << " Type of gsp: " << typeid(current_gsp).name() << endl;       //DEBUG
            if( is_number( current_token ) )
            {
                cout << "[token type]: number" << endl;
                log << "[token type]: number" << endl;
                cur_element.items.insert( std::make_pair( StringToNumber<unsigned long>( current_token ), -1 ) );
                cout << "current element's items: ";
                log << "current element's items: ";
                for (std::map<gsp::slong,gsp::slong>::iterator item_it = cur_element.items.begin();
                     item_it != cur_element.items.end();
                     ++item_it)
                {
                    cout << item_it->first << " ";
                    log << item_it->first << " ";
                }
                cout << endl;
                log << endl;
            }
            else if(current_token == "<(")
            {
                cout << "[token type]: sequence beginning" << endl;
                cout << "next iteration" << endl;
                log << "[token type]: sequence beginning" << endl;
                log << "next iteration" << endl;
                continue;
            }
            else
            {
                cout << "[token type]: ')(' or ')>' " << endl;
                cout << "=> end of element" << endl;
                cout << "=> push element to the vector." << endl;
                log << "[token type]: ')(' or ')>' " << endl;
                log << "=> end of element" << endl;
                log << "=> push element to the vector." << endl;
                Sleep( 100 );
                current_gsp.push_back( cur_element );
                cout << "Number of items in element #" << element_count << ": " << cur_element.items.size() << endl;
                cout << "The same but from gsp vector" << current_gsp.size() << ": " << current_gsp[current_gsp.size()-1].items.size() << endl;
                log << "Number of items in element #" << element_count << ": " << cur_element.items.size() << endl;
                log << "The same but from gsp vector" << current_gsp.size() << ": " << current_gsp[current_gsp.size()-1].items.size() << endl;
                cur_element.items.clear();
                cout << "current element size after clear: " << cur_element.items.size() << " (expected 0)" << endl;
                log<< "current element size after clear: " << cur_element.items.size() << " (expected 0)" << endl;
                ++element_count;
            }
        }

        // show current GSP (from created structure)
        cout << "length of current GSP (number of elements): " << current_gsp.size() << endl;
        cout << "Current GSP (from structure): ";
        log << "length of current GSP (number of elements): " << current_gsp.size() << endl;
        log << "Current GSP (from structure): ";
        show_gsp(current_gsp, log);
        cout << endl;
        log << endl;

    /* --------------------- read current CId & search GSP in its data -------------------------------- */
        // data input
        ifstream infile( in_filename.c_str() );
        if( !infile )
        {
            cout << "[Error]: failed to open file " + in_filename << endl;
            log << "[Error]: failed to open file " + in_filename << endl;
            throw exception();
        }

        if( verbose ) {
                cout << "---- reading CIds ----" << endl;
                log << "---- reading CIds ----" << endl;
        }
        gsp::slong cid = 0,
                   tid = 0,
                   items_num = 0,
                   last_cid = -1,
                   cur_support = 0;
        std::set<gsp::slong> itemset;

        while( std::getline( infile, cur_cid_line ) )
        {
            std::istringstream iss( cur_cid_line );
            if(!iss) {
                cout << "[Error]: failed to create string stream from line." << endl;
                cout << "[Error]: in reading dataset" << endl;
                log << "[Error]: failed to create string stream from line." << endl;
                log << "[Error]: in reading dataset" << endl;
//                sleep(1);
                throw exception();
            }
            else {
                cout << "[dataset] Openning string stream" << endl;
                log << "[dataset] Openning string stream" << endl;
            }

            iss >> cid;
            cout << "cid: " << cid << " | last_cid: " << last_cid << endl;
            log << "cid: " << cid << " | last_cid: " << last_cid << endl;
            if( last_cid == -1 )
            {
                last_cid = cid;
                cout << "last_cid was -1 and now is changed to " << last_cid << " (the first CId in dataset)" << endl;
                log << "last_cid was -1 and now is changed to " << last_cid << " (the first CId in dataset)" << endl;
            }
            if( cid != last_cid  ) {
                cout << "OTHER Cid! SUPPORT COUNTING ROUTING" << endl;
                cout << "***********************************GSP check for previous CId" << endl;
                cout << "-- passing arguments:" << endl;
                cout << "----current_gsp:      " << typeid(current_gsp).name() << " (size) " << current_gsp.size() << endl;
                cout << "----current_cid_data: " << typeid(current_cid_data).name() << " (size) " << current_cid_data.size() << endl;
                cout << "----winSize:          " << winSize << endl;
                cout << "----minGap:           " << minGap << endl;
                cout << "----maxGap:           " << maxGap << endl;
                log << "OTHER Cid! SUPPORT COUNTING ROUTING" << endl;
                log << "***********************************GSP check for previous CId" << endl;
                log << "-- passing arguments:" << endl;
                log << "----current_gsp:      " << typeid(current_gsp).name() << " (size) " << current_gsp.size() << endl;
                log << "----current_cid_data: " << typeid(current_cid_data).name() << " (size) " << current_cid_data.size() << endl;
                log << "----current_cid_data(size): " << current_cid_data.size() << endl;
                log << "----winSize:          " << winSize << endl;
                log << "----minGap:           " << minGap << endl;
                log << "----maxGap:           " << maxGap << endl;
                if( checkGSP( current_gsp, current_cid_data, winSize, minGap, maxGap, log ) ) {
                    cout << "GSP is in this Cid. cid =  " << last_cid << endl;
                    log << "GSP is in this Cid. cid =  " << last_cid << endl;
                    ++cur_support;
                }
                cout << "***********************************END of GSP check" << endl;
                cout << "support - " << cur_support << endl;
                log << "***********************************END of GSP check" << endl;
                log << "support - " << cur_support << endl;
                //clean and move to next Cid
                current_cid_data.clear();
            }

            iss >> tid >> items_num;
            current_token = "";

            while( iss >> current_token )
            {
                cout << "current_token (item): " << current_token << endl;
                log << "current_token (item): " << current_token << endl;
                itemset.insert( StringToNumber<gsp::slong>( current_token ) );
            }

            cout << "items to insert:";
            log << "items to insert:";
            for(set<gsp::slong>::const_iterator iter = itemset.begin(); iter != itemset.end(); ++iter)
            {
                cout << " " << *iter;
                log << " " << *iter;
            }
            cout << endl;
            log << endl;

            //push into structure
            current_cid_data.insert( std::make_pair( tid,itemset ) );
            cout << "itemset pushed: ";
            log << "itemset pushed: ";
            for(const auto &item : current_cid_data.at(tid))
            {
                cout << item << " ";
                log << item << " ";
            }
            cout << endl;
            cout << "==check current_cid_data==" << endl;
            log << endl;
            log << "==check current_cid_data==" << endl;
            for( map<gsp::slong, set<gsp::slong>>::const_iterator m_it = current_cid_data.begin(); m_it != current_cid_data.end(); ++m_it )
            {
                cout << "cid: " << cid << " | last_cid: " << last_cid << " | tid: " << m_it->first << " | ";
                cout << "items:";
                log << "cid: " << cid << " | last_cid: " << last_cid << " | tid: " << m_it->first << " | ";
                log << "items:";
                for(set<gsp::slong>::const_iterator item_iter = m_it->second.begin(); item_iter != m_it->second.end(); ++item_iter)
                {
                    cout << " " << *item_iter;
                    log << " " << *item_iter;
                }
                cout << endl;
                log << endl;
            }
            cout << "==========================" << endl << endl;
            log << "==========================" << endl << endl;
//            sleep(1);
            last_cid = cid;
            itemset.clear();
        }

        //count for the last cid
            cout << "last Cid! SUPPORT COUNTING ROUTING" << endl;
            cout << "***********************************GSP check for last CId" << endl;
            cout << "-- passing arguments:" << endl;
                cout << "----current_gsp:      " << typeid(current_gsp).name() << " (size) " << current_gsp.size() << endl;
                cout << "----current_cid_data: " << typeid(current_cid_data).name() << " (size) " << current_cid_data.size() << endl;
                cout << "----winSize:          " << winSize << endl;
                cout << "----minGap:           " << minGap << endl;
                cout << "----maxGap:           " << maxGap << endl;
            log << "last Cid! SUPPORT COUNTING ROUTING" << endl;
            log << "***********************************GSP check for last CId" << endl;
            log << "-- passing arguments:" << endl;
                log << "----current_gsp:      " << typeid(current_gsp).name() << " (size) " << current_gsp.size() << endl;
                log << "----current_cid_data: " << typeid(current_cid_data).name() << " (size) " << current_cid_data.size() << endl;
                log << "----current_cid_data(size): " << current_cid_data.size() << endl;
                log << "----winSize:          " << winSize << endl;
                log << "----minGap:           " << minGap << endl;
                log << "----maxGap:           " << maxGap << endl;
            if( checkGSP( current_gsp, current_cid_data, winSize, minGap, maxGap, log ) ) {
                cout << "GSP is in this Cid. cid =  " << cid << endl;
                log << "GSP is in this Cid. cid =  " << cid << endl;
                ++cur_support;
            }
            cout << "***********************************END of GSP check for last CId" << endl;
            cout << "support - " << cur_support << endl;
            log << "***********************************END of GSP check for last CId" << endl;
            log << "support - " << cur_support << endl;
            //clean and move to next Cid
            current_cid_data.clear();

        //write to file
        outseqfile << cur_gsp_line << endl << "support - " << cur_support << endl;

        //clean current gsp structure
        current_gsp.clear();

        //close the data file to reopen it in the next iteration
        infile.close();
    }




/** ========================= check minSup and remove infrequent GSPs =============================== **/
    cout << "=== Removing infrequent GSPs===" << endl;
    // frequent sequences
    ofstream outfreqseqfile( out_freq_seq_filename.c_str(), ios::trunc | ios::out );
    if( !outfreqseqfile )
    {
        cout << "[Error]: failed to open file " + out_freq_seq_filename << endl;
        log << "[Error]: failed to open file " + out_freq_seq_filename << endl;
        throw exception();
    }

    // sequences with support output
    ifstream inseqsupportfile( out_cand_seq_filename.c_str() );
    if( !inseqsupportfile )
    {
        cout << "[Error]: failed to open file " + out_cand_seq_filename << endl;
        log << "[Error]: failed to open file " + out_cand_seq_filename << endl;
        throw exception();
    }

    std::string cur_support_line = "";
    while( std::getline( inseqsupportfile, cur_gsp_line ) )
    {
        /** TODO: add fault tolerance */
        if( !std::getline( inseqsupportfile, cur_support_line ) )
        {
            cout << "[Error] reading line with support" << endl;
            continue;
        }

        cout << "GSP: " << cur_gsp_line << endl;
        cout << "support: " << cur_support_line << endl;
        Sleep( 500 );

        std::istringstream iss_sup_line( cur_support_line  );
        if( !iss_sup_line ) {
            cout << "[Error]: failed to create string stream from line." << endl;
            cout << "[Error]: in reading supports" << endl;
            log << "[Error]: failed to create string stream from line." << endl;
            log << "[Error]: in reading supports" << endl;
            Sleep( 500 );
            throw exception();
        }
        std::string cur_token = "";
        while( iss_sup_line >> cur_token )
        {
            cout << "[token]: " << cur_token << endl;
            if( is_number( cur_token )) {
                    cout << "[minSup] token is number" << endl;
//                    cout << "str2num: result - " << (unsigned long)StringToNumber<unsigned long>( cur_token ) << endl;
//                    cout << "minSup: result - " << minSup << endl;
//                if(( (unsigned long)StringToNumber<unsigned long>( cur_token ) > (unsigned long)minSup ) )
                    if(( StringToNumber<gsp::slong>( cur_token ) > minSup ) )
                {
                    cout << "===[minSup] support is sufficient" << endl;
                    outfreqseqfile << cur_gsp_line << endl;
                    outfreqseqfile << "support - " + cur_token << endl;
                    break;
                }
            }
        }
    }
    cout << "[minSup] ENDED ====" << endl;
     //close file streams
    inparamsfile.close();
    inseqfile.close();
    outseqfile.close();
    log.close();
    outfreqseqfile.close();
    inseqsupportfile.close();

}

void show_gsp(const vector<gsp::gsp_element> &gsp, std::ofstream& logFile) {
    cout << "ITEMS: <";
    logFile << "ITEMS: <";
    for(vector<gsp::gsp_element>::const_iterator gsp_iter = gsp.begin(); gsp_iter != gsp.end(); ++gsp_iter) {
        cout << "(";
        logFile << "(";
//        cout << "[" <<typeid(*gsp_iter).name() << "]";
        for(std::map<gsp::slong,gsp::slong>::const_iterator item_iter = gsp_iter->items.begin(); item_iter != gsp_iter->items.end(); ++item_iter) {
            cout << " " << item_iter->first;
            logFile << " " << item_iter->first;
        }
        cout << " )";
        logFile << " )";
    }
    cout << ">" << endl;
    logFile << ">" << endl;
}




bool checkGSP( std::vector<gsp::gsp_element> &gsp, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
               gsp::slong &winSize, gsp::slong &minGap, gsp::slong &maxGap, std::ofstream& logFile)
{
    cout << "[== checkGSP() function called ==]" << endl;
    cout << "-- got parameters:" << endl;
    cout << "---- &gsp: <type>: " << typeid(gsp).name() << " <size>: "<< gsp.size() << " elements:" << endl;
    logFile << "[== checkGSP() function called ==]" << endl;
    logFile << "-- got parameters:" << endl;
    logFile << "---- &gsp: <type>: " << typeid(gsp).name() << " <size>: "<< gsp.size() << " elements:" << endl;
    for( unsigned i = 0; i < gsp.size(); ++i )
    {
        cout << "------ <elem_index>" << i << " <type>: " << typeid(gsp[i]).name();
        logFile << " <start_time>: " << gsp[i].start_time << " <end_time>: " << gsp[i].end_time << " <items.size>: " << gsp[i].items.size() << endl;
        for( std::map<gsp::slong,gsp::slong>::iterator j = gsp[i].items.begin(); j != gsp[i].items.end(); ++j )
        {
            cout << "-------- <type>: " << typeid(*j).name() << " <item.key>: " << j->first << " <item.value (time)>: " << j->second << endl;
            logFile << "-------- <type>: " << typeid(*j).name() << " <item.key>: " << j->first << " <item.value (time)>: " << j->second << endl;
        }
    }
    cout << "---- &cid_data: <type>: " << typeid( cid_data ).name() << " <size>: "<< cid_data.size() << " elements:" << endl;
    logFile << "---- &cid_data: <type>: " << typeid( cid_data ).name() << " <size>: "<< cid_data.size() << " elements:" << endl;
    for( std::map<gsp::slong,std::set<gsp::slong>>::const_iterator i = cid_data.begin(); i != cid_data.end(); ++i )
    {
        cout << "------ <type>: " << typeid(*i).name() << " <tid>:" << i->first;
        cout << " <itemset.type>: " << typeid(i->second).name() << " <itemset.size>: " << (i->second).size() << endl;
        logFile << "------ <type>: " << typeid(*i).name() << " <tid>:" << i->first;
        logFile << " <itemset.type>: " << typeid(i->second).name() << " <itemset.size>: " << (i->second).size() << endl;
        for( std::set<gsp::slong>::const_iterator j = (i->second).begin(); j != (i->second).end(); ++j )
        {
            cout << "-------- <type>: " << typeid( *j ).name() << " <value>: " << *j << endl;
            logFile << "-------- <type>: " << typeid( *j ).name() << " <value>: " << *j << endl;
        }
    }
    cout << "----= winSize: " << winSize << endl;
    cout << "----= minGap: " << minGap << endl;
    cout << "----= maxGap: " << maxGap << endl;
    logFile << "----= winSize: " << winSize << endl;
    logFile << "----= minGap: " << minGap << endl;
    logFile << "----= maxGap: " << maxGap << endl;
    gsp::slong startTime = 0;
    // iterating through elements in GSP
    for( unsigned i = 0; i < gsp.size(); ++i ) {
        //forward faze
        cout << "(checkGsp)[forward faze]: iteration: i = " << i << "; \n gsp_element: start_time = " << gsp[i].start_time;
        cout << " end_time = " << gsp[i].end_time << " num of itemsets = " << gsp[i].items.size() << endl;
        cout << "startTime to pass to checkElement(): " << startTime << endl;
        logFile << "(checkGsp)[forward faze]: iteration: i = " << i << "; \n gsp_element: start_time = " << gsp[i].start_time;
        logFile << " end_time = " << gsp[i].end_time << " num of itemsets = " << gsp[i].items.size() << endl;
        logFile<< "startTime to pass to checkElement(): " << startTime << endl;
        if( !checkElement( gsp[i], cid_data, startTime, winSize, logFile ) ) {
            cout << "(checkGsp)[forward faze]Element " << i << " not found => RETURN false" << endl;
            logFile << "(checkGsp)[forward faze]Element " << i << " not found => RETURN false" << endl;
            return false;
        }
        else if(i == 0) {
            cout << "(checkGsp)Element 0  => continue" << endl;
            logFile << "(checkGsp)Element 0  => continue" << endl;
            continue;
        }
        if( ( gsp[i].end_time - gsp[i-1].start_time ) > maxGap ) {
            // backward faze
            cout << "(checkGsp)maxGap criterion is not satisfied => backward faze" << endl;
            logFile << "(checkGsp)maxGap criterion is not satisfied => backward faze" << endl;
            for(int j = i; 0 < j; ++j) {
                cout << "(checkGsp)[backward faze]: iteration: " << j << endl;
                logFile << "(checkGsp)[backward faze]: iteration: " << j << endl;
                if( !checkElement( gsp[j-1], cid_data, gsp[j].end_time - maxGap, winSize, logFile ) ) {
                    cout << "(checkGsp)[backward faze]Element " << j << " not found => RETURN false" << endl;
                    logFile << "(checkGsp)[backward faze]Element " << j << " not found => RETURN false" << endl;
                    return false;
                }
            }
            i = 0;
        }
        startTime = gsp[i].end_time + minGap;
        cout << "(checkGsp)new startTime: " << startTime << endl;
        logFile << "(checkGsp)new startTime: " << startTime << endl;
    }
    return true;
}

bool checkElement( gsp::gsp_element &element, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
                   gsp::slong startTime, gsp::slong &winSize, std::ofstream& logFile) {
    cout << "[== checkElement() function called ==]" << endl;
    cout << "(checkElement)params got: " << endl;
    cout << "(checkElement)--startTime (to search from) " << startTime << endl;
    cout << "(checkElement)--element.start_time: " << element.start_time << endl;
    cout << "(checkElement)--element.end_time: " << element.end_time << endl;
    logFile << "[== checkElement() function called ==]" << endl;
    logFile << "(checkElement)params got: " << endl;
    logFile << "(checkElement)--startTime (to search from) " << startTime << endl;
    logFile << "(checkElement)--element.start_time: " << element.start_time << endl;
    logFile << "(checkElement)--element.end_time: " << element.end_time << endl;
    while( true )
    {
        // iterate through items in element
        for(std::map<gsp::slong,gsp::slong>::iterator item_iter = element.items.begin(); item_iter != element.items.end(); ++item_iter)
        {
            cout << "(checkElement)checking item '" << item_iter->first << "', time: " << item_iter->second << endl;
            logFile<< "(checkElement)checking item '" << item_iter->first << "', time: " << item_iter->second << endl;
            bool item_found = false;
            // iterate through TIds in data for current CId
            for( std::map<gsp::slong,std::set<gsp::slong>>::const_iterator cid_data_iter = cid_data.begin();
                 cid_data_iter != cid_data.end(); ++cid_data_iter )
            {
                cout << "(checkElement)comparing with cid_data\ntid | items" << endl;
                cout << cid_data_iter->first << " | ";
                logFile << "(checkElement)comparing with cid_data\ntid | items" << endl;
                logFile << cid_data_iter->first << " | ";
                for( std::set<gsp::slong>::const_iterator set_item_iter = (cid_data_iter->second).begin();
                     set_item_iter != (cid_data_iter->second).end(); ++set_item_iter )
                {
                    cout << *set_item_iter << " ";
                    logFile << *set_item_iter << " ";
                }
                cout << endl << "(checkElement)-----" << endl;
                logFile << endl << "(checkElement)-----" << endl;
                if( cid_data_iter->first < startTime )
                {
                    cout << "(checkElement)startTime > tid => No sense to check => continue" << endl;
                    logFile << "(checkElement)startTime > tid => No sense to check => continue" << endl;
                    continue;
                }

                // iterate through items in itemset of current TId
                for( std::set<gsp::slong>::const_iterator tidlist_iter = cid_data_iter->second.begin();
                     tidlist_iter != cid_data_iter->second.end(); ++tidlist_iter )
                {
                    cout << "(checkElement)\n--inner loop: finding the match of elem's item " << item_iter->first;
                    cout << " with item " << *tidlist_iter << "in data" << endl;
                    logFile << "(checkElement)\n--inner loop: finding the match of elem's item " << item_iter->first;
                    logFile << " with item " << *tidlist_iter << "in data" << endl;

                    if( *tidlist_iter == item_iter->first )
                    {
                        //if such item in current_cid_data found, update time for item in element
                        cout << "Item found. Check next item in current element" << endl;
                        logFile << "Item found. Check next item in current element" << endl;
                        item_found = true;
                        item_iter->second = cid_data_iter->first;
                        element.start_time = (*tidlist_iter < element.start_time) ? *tidlist_iter : element.start_time;
                        element.end_time   = ( element.end_time < *tidlist_iter ) ? *tidlist_iter : element.end_time;
                        break;
                    }
                }
                if( item_found ) { break; }
            }
            if( !item_found ) {
                return false;
            }
        }

        // check window size & correct startTime if needed
        if( ( element.end_time - element.start_time ) <= winSize )
        {
            break;
        }
        else
        {
            startTime = element.end_time - winSize;
        }
    }
    return true;
}
//
//template <typename T>
//T StringToNumber ( const std::string &Text )
//{
//    istringstream ss(Text);
//    T result;
//    return ss >> result ? result : 0;
//}
