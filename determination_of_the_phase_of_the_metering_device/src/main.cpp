#include "meters_data.h"
#include "input_reader.h"

using namespace std;

int main(int argc, char** argv) {
    //MeterVoltageProfilesIO::PhasingDataFromPostgreSQL<ElectricityMeter::Iskraemeco>(argc, argv);
    ifstream input("metersPQ14-12-2022.csv");
    MeterVoltageProfilesIO::PhasingDataFromCSV<ElectricityMeter::Iskraemeco>(input);
    return 0;
}