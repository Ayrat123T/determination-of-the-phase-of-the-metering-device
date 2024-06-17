#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
//#include <libpq-fe.h> /* libpq header file */ 

#include "electricity_meter.h"
#include "meters_data.h"

namespace detailMeterVoltageProfilesIO {

    std::vector<std::string> SplitIntoWordsBySemicolon(std::string& text);

} //namespace detailMeterVoltageProfilesIO

//чтение профилей напряжения с счётчиков
namespace MeterVoltageProfilesIO { 

    //Сфазировать счётчики используя данные из PostgreSQL
    /*template <typename MeterModel>
    void PhasingDataFromPostgreSQL(int argc, char** argv, const char* conninfo);

    //прочитать данные из PostgreSQL
    template <typename MeterModel>
    MeterPhaseDeterminate::VoltageData<MeterModel> ReadFromPostgreSQL(PGconn* conninfo);

    //напечатать данные из PostgreSQL
    void PrintAttribDataFromPostgreSQL(int nFields, PGresult* res);*/

    //Сфазировать счётчики используя данные из CSV таблицы
    template <typename MeterModel>
    void PhasingDataFromCSV(std::ifstream& input);

    //прочитать данные из CSV таблицы
    template <typename MeterModel>
    MeterPhaseDeterminate::VoltageData<MeterModel> ReadFromCSV(std::ifstream& input);

} //namespace MeterVoltageProfilesIO

/*template <typename MeterModel>
void MeterVoltageProfilesIO::PhasingDataFromPostgreSQL(int argc, char** argv, const char* conninfo) {
  
    PGconn* conn = PQconnectdb(conninfo); // Установить подключение к базе данных
    if (PQstatus(conn) != CONNECTION_OK) { // Убедиться, что соединение с сервером установлено успешно
        fprintf(stderr, "Connection to database failed: %s",
            PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    
    MeterPhaseDeterminate::VoltageData<MeterModel> voltage_data = ReadFromPostgreSQL<MeterModel>(conn);

    voltage_data.Interpolation();
    std::ofstream outFreeRatio; // поток для записи
    outFreeRatio.open("PhasesToMeterToFreeRatio.csv"); // открываем файл для записи
    voltage_data.PrintPhasesToMeterToFreeRatio(outFreeRatio);
    std::ofstream outSlope; // поток для записи
    outSlope.open("PhasesToMeterToSlope.csv"); // открываем файл для записи
    voltage_data.PrintPhasesToMeterToSlope(outSlope);

    PQfinish(conn);// закрыть подключение к базе данных и провести очистку
}
template <typename MeterModel>
MeterPhaseDeterminate::VoltageData<MeterModel> MeterVoltageProfilesIO::ReadFromPostgreSQL(PGconn* conn) {
    MeterPhaseDeterminate::VoltageData<MeterModel> voltage_data;
    //PGresult* res = PQexec(conn, "SELECT * from data WHERE timestamp < '2022-12-15'");
    PGresult* res = PQexec(conn, "SELECT * from test"); // holds query result
    /*int nFields = PQnfields(res);
    PrintAttribDataFromPostgreSQL(nFields, res);*/
    /*for (int i = 0; i < PQntuples(res); ++i) {
        MeterModel meter(PQgetvalue(res, i, 0), PQgetvalue(res, i, 3));
        voltage_data.Insert(meter, PQgetvalue(res, i, 2));
    }
    std::cout << "Insert complited!" << std::endl;
    return voltage_data;
}*/

/*void ExportPhasingResultToPostgreSQL(PGconn* conn) {
    const char* name = "name";
    int num = 1;
    PGresult* res = PQexec(conn, "CREATE TABLE IF NOT EXISTS MetersToVoltages "
        "(id SERIAL PRIMARY KEY,"
        "MeterNomber TEXT,"
        "Voltage double precision)");

    PQclear(res);
    PQfinish(conn);// закрыть подключение к базе данных и провести очистку
}*/

template <typename MeterModel>
void MeterVoltageProfilesIO::PhasingDataFromCSV(std::ifstream& input) {
    MeterPhaseDeterminate::VoltageData<MeterModel> voltage_data = ReadFromCSV<MeterModel>(input);
    voltage_data.Interpolation();
    std::ofstream outFreeRatio; // поток для записи
    outFreeRatio.open("PhasesToMeterToFreeRatio.csv"); // открываем файл для записи
    voltage_data.PrintPhasesToMeterToFreeRatio(outFreeRatio);
    std::ofstream outSlope; // поток для записи
    outSlope.open("PhasesToMeterToSlope.csv"); // открываем файл для записи
    voltage_data.PrintPhasesToMeterToSlope(outSlope);
}

template <typename MeterModel>
MeterPhaseDeterminate::VoltageData<MeterModel> MeterVoltageProfilesIO::ReadFromCSV(std::ifstream & input) {
    MeterPhaseDeterminate::VoltageData<MeterModel> voltage_data;
    std::string line;
    if (input.is_open()) {
        while (std::getline(input, line)) {
            std::vector<std::string> line_data = detailMeterVoltageProfilesIO::SplitIntoWordsBySemicolon(line);
            MeterModel meter(line_data[0], line_data[3]);
            voltage_data.Insert(meter, line_data[2]);
        }
    }
    std::cout << "Insert complited!" << std::endl;
    return voltage_data;
}