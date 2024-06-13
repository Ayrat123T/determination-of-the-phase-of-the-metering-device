#include "input_reader.h"

std::vector<std::string_view> detailMeterVoltageProfilesIO::SplitIntoWordsBySemicolon(std::string_view str) {
    std::vector<std::string_view> result;
    const int64_t pos_end = str.npos;
    while (true) {
        int64_t semicolon = str.find(';');
        if (comma != 0) {
            result.push_back(comma == pos_end ? str.substr(0) : str.substr(0, comma));
        }
        if (comma == pos_end) {
            break;
        }
        else {
            str.remove_prefix(comma + 1);
        }
    }
    return result;
}
