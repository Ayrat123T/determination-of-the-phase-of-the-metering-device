#include "electricity_meter.h"

namespace ElectricityMeter {

    Iskraemeco::Iskraemeco(const std::string& str_number, const std::string& str_phase)
    {
        name_ = str_number;
        number_ = ParseNomber(str_number);
        phase_ = ParsePhase(str_phase);
        type_ = ParseType(str_number);
    }

    int Iskraemeco::ParseNomber(const std::string& str_number) {
        return std::stoi(str_number.substr(8));
    }

    meter_type Iskraemeco::ParseType(const std::string& str_number) {
        using namespace std::literals;
        if (str_number.find("ISK103"sv) != str_number.npos) {
            return meter_type::three_phase;
        }
        else if (str_number.find("ISK102"sv) != str_number.npos) {
            return meter_type::single_phase;
        }
    }

    int Iskraemeco::ParsePhase(const std::string& str_phase) {
        using namespace std::literals;
        auto end_str_phase = str_phase.npos;
        if (str_phase.find("L1"sv) != end_str_phase) {
            return 1;
        }
        else if (str_phase.find("L2"sv) != end_str_phase) {
            return 2;
        }
        else {
            return 3;
        }
    }

    I_prom::I_prom(const int number) {
        number_ = number;
    }

} // namespace ElectricityMeter