#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "electricity_meter.h"
#include "meters_data.h"

//чтение профилей напряжения с счётчиков
namespace MeterVoltageProfilesIO { 

    //прочитать данные из CSV таблицы
    template <typename MeterModel>
    void ReadFromCSV(std::ifstream & input, MeterPhaseDeterminate::VoltageData<ElectricityMeter::Meter<MeterModel>>& voltage_data);

    template <typename MeterModel>
    void ReadFromCSV(std::ifstream & input, MeterPhaseDeterminate::VoltageData<ElectricityMeter::Meter<MeterModel>>& voltage_data) {
        
    }

} //namespace MeterVoltageProfilesIO