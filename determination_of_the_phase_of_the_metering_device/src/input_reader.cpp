#include "input_reader.h"

std::vector<std::string> detailMeterVoltageProfilesIO::SplitIntoWordsBySemicolon(std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ';') {
            words.push_back(word);
            word.clear();
        }
        else {
            word += c;
        }
    }
    words.push_back(word);
    return words;
}

void MeterVoltageProfilesIO::PrintAttribDataFromPostgreSQL(int nFields, PGresult* res) {
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