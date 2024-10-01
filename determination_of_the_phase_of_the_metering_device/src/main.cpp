#include "meters_data.h"
#include "input_reader.h"

using namespace std;

void testPhasingDataFromPostgreSQL(int argc, char** argv) {
    const char* conninfo;
    /* Если пользователь передаёт параметр в командной строке, использовать его как
    строку соединения; иначе принять по умолчанию dbname=postgres и использовать
    переменные среды или значения по умолчанию для всех других параметров соединения.*/
    if (argc > 1) conninfo = argv[1];
    else conninfo = "dbname=unloading_from_meters user=postgres password=admin hostaddr=127.0.0.1 port=5432";
    MeterVoltageProfilesIO::PhasingDataFromPostgreSQL<ElectricityMeter::Iskraemeco>(argc, argv, conninfo);
}

void testPhasingDataFromCSV() {
    ifstream input("metersPQ14-12-2022.csv");
    MeterVoltageProfilesIO::PhasingDataFromCSV<ElectricityMeter::Iskraemeco>(input);
}

int main(int argc, char** argv) {
    testPhasingDataFromPostgreSQL(argc, argv);
    //testPhasingDataFromCSV();
    return 0;
}