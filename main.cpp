/// UnBeatables - LARA
//g++ -ggdb `pkg-config --cflags opencv` -o `basename main.cpp .cpp` main.cpp `pkg-config --libs opencv`./

#include "controller.hpp"

using namespace std;

/*void parseOpt(string *naoBrokerIP, int *naoBrokerPort, int argc, char* argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Print help mesages.")
        ("pip", po::value<string>(naoBrokerIP)->default_value("127.0.0.1"), "IP of the parent broker. Default: 127.0.0.1")
        ("pport", po::value<int>(naoBrokerPort)->default_value(9559),"Port of the parent broker. Default: 9559");
    po::positional_options_description positionalOptions;
    po::variables_map vm; // Map containing all the options with their values
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), vm);

        if (vm.count("help"))
        {
            cout << "Video Recorder Program"                                   << endl;
            cout << "Can be executed locally or remotelly."                   << endl;
            cout                                                              << endl;
            cout << "Usage:"                                                  << endl;
            cout << "  " << argv[0] << " [options] [input] "                  << endl;
            cout                                                              << endl;
            cout << desc                                                      << endl;

            exit(0);
        }

        po::notify(vm);
    }
    catch(po::error &e)
    {
        cerr << e.what() << endl;
        cout << desc << endl;
        exit(1);
    }
}*/

int main(int argc, char* argv[])
{
    string parentBrokerIP;
    int parentBrokerPort;

    //setlocale(LC_NUMERIC, "C"); // Need this to for SOAP serialization of floats to work

    //parseOpt(&parentBrokerIP, &parentBrokerPort, argc, argv);

    cout << "parse created..." << endl;

    Controller controller(argc, argv);

    return 0;
}
