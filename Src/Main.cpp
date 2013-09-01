#ifdef RPI
#include <bcm_host.h>
#endif

#include <string>
#include <iostream>
using namespace std;

void run_app();

int main(int argc, char **argv) {
    try {
#ifdef RPI
        bcm_host_init();
#endif
        run_app();
        return 0;
    } catch (std::string & err) {
        cerr << "Caught exception" << endl;
        cerr << err;
        return -1;
    }
}
