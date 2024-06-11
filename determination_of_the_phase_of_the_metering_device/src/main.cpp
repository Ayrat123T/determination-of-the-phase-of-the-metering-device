#include <libpq-fe.h> /* libpq header file */ 

#include "meters_data.h"
#include "input_reader.h"


using namespace std;

void PrintAttribDataFromPostgreSQL(int nFields, PGresult* res) {
    for (int i = 0; i < nFields; i++) { // сначала напечатать имена атрибутов
        printf(PQfname(res, i));
        printf("\t");
    }
    printf("\n");
    for (int i = 0; i < PQntuples(res); i++) { //затем напечатать строки 
        for (int j = 0; j < nFields; j++) {
            printf(PQgetvalue(res, i, j));
            printf("\t");
        }
        printf("\n");
    }
}

void PhasingDataFromPostgreSQL(const char* conninfo) {
    char query_string[256];	// holds constructed SQL query
    PGconn* conn = PQconnectdb(conninfo); // Установить подключение к базе данных
    if (PQstatus(conn) != CONNECTION_OK) { // Убедиться, что соединение с сервером установлено успешно
        fprintf(stderr, "Connection to database failed: %s",
            PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    //PGresult* res = PQexec(conn, "SELECT * from data WHERE timestamp < '2022-12-15'");
    PGresult* res = PQexec(conn, "SELECT * from test"); // holds query result

    /*int nFields = PQnfields(res);
    PrintAttribDataFromPostgreSQL(nFields, res);*/

    MeterPhaseDeterminate::VoltageData<ElectricityMeter::Iskraemeco> voltage_data;
    for (int i = 0; i < PQntuples(res); ++i) {
        ElectricityMeter::Iskraemeco meter(PQgetvalue(res, i, 0), PQgetvalue(res, i, 3));
        voltage_data.Insert(meter, PQgetvalue(res, i, 2));
    }
    cout << "Insert complited!" << endl;
    voltage_data.Interpolation();
    ofstream outFreeRatio; // поток для записи
    outFreeRatio.open("PhasesToMeterToFreeRatio.csv"); // открываем файл для записи
    voltage_data.PrintPhasesToMeterToFreeRatio(outFreeRatio);
    ofstream outSlope; // поток для записи
    outSlope.open("PhasesToMeterToSlope.csv"); // открываем файл для записи
    voltage_data.PrintPhasesToMeterToSlope(outSlope);

    PQfinish(conn);// закрыть подключение к базе данных и провести очистку
}

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

int main(int argc, char** argv)
{
    //Ниже приведён пример использования совместно с базой данных PostgreSQL
    const char* conninfo;
    /* Если пользователь передаёт параметр в командной строке, использовать его как
    строку соединения; иначе принять по умолчанию dbname=postgres и использовать
    переменные среды или значения по умолчанию для всех других параметров соединения.*/
    if (argc > 1) conninfo = argv[1];
    else conninfo = "dbname=unloading_from_meters user=postgres password=admin hostaddr=127.0.0.1 port=5432";
    PhasingDataFromPostgreSQL(conninfo);
    return 0;
}