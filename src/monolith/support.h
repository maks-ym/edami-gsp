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
void show_gsp( const std::vector<gsp::gsp_element> &gsp );
bool checkGSP( std::vector<gsp::gsp_element> &gsp, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
              gsp::slong &winSize, gsp::slong &minGap, gsp::slong &maxGap );
bool checkElement( gsp::gsp_element &element, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
                   gsp::slong startTime, gsp::slong &winSize );
bool is_number(const std::string& s);
void resetElementsTimes( std::vector<gsp::gsp_element> &gsp );

template <typename T> T StringToNumber ( const std::string &Text );

void support( std::string dataset_filename, std::string params_filename )
{
//    ofstream log( ("../../outputs/SUPPORT_LOG_" + dataset_filename).c_str(), ios::trunc | ios::out );
//    if( !log )
//    {
//        cout << "(support)[Error]: failed to open file ../../outputs/SUPPORT_LOG_" + dataset_filename << endl;
//        throw exception();
//    }

    std::cout << "=============== SUPPORT ===============" << endl;
/** ===================================== initialize vars =============================================== **/
    std::string in_filename = "temp/" + dataset_filename + "_processed.txt",
           in_params_filename = "params/" + params_filename,
           in_cand_seq_filename = "temp/" + dataset_filename + "_candidate_sequences.txt",
           out_cand_seq_filename = "temp/" + dataset_filename + "_num_cand_seq_with_sup.txt",
           out_freq_seq_filename = "temp/" + dataset_filename + "_num_freq_seq.txt";

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
        std::cout << "(support)[Error]: failed to open file " + in_params_filename << endl;
        throw exception();
    }

    // params input
    ifstream inseqfile( in_cand_seq_filename.c_str() );
    if( !inseqfile )
    {
        std::cout << "(support)[Error]: failed to open file " + in_cand_seq_filename << endl;
        throw exception();
    }

    // sequences with support output
    ofstream outseqfile( out_cand_seq_filename.c_str(), ios::trunc | ios::out );
    if( !outseqfile )
    {
        std::cout << "(support)[Error]: failed to open file " + out_cand_seq_filename << endl;
        throw exception();
    }

/** ============================ read parameters (winSize, minGap, maxGap) ============================== **/
    std::cout << "(support) -------- parameters: --------" << endl;
    unsigned num_of_params = 4;
    gsp::slong params[num_of_params] = { 0 };
    for( unsigned i = 0; i < num_of_params; ++i )
    {
        std::getline( inparamsfile, cur_param_line );
        std::istringstream iss( cur_param_line );
        if( !iss )
        {
            std::cout << "(support)[Error]: failed to create string stream from cur_param_line." << endl;
            std::cout << "(support)[Error]: in reading sequences" << endl;
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
    std::cout << "(support) - winSize: " << winSize << endl;
    std::cout << "(support) - minGap:  " << minGap  << endl;
    std::cout << "(support) - maxGap:  " << maxGap  << endl;
    std::cout << "(support) - minSup:  " << minSup  << endl;


/** ======================== MAIN LOOP ( read & check support of GSP one-by-one ) ======================== */
    while( std::getline( inseqfile, cur_gsp_line ) )
    {
    /* ------------------------------------------- read gsp ------------------------------------------- */
        std::cout << "(support) --------  reading GSP --------"  << endl;

        if( cur_gsp_line.length() < 4 ) {
            std::cout << "(support) [Warning]: too short line. => Skip it." << endl;
            continue;
        }

        std::cout << "(support) Current GSP (read line): " << cur_gsp_line << endl;
        Sleep( 500 );

        std::istringstream iss( cur_gsp_line );
        if( !iss ) {
            std::cout << "(support) [Error]: failed to create string stream from line." << endl;
            std::cout << "(support) [Error]: in reading sequences" << endl;
            Sleep( 500 );
            throw exception();
        }
        else {
            std::cout << "(support) [stringstream]: opened" << endl;
        }

        // create data structure for current gsp
        std::cout << "(support) [stringstream] start reading." << endl;
        std::string current_token = "";
        gsp::gsp_element cur_element;
        cur_element.start_time = LLONG_MAX;
        cur_element.end_time = LLONG_MIN;

        gsp::slong element_count = 1;        //DEBUG
        while( iss >> current_token )
        {
            std::cout << "(support) Current element index: " << element_count << endl;
            std::cout << "(support) Length of GSP (vector of elements): " << current_gsp.size() << endl;
            std::cout << "(support) [token]: " << current_token;
//            cout << " Type of gsp: " << typeid(current_gsp).name() << endl;       //DEBUG
            if( is_number( current_token ) )
            {
                std::cout << "=> number" << endl;
                cur_element.items.insert( std::make_pair( StringToNumber<unsigned long>( current_token ), -1 ) );
                std::cout << "current element's items: ";
                for (std::map<gsp::slong,gsp::slong>::iterator item_it = cur_element.items.begin();
                     item_it != cur_element.items.end();
                     ++item_it)
                {
                    std::cout << item_it->first << " ";
                }
                std::cout << endl;
            }
            else if(current_token == "<(")
            {
                std::cout << "=> sequence beginning => next iteration" << endl;
                continue;
            }
            else
            {
                std::cout << "=> ')(' or ')>' => end of element => push element to the vector." << endl;
                Sleep( 100 );
                current_gsp.push_back( cur_element );
                std::cout << "(support) Number of items in element #" << element_count << ": " << cur_element.items.size() << endl;
                std::cout << "(support) The same but from gsp vector" << current_gsp.size() << ": " << current_gsp[current_gsp.size()-1].items.size() << endl;
                cur_element.items.clear();
                std::cout<< "(support) current element size after clear: " << cur_element.items.size() << " (expected 0)" << endl;
                ++element_count;
            }
        }

        // show current GSP (from created structure)
        std::cout << "(support)length of current GSP (number of elements): " << current_gsp.size() << endl;
        std::cout << "(support)Current GSP (from structure): ";
        show_gsp(current_gsp);
        std::cout << endl;

    /* --------------------- read current CId & search GSP in its data -------------------------------- */
        // data input
        ifstream infile( in_filename.c_str() );
        if( !infile )
        {
            std::cout << "(support)[Error]: failed to open file " + in_filename << endl;
            throw exception();
        }

        std::cout << "(support) ---- reading CIds ----" << endl;
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
                std::cout << "(support)[Error]: failed to create string stream from line." << endl;
                std::cout << "(support)[Error]: in reading dataset" << endl;
//                sleep(1);
                throw exception();
            }
            else {
                std::cout << "(support)[dataset] Openning string stream" << endl;
            }

            iss >> cid;
            std::cout << "(support) cid: " << cid << " | last_cid: " << last_cid << endl;
            if( last_cid == -1 )
            {
                last_cid = cid;
                std::cout << "(support) last_cid was -1 and now is changed to " << last_cid << " (the first CId in dataset)" << endl;
            }
            if( cid != last_cid  ) {
                /** TODO: debug log mechanism */
//                log << "(support)OTHER Cid! SUPPORT COUNTING ROUTING" << endl;
//                log << "(support)***********************************GSP check for previous CId" << endl;
//                log << "(support)-- passing arguments:" << endl;
//                log << "(support)----current_gsp:      " << typeid(current_gsp).name() << " (size) " << current_gsp.size() << endl;
//                log << "(support)----current_cid_data: " << typeid(current_cid_data).name() << " (size) " << current_cid_data.size() << endl;
//                log << "(support)----winSize:          " << winSize << endl;
//                log << "(support)----minGap:           " << minGap << endl;
//                log << "(support)----maxGap:           " << maxGap << endl;
                if( checkGSP( current_gsp, current_cid_data, winSize, minGap, maxGap ) ) {
                    // log << "(support) GSP is in this Cid. cid =  " << last_cid << endl; /** TODO: debug log mechanism */
                    ++cur_support;
                }
                // log << "(support) reset GSP elements time" << endl; /** TODO: debug log mechanism */
                resetElementsTimes( current_gsp );
//                log << "(support)***********************************END of GSP check" << endl; /** TODO: debug log mechanism */
//                log << "(support)support - " << cur_support << endl;
                //clean and move to next Cid
                current_cid_data.clear();
            }

            iss >> tid >> items_num;
            current_token = "";

            while( iss >> current_token )
            {
                std::cout << "(support) current_token (item): " << current_token << endl;
                itemset.insert( StringToNumber<gsp::slong>( current_token ) );
            }

            std::cout << "(support)items to insert:";
            for(set<gsp::slong>::const_iterator iter = itemset.begin(); iter != itemset.end(); ++iter)
            {
                std::cout << " " << *iter;
            }
            std::cout << endl;

            //push into structure
            current_cid_data.insert( std::make_pair( tid,itemset ) );
            std::cout << "(support) itemset pushed: ";
            for(const auto &item : current_cid_data.at(tid))
            {
                std::cout << item << " ";
            }
            std::cout << endl;
            std::cout << "(support) ==check current_cid_data==" << endl;
            for( map<gsp::slong, set<gsp::slong>>::const_iterator m_it = current_cid_data.begin(); m_it != current_cid_data.end(); ++m_it )
            {
                std::cout << "(support) cid: " << cid << " | last_cid: " << last_cid << " | tid: " << m_it->first << " | ";
                std::cout << "items:";
                for(set<gsp::slong>::const_iterator item_iter = m_it->second.begin(); item_iter != m_it->second.end(); ++item_iter)
                {
                    std::cout << " " << *item_iter;
                }
                std::cout << endl;
            }
            std::cout << "(support) ==========================" << endl << endl;
//            sleep(1);
            last_cid = cid;
            itemset.clear();
        }

        //count for the last cid
        /** TODO: debug log mechanism */
//        log << "(support)last Cid! SUPPORT COUNTING ROUTING" << endl;
//        log << "(support)***********************************GSP check for last CId" << endl;
//        log << "(support)-- passing arguments:" << endl;
//        log << "(support)----current_gsp:      " << typeid(current_gsp).name() << " (size) " << current_gsp.size() << endl;
//        log << "(support)----current_cid_data: " << typeid(current_cid_data).name() << " (size) " << current_cid_data.size() << endl;
//        log << "(support)----winSize:          " << winSize << endl;
//        log << "(support)----minGap:           " << minGap << endl;
//        log << "(support)----maxGap:           " << maxGap << endl;
        if( checkGSP( current_gsp, current_cid_data, winSize, minGap, maxGap ) )
        {
//            log << "(support) GSP is in this Cid. cid =  " << cid << endl; /** TODO: debug log mechanism */
            ++cur_support;
        }
        std::cout << "(support)***********************************END of GSP check for last CId" << endl;
        std::cout << "(support)support - " << cur_support << endl;
        //clean and move to next Cid
        current_cid_data.clear();

        //write to file
        outseqfile << cur_gsp_line << "\t[ " << cur_support << " ]" << endl;

        //clean current gsp structure
        current_gsp.clear();

        //close the data file to reopen it in the next iteration
        infile.close();
    }


/** ========================= check minSup and remove infrequent GSPs =============================== **/

    std::cout << "(support)=== Removing infrequent GSPs ===" << endl;
    // frequent sequences
    ofstream outfreqseqfile( out_freq_seq_filename.c_str(), ios::trunc | ios::out );
    if( !outfreqseqfile )
    {
        std::cout << "(support)[Error]: failed to open file " + out_freq_seq_filename << endl;
        throw exception();
    }

    // sequences with support output
    ifstream inseqsupportfile( out_cand_seq_filename.c_str() );
    if( !inseqsupportfile )
    {
        std::cout << "(support)[Error]: failed to open file " + out_cand_seq_filename << endl;
        throw exception();
    }

    std::string cur_line = "";
    while( std::getline( inseqsupportfile, cur_line ) )
    {
        std::cout << "(support)[infreq remove] GSP: " << cur_line;
        Sleep( 500 );

        std::istringstream iss_line( cur_line  );
        if( !iss_line ) {
            std::cout << "(support)[infreq remove][Error]: failed to create string stream from line." << endl;
            Sleep( 500 );
            throw exception();
        }
        std::string cur_token = "";
        while( iss_line >> cur_token )
        {
//            std::cout << "(support)[token]: " << cur_token << endl; /** TODO: debug log mechanism */
            if( cur_token == "[" )
            {
                iss_line >> cur_token;
                if( is_number( cur_token ) )
                {
//                    log << "(support)[minSup] token is number" << endl; /** TODO: debug log mechanism */
                    if(( StringToNumber<gsp::slong>( cur_token ) > minSup ) )
                    {
                        std::cout << " => sufficient" << endl;
                        //log << "(support) ===[minSup] support is sufficient" << endl; /** TODO: debug log mechanism */
                        outfreqseqfile << cur_line << endl;
                        break;
                    }
                    else
                    {
                        std::cout << " => insufficient => remove" << endl;
                    }
                }
            }
        }
    }
    std::cout << "(support)=== END of Removing infrequent GSPs ===" << endl;

    //close file streams
    inparamsfile.close();
    inseqfile.close();
    outseqfile.close();
    outfreqseqfile.close();
    inseqsupportfile.close();

}

void show_gsp(const vector<gsp::gsp_element> &gsp ) {
    /** TODO: debug log mechanism */
//    logFile << "(show_gsp)[==function called==]" << endl;
    std::cout << "(show_gsp)ITEMS: <";
    for(vector<gsp::gsp_element>::const_iterator gsp_iter = gsp.begin(); gsp_iter != gsp.end(); ++gsp_iter) {
        std::cout << "(";
        for(std::map<gsp::slong,gsp::slong>::const_iterator item_iter = gsp_iter->items.begin(); item_iter != gsp_iter->items.end(); ++item_iter) {
            std::cout << " " << item_iter->first;
        }
        std::cout << " )";
    }
    std::cout << ">" << endl;
}

bool checkGSP( std::vector<gsp::gsp_element> &gsp, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
               gsp::slong &winSize, gsp::slong &minGap, gsp::slong &maxGap )
{
//    std::cout << "(checkGSP)[== checkGSP() function called ==]" << endl;
//    logFile << "(checkGSP)-- got parameters:" << endl;
//    logFile << "(checkGSP)---- &gsp: <type>: " << typeid(gsp).name() << " <size>: "<< gsp.size() << " elements:" << endl;
    // empty GSP always present in CId (Technically GSP will have 1 element but with empty itemset)
    if( gsp[0].items.size() == 0 )
    {
        return true;
    }
    // if not empty should check it
    /** TODO: debug log mechanism */
//    for( unsigned i = 0; i < gsp.size(); ++i )
//    {
//        logFile << "(checkGSP) <start_time>: " << gsp[i].start_time << " <end_time>: " << gsp[i].end_time << " <items.size>: " << gsp[i].items.size() << endl;
//        for( std::map<gsp::slong,gsp::slong>::iterator j = gsp[i].items.begin(); j != gsp[i].items.end(); ++j )
//        {
//            logFile << "(checkGSP) -------- <type>: " << typeid(*j).name() << " <item.key>: " << j->first << " <item.value (time)>: " << j->second << endl;
//        }
//    }
//    logFile << "(checkGSP)---- &cid_data: <type>: " << typeid( cid_data ).name() << " <size>: "<< cid_data.size() << " elements:" << endl;
//    for( std::map<gsp::slong,std::set<gsp::slong>>::const_iterator i = cid_data.begin(); i != cid_data.end(); ++i )
//    {
//        logFile << "(checkGSP)------ <type>: " << typeid(*i).name() << " <tid>:" << i->first;
//        logFile << "(checkGSP) <itemset.type>: " << typeid(i->second).name() << " <itemset.size>: " << (i->second).size() << endl;
//        for( std::set<gsp::slong>::const_iterator j = (i->second).begin(); j != (i->second).end(); ++j )
//        {
//            logFile << "(checkGSP)-------- <type>: " << typeid( *j ).name() << " <value>: " << *j << endl;
//        }
//    }

    std::cout << "(checkGSP)----= winSize: " << winSize << endl;
    std::cout << "(checkGSP)----= minGap: " << minGap << endl;
    std::cout << "(checkGSP)----= maxGap: " << maxGap << endl;

    gsp::slong startTime = 0;

    // iterating through elements in GSP
    for( unsigned i = 0; i < gsp.size(); ++i ) {
        //forward faze
        std::cout << "(checkGsp)[forward faze]: iteration: i = " << i << endl;
        std::cout << "(checkGsp)[forward faze]: gsp_element: start_time = " << gsp[i].start_time;
        std::cout << " end_time = " << gsp[i].end_time << " num of items = " << gsp[i].items.size() << endl;
        std::cout<< "(checkGSP)startTime to pass to checkElement(): " << startTime << endl;

        startTime = ( i == 0 ) ? startTime : gsp[i-1].end_time + minGap + 1;
        std::cout << "(checkGsp)new startTime: " << startTime << endl;
        if( !checkElement( gsp[i], cid_data, startTime, winSize ) ) {
            std::cout << "(checkGsp)[forward faze]Element " << i << " not found => RETURN false" << endl;
            return false;
        }

        if( ( gsp[i].end_time - gsp[i-1].start_time ) > maxGap ) {
            // backward faze
            std::cout << "(checkGsp)maxGap criterion is not satisfied => backward faze" << endl;
            for(int j = i; 0 < j; ++j) {
                std::cout << "(checkGsp)[backward faze]: iteration: " << j << endl;
                if( !checkElement( gsp[j-1], cid_data, gsp[j].end_time - maxGap, winSize ) ) {
                    std::cout << "(checkGsp)[backward faze]Element " << j << " not found => RETURN false" << endl;
                    return false;
                }
            }
            i = 0;
        }
    }
    return true;
}

bool checkElement( gsp::gsp_element &element, const std::map<gsp::slong,std::set<gsp::slong>> &cid_data,
                   gsp::slong startTime, gsp::slong &winSize ) {
    std::cout << "(checkElement)[== function called ==]" << endl;
    std::cout << "(checkElement)params got: " << endl;
    std::cout << "(checkElement)--startTime (to search from) " << startTime << endl;
    std::cout << "(checkElement)--element.start_time: " << element.start_time << endl;
    std::cout << "(checkElement)--element.end_time: " << element.end_time << endl;
    while( true )
    {
        // iterate through items in element
        for(std::map<gsp::slong,gsp::slong>::iterator item_iter = element.items.begin(); item_iter != element.items.end(); ++item_iter)
        {
            std::cout<< "(checkElement) checking item '" << item_iter->first << "', time: " << item_iter->second << endl;
            bool item_found = false;
            // iterate through TIds in data for current CId
            for( std::map<gsp::slong,std::set<gsp::slong>>::const_iterator cid_data_iter = cid_data.begin();
                 cid_data_iter != cid_data.end(); ++cid_data_iter )
            {
                std::cout << "(checkElement) comparing with cid_data (tid | {items}): ";
                std::cout << "{ " << cid_data_iter->first << " | ";
                for( std::set<gsp::slong>::const_iterator set_item_iter = (cid_data_iter->second).begin();
                     set_item_iter != (cid_data_iter->second).end(); ++set_item_iter )
                {
                    std::cout << *set_item_iter << " ";
                }
                std::cout << "}" << endl;

                if( cid_data_iter->first < startTime )
                {
                    std::cout << "(checkElement) [startTime > tid] => No sense to check => next iteration" << endl;
                    continue;
                }

                // iterate through items in itemset of current TId
                for( std::set<gsp::slong>::const_iterator tidlist_iter = cid_data_iter->second.begin();
                     tidlist_iter != cid_data_iter->second.end(); ++tidlist_iter )
                {
                    std::cout << "(checkElement) --inner loop: finding the match of elem's item " << item_iter->first;
                    std::cout << " with item " << *tidlist_iter << "in data" << endl;

                    if( *tidlist_iter == item_iter->first )
                    {
                        //if such item in current_cid_data found, update time for item in element
                        std::cout << "(checkElement) Item found => Check next item in current element" << endl;
                        item_found = true;
                        item_iter->second = cid_data_iter->first;
                        element.start_time = (cid_data_iter->first < element.start_time) ? cid_data_iter->first: element.start_time;
                        element.end_time   = ( element.end_time < cid_data_iter->first ) ? cid_data_iter->first : element.end_time;
                        break;
                    }
                }
                if( item_found )
                {
                    break;
                }
            }
            if( !item_found )
            {
                return false;
            }
        }

        // check window size & correct startTime if needed
        std::cout << "(checkElement) winSize check. element.end_time " << element.end_time << "; element.start_time" << element.start_time;
        std::cout << " winSize " << winSize << endl;
        std::cout << "(checkElement) comparison: (end - start) <= winSize ? [" ;
        std::cout << ( element.end_time - element.start_time ) << " <= " << winSize << "]: ";
        std::cout << ( ( element.end_time - element.start_time ) <= winSize ) << endl;
        std::cout << "(checkElement) winSize check. element.end_time " << element.end_time << "; element.start_time" << element.start_time << endl;
        if( ( element.end_time - element.start_time ) <= winSize )
        {
            std::cout << "(checkElement) INSIDE IF STATEMENT!" << endl;
            break;
            std::cout << "(checkElement) After break (shouldn't be executed!!!)" << endl;
        }
        else
        {
            startTime = element.end_time - winSize;
            element.start_time = LLONG_MAX;
            element.end_time = LLONG_MIN;
        }
    }
    std::cout << "(checkElement) element found => return TRUE should occur" << endl;
    return true;
}

void resetElementsTimes( std::vector<gsp::gsp_element> &gsp )
{
    std::cout << "(resetElementsTimes)[GSP CLEANING]" << endl;

    for( std::vector<gsp::gsp_element>::iterator elem_iter = gsp.begin(); elem_iter != gsp.end(); ++elem_iter )
    {

        gsp::gsp_element elem = *elem_iter;
        std::cout << "(resetElementsTimes)elem.start_time was " << elem.start_time;
        (*elem_iter).start_time = LLONG_MAX;
        std::cout << "  become " << elem.start_time << endl;
        std::cout << "(resetElementsTimes)elem.end_time was " << elem.end_time;
        (*elem_iter).end_time  =  LLONG_MIN;
        std::cout << "  become " << elem.end_time << endl;
        for( std::map<gsp::slong, gsp::slong>::iterator item_iter = (*elem_iter).items.begin(); item_iter != (*elem_iter).items.begin(); ++item_iter )
        {
            std::cout << "item time was " << item_iter->second;
            item_iter->second = -1;
            std::cout << "become " << item_iter->second;
        }
    }
}
