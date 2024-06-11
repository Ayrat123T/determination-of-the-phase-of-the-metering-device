#pragma once
#include <map>
#include <set>
#include <string>
#include <cmath>
#include <vector>
#include <execution>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <tuple>
#include <algorithm>
#include <iomanip>
#include <array>
#include <chrono>
#include <ctime>
#include <time.h>
#include <stdint.h>
#include <stdexcept>
#include <functional>
#include <memory>
#include <optional>
#include <fstream>

#include "electricity_meter.h"

namespace MeterPhaseDeterminate {

    struct StraightRatios { // коэффициенты уравнения прямой y = kx+b
        double k; // угловой коэффициент
        double b; // свободный член
    };

    //typedef MetersToStraightsRatios<>

    namespace detail {

        // возвращает среднее арифметическое
        template <typename ExecutionPolicy, typename IteratorRange>
        double GetArithmeticMean(ExecutionPolicy&& Policy, IteratorRange begin, IteratorRange end) {
            double res = std::reduce(Policy, begin, end);
            int sum = (end - begin);
            return res / sum;
        }

        // возвращает стандартное отклонение
        template <typename ExecutionPolicy, typename IteratorRange>
        double GetStandardDeviation(ExecutionPolicy&& Policy, IteratorRange begin, IteratorRange end) {
            double arithmetic_mean = GetArithmeticMean(Policy, begin, end);
            return std::sqrt(std::transform_reduce(Policy, begin, end, 0.0, std::plus{}, [&arithmetic_mean](const auto delt) { // среднеквадратичное отклонение
                return pow(arithmetic_mean - delt, 2);
                }) / (end - begin - 1));
        }

        // возвращает cреднее отклонение
        template <typename ExecutionPolicy, typename IteratorRange>
        double GetMeanDeviation(ExecutionPolicy&& Policy, IteratorRange begin, IteratorRange end) {
            double arithmetic_mean = GetArithmeticMean(Policy, begin, end);
            double mean_deviation = std::transform_reduce(Policy, begin, end, 0.0, std::plus{}, [&arithmetic_mean](const auto delt) { // среднеарифметическое отклонение
                return std::abs(arithmetic_mean - delt);
                }) / (end - begin);
                return mean_deviation;
        }
    } // namespace detail

    template <typename MeterModel>
    class VoltageData {
    public:         
        // добавить счётчик в базу
        void Insert(const MeterModel& meter, const std::string& str_voltage);

        // отсортировать и разбить счётчики по фазам и фидерам
        void Interpolation();

        // вывести сырые значения напряжений
        void PrintVoltage();

        // вывести размер вектора напряжений каждого счётчика
        void PrintSize();
           
        // вывести коэффицент К интерполяции
        void PrintMeterToSlope();
            
        // вывести сфазированные счётчики и коэффиценты k интерполяции
        void PrintPhasesToMeterToSlope(std::ofstream & out);

        // вывести сфазированные счётчики и коэффиценты b интерполяции
        void PrintPhasesToMeterToFreeRatio(std::ofstream & out);

        // вывести несфазированные счётчики
        void PrintUnphasedMetersToSlope();

        // выделение фидеров сфазированных счётчиков
        void AllocateFeeders();

    private:
        // Коэффиценты уравнений прямых счётчика
        struct MeterToStraightsRatios {
            MeterModel meter;
            std::vector<StraightRatios> straights_ratios;
        };

        using MetersArrayToStraightsRatios = std::vector<MeterToStraightsRatios>;
        using PhasesMetersArrayToStraightsRatios = std::vector<MetersArrayToStraightsRatios>;

        std::unordered_map <MeterModel, std::vector<double>, ElectricityMeter::Hasher<MeterModel>> meters_to_voltage_;
        MetersArrayToStraightsRatios meters_to_straights_ratios_;
        PhasesMetersArrayToStraightsRatios phases_to_meters_to_straights_ratios_;
        MetersArrayToStraightsRatios unphased_meters_to_straights_ratios_;
        std::set<MeterModel>meters_without_data_;
        std::vector<std::shared_ptr<PhasesMetersArrayToStraightsRatios>> feeders_to_phases_to_meters_to_straights_ratios_;

        // перенос счётчиков без данных в неотсортированнный список
        void TransferMetersWithoutData();

        // расчитать коэффициенты интерполяции
        void CalcLinearFunction();

        // парсинг напряжения из базы данных
        double ParseVoltage(const std::string& str_voltage);

        // провести фазировку всех счётчиков с данными
        void PhaseAllMetersWithVoltageData();

        //сфазировать несфазированные счётчики
        void PhaseUnphasedMeters();

    };

    template <typename MeterModel>
    void VoltageData<MeterModel>::Insert(const MeterModel& meter, const std::string& str_voltage) {
        double voltage = VoltageData::ParseVoltage(str_voltage);
        meters_to_voltage_[meter].push_back(voltage);
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::Interpolation() {
        TransferMetersWithoutData();
        CalcLinearFunction();  
        std::cout << "Calc \"k\"&\"b\" complited!" << std::endl;

        sort(std::execution::par, meters_to_straights_ratios_.begin(), meters_to_straights_ratios_.end(),
            [](const auto& meter_to_K_lhs, const auto& meter_to_K_rhs) {
                return meter_to_K_lhs.straights_ratios[0].k < meter_to_K_rhs.straights_ratios[0].k;
            });

        PhaseAllMetersWithVoltageData();
        PhaseUnphasedMeters();
        std::cout << "PhaseAllMetersWithVoltageData complited!" << std::endl;
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::PrintVoltage() {
        for (const auto& [meter, voltages] : meters_to_voltage_) {
            std::cout << meter.GetNumber() << " " << meter.GetPhase() << " ";
            bool is_first = true;
            for (const double voltage : voltages) {
                if (!is_first) {
                    std::cout << " ";
                }
                std::cout << /*string(meter.number.size(), ' ') <<*/ voltage;
                is_first = false;
            }
            std::cout << ";" << std::endl;
        }
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::PrintSize() {
        for (const auto& [meter, voltages] : meters_to_voltage_) {
            std::cout << meter << " voltages.size: " << voltages.size() << std::endl;
        }
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::PrintMeterToSlope() {
        for (const auto& [meter, straights_ratios] : meters_to_straights_ratios_) {
            std::cout << meter.GetNumber() << " phase: " << meter.GetPhase() << " type: " << meter.GetType() << " ";
            for (const auto straight_ratio : straights_ratios) {
                std::cout << straight_ratio.k << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::PrintPhasesToMeterToSlope(std::ofstream & out) {
        out << "phase_;meter_number;meter_phase;type;straights_ratios;\n";
        int i = 1;
        for (const auto& phase_to_meters_to_straights_ratios : phases_to_meters_to_straights_ratios_) {
            for (const auto& [meter, straights_ratios] : phase_to_meters_to_straights_ratios) {
                out << i << ';' << meter.GetNumber() << ';' << meter.GetPhase() << ';' <<  meter.GetType() << ';';
                for (const auto straight_ratio : straights_ratios) {
                    out << straight_ratio.k << ";";
                }
                out << std::endl;
            }
            i++;
        }
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::PrintPhasesToMeterToFreeRatio(std::ofstream & out) {
        out << "phase_;meter_number;meter_phase;type;straights_ratios;\n";
        int i = 1;
        for (const auto& phase_to_meters_to_straights_ratios : phases_to_meters_to_straights_ratios_) {
            for (const auto& [meter, straights_ratios] : phase_to_meters_to_straights_ratios) {
                out << i << ';' << meter.GetNumber() << ';' << meter.GetPhase() << ';' <<  meter.GetType() << ';';
                for (const auto straight_ratio : straights_ratios) {
                    out << straight_ratio.b << ";";
                }
                out << std::endl;
            }
            i++;
        }
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::PrintUnphasedMetersToSlope() {
        for (const auto& [meter, straights_ratios] : unphased_meters_to_straights_ratios_) {
            std::cout << meter.GetNumber() << " meter_phase: " << meter.GetPhase() << " type: " << meter.GetType() << " ";
            for (const auto straight_ratio : straights_ratios) {
                std::cout << straight_ratio.k << " ";
            }
            std::cout << std::endl;
        }
    }


    template <typename MeterModel>
    void VoltageData<MeterModel>::TransferMetersWithoutData() {
        for (auto it = meters_to_voltage_.begin(); it != meters_to_voltage_.end();) {
            if (it->second == std::vector<double>((it->second).size(), 0.0)) {
                meters_without_data_.insert(it->first);
                it = meters_to_voltage_.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    template <typename MeterModel>
    double VoltageData<MeterModel>::ParseVoltage(const std::string& str_voltage) {
        double voltage = std::stod(str_voltage);
        auto comma = str_voltage.find(',');
        if (comma != str_voltage.npos) {
            voltage += std::stod("0." + (str_voltage.substr(comma + 1)));
        }
        return voltage;
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::CalcLinearFunction() {
        for (const auto& [meter, voltages] : meters_to_voltage_) {
            std::vector<StraightRatios> straights_ratios_temp;
            for (int i = 1; i < voltages.size(); ++i) {
                int x1 = i - 1, x2 = i;
                double y1 = voltages[i - 1], y2 = voltages[i];
                double k = (y1 - y2) / (x1 - x2);
                double b = (y2 * x1 - x2 * y1) / (x1 - x2);
                straights_ratios_temp.push_back({ k, b });
            }
            meters_to_straights_ratios_.push_back({ meter , straights_ratios_temp });
        }
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::PhaseAllMetersWithVoltageData() {
        while (!meters_to_straights_ratios_.empty()) {
            std::vector<MeterToStraightsRatios> phase_to_meters_to_straights_ratios;
            if (phase_to_meters_to_straights_ratios.empty()) {
                phase_to_meters_to_straights_ratios.push_back(std::move(meters_to_straights_ratios_.front()));
                meters_to_straights_ratios_.erase(meters_to_straights_ratios_.begin());
            }
            for (int i = 0; i < meters_to_straights_ratios_.size();) {
                const std::vector<StraightRatios> second_meter_straights_ratios_data = meters_to_straights_ratios_[i].straights_ratios;
                auto lambda_paste_meter_to_phase = [&]() {
                    for (auto& phase_to_meter_to_Ks : phase_to_meters_to_straights_ratios) {
                        const std::vector<StraightRatios> first_meter_straights_ratios_data = phase_to_meter_to_Ks.straights_ratios;
                        std::vector<double> delta(first_meter_straights_ratios_data.size());
                        std::transform(std::execution::par, first_meter_straights_ratios_data.begin(),
                            first_meter_straights_ratios_data.end(),
                            second_meter_straights_ratios_data.begin(), delta.begin(),
                            [](auto first_straight_ratio, auto second_straight_ratio) {
                                return first_straight_ratio.k - second_straight_ratio.k;
                            }
                        );
                        double standard_deviation = detail::GetStandardDeviation(std::execution::par, delta.begin(), delta.end());
                        if (standard_deviation < 0.8) {
                            phase_to_meters_to_straights_ratios.push_back(std::move(meters_to_straights_ratios_[i]));
                            meters_to_straights_ratios_.erase(meters_to_straights_ratios_.begin() + i);
                            return true;
                        }
                    }
                    return false;
                };
                if (!lambda_paste_meter_to_phase()) {
                    i++;
                }
            }
            if (phase_to_meters_to_straights_ratios.size() > 1) {
                phases_to_meters_to_straights_ratios_.push_back(std::move(phase_to_meters_to_straights_ratios));
            }
            else {
                unphased_meters_to_straights_ratios_.push_back(phase_to_meters_to_straights_ratios.back());
            }
            phase_to_meters_to_straights_ratios.clear();
        }
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::PhaseUnphasedMeters() {
        for (int i = 0; i < unphased_meters_to_straights_ratios_.size(); i++) {
            auto lambda = [&]() {
                for (auto& phase_to_meters_to_straights_ratios : phases_to_meters_to_straights_ratios_) {
                    for (const auto& meter_to_straights_ratios : phase_to_meters_to_straights_ratios) {
                        const std::vector<StraightRatios> first_meter_straights_ratios_data = meter_to_straights_ratios.straights_ratios;
                        const std::vector<StraightRatios> second_meter_straights_ratios_data = unphased_meters_to_straights_ratios_[i].straights_ratios;
                        std::vector<double> delta(first_meter_straights_ratios_data.size());
                        std::transform(std::execution::par, first_meter_straights_ratios_data.begin(),
                            first_meter_straights_ratios_data.end(),
                            second_meter_straights_ratios_data.begin(), delta.begin(),
                            [](auto first_straight_ratio, auto second_straight_ratio) {
                                return first_straight_ratio.k - second_straight_ratio.k;
                            }
                        );
                        double arithmetic_mean = detail::GetArithmeticMean(std::execution::par, delta.cbegin(), delta.cend());
                        double mean_deviation = detail::GetMeanDeviation(std::execution::par, delta.cbegin(), delta.cend());
                        double standard_deviation = detail::GetStandardDeviation(std::execution::par, delta.cbegin(), delta.cend());
                        if (standard_deviation < 0.8 /* && std::abs(arithmetic_mean) < 1*/) {
                            phase_to_meters_to_straights_ratios.push_back(std::move(unphased_meters_to_straights_ratios_[i]));
                            unphased_meters_to_straights_ratios_.erase(unphased_meters_to_straights_ratios_.begin() + i);
                            return;
                        }
                    }
                }
                return;
            };
            lambda();          
        }
    }

    template <typename MeterModel>
    void VoltageData<MeterModel>::AllocateFeeders() {
        /*std::set<int> phases;
        for (int i = 0; i < phases_to_meters_to_straights_ratios_.size(); i++) {
            auto it = std::find_if(phases_to_meters_to_straights_ratios_[i].begin(), phases_to_meters_to_straights_ratios_[i].end(),
                [](const auto& meter_to_straights_ratios) {
                    return meter_to_straights_ratios.meter.GetType() == ElectricityMeter::meter_type::three_phase;
                });
            if (it != std::end(phases_to_meters_to_straights_ratios_[i]) && !phases.count(i)) {
                //std::cout << (*it).meter.GetNumber() << std::endl;
                auto it2 = std::find_if(phases_to_meters_to_straights_ratios_.begin(), phases_to_meters_to_straights_ratios_.end(),
                    [it](const auto& phase_to_meters_to_straights_ratios) {
                        return std::count(phase_to_meters_to_straights_ratios.begin(), phase_to_meters_to_straights_ratios.end(),
                        *it);
                    });
                if (it2 != std::end(phases_to_meters_to_straights_ratios_) && !phases.count(*it2)) {
                    phases.insert(i);

                }
            }
            /*for (int j = 0; j < phases_to_meters_to_straights_ratios_[i].size(); j++) {
                if (phases_to_meters_to_straights_ratios_[i][j].meter.GetType() = ElectricityMeter::meter_type::three_phase) {
                    int phase_A;
                    int phase_B;
                    int phase_C;

                }
            }*/
        //}
    }

} //namespace MeterPhaseDeterminate