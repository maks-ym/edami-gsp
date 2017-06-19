// 'output' and 'temp' directory must be created manually

#include <iostream>
#include <time.h>
#include "postprocessing.h"
#include "preprocessing.h"
#include "support.h"

using namespace std;

int main( int argc, char* argv[] )
{
    if( argc < 3 )
    {
        std::cerr << "Usage: " << argv[0] << " dataset_filename parameters_filename" << std::endl;
        return 1;
    }

    std::string dataset_filename = argv[1];
    std::string parameters_filename = argv[2];

    // timestamp
    time_t _tm = time( NULL ); struct tm * curtime = localtime ( &_tm );
    std::cout << "============== "   << asctime( curtime );
    std::cout<< "----------------------------------------------------------" << endl;

    //preprocessing
    preprocessing( dataset_filename );

    //tree
    // Some magic with hash tree to create candidate sequences (MOCK CANDIDATES USED)

    //support count
    support( dataset_filename, parameters_filename );

    //postprocessing
    postprocessing( dataset_filename );

    cout << "The result is in /output/ directory" << endl;
    system("pause");
    return 0;
}
