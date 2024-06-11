#include "electricity_meter.h"

namespace ElectricityMeter {

    std::ostream& operator<<(std::ostream& stream, const METER_TYPE& meter_type) {
        return stream << (meter_type == METER_TYPE::SINGLE_PHASE ? "single-phase" : "three-phase");
    }

    Iskraemeco::Iskraemeco(const std::string& str_number, const std::string& str_phase) {
        name_ = str_number;
        number_ = ParseNomber(str_number);
        phase_ = ParsePhase(str_phase);
        type_ = ParseType(str_number);
    }

    int Iskraemeco::ParseNomber(const std::string& str_number) {
        return std::stoi(str_number.substr(8));
    }

    METER_TYPE Iskraemeco::ParseType(const std::string& str_number) {
        using namespace std::literals;
        if (str_number.find("ISK103"sv) != str_number.npos) {
            return METER_TYPE::THREE_PHASE;
        }
        else if (str_number.find("ISK102"sv) != str_number.npos) {
            return METER_TYPE::SINGLE_PHASE;
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