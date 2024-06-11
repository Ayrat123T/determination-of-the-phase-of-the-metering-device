#pragma once
#include <string>
#include <iostream>

namespace ElectricityMeter {
    
    enum meter_type { //тип счётчика
        single_phase = 1,// однофазный
        three_phase = 3 // трёхфазный
    };

    //счётчик
    template <typename MeterModel>
    class Meter {
    public:
        
        Meter<MeterModel>() = default;

        // печать номера, типа и фазы счётчика
        void PrintMeter();

        const std::string* GetName() const;
        
        const int GetNumber() const;

        const int GetPhase() const;

        const meter_type GetType() const;

        ~Meter() = default;

        const bool operator==(const Meter& other) const;

        const bool operator<(const Meter& other) const;

        //std::ostream& operator<<(std::ostream& stream) const;

    protected:
        std::string name_;//название счётчика
        int number_; //номер счётчика
        int phase_ = 1; //фаза счётчика
        meter_type type_; //тип счётчика

    private:

        virtual int ParseNomber(const std::string& str_number) = 0;

        virtual meter_type ParseType(const std::string& str_number) = 0;

        virtual int ParsePhase(const std::string& str_phase) = 0;
    };

    class Iskraemeco final : public Meter<Iskraemeco> {
    public:

        Iskraemeco() = default;

        Iskraemeco(const std::string& str_number, const std::string& str_phase);

        int ParseNomber(const std::string& str_number) override;

        meter_type ParseType(const std::string& str_number) override;

        int ParsePhase(const std::string& str_phase) override;

    };

    class I_prom final : public Meter<I_prom> {
    public:

        I_prom() = default;

        I_prom(const int number);

    };

    template <typename MeterModel>
    class Hasher {
    public:
        size_t operator()(const ElectricityMeter::Meter<MeterModel>& meter) const {
            return  static_cast<size_t>(std::hash<int>{}(meter.GetNumber())) + 1000000000 * static_cast<size_t>(std::hash<int>{}(meter.GetPhase()));
        }
    private:
        std::hash<int> hasher_;
    };

    template <typename MeterModel>
    void Meter<MeterModel>::PrintMeter() {
        std::cout << "number: " << number_ << " type: " << type_ << " phase: " << phase_ << std::endl;
    }

    template <typename MeterModel>
    const std::string* Meter<MeterModel>::GetName() const {
        return &name_;
    }

    template <typename MeterModel>
    const int Meter<MeterModel>::GetNumber() const {
        return number_;
    }

    template <typename MeterModel>
    const int Meter<MeterModel>::GetPhase() const {
        return phase_;
    }

    template <typename MeterModel>
    const meter_type Meter<MeterModel>::GetType() const {
        return type_;
    }

    template <typename MeterModel>
    const bool Meter<MeterModel>::operator==(const Meter& other) const {
        return (GetNumber()) == (other.GetNumber())
            && (GetPhase()) == (other.GetPhase());
    }

    template <typename MeterModel>
    const bool Meter<MeterModel>::operator<(const Meter& other) const {
        return std::pair{ GetNumber(), GetPhase() } < std::pair{ other.GetNumber(), other.GetPhase() };
    }

    /*template <typename MeterModel>
    std::ostream& Meter<MeterModel>::operator<<(std::ostream& stream) const {
        stream << "number: " << GetNumber() << " type: " << GetType() << " phase: " << GetPhase();
        return stream;
    }*/

    template <typename MeterModel>
    std::ostream& operator<<(std::ostream& stream, const Meter<MeterModel>& meter) {
        stream << "number: " << meter.GetNumber() << " type: " << meter.GetType() << " phase: " << meter.GetPhase();
        return stream;
    }

} //namespace ElectricityMeter