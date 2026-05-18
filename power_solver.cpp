#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <memory>

namespace CDPAPhysics {
    constexpr double C = 299792458.0;         
    constexpr double PROTON_MASS = 1.67262192e-27; 
    constexpr double PROTON_CHARGE = 1.60217663e-19; 
    constexpr double VACUUM_PERMITTIVITY = 8.8541878128e-12; 
    constexpr double LIQUID_HELIUM_TEMP = 4.2; 
}

struct PlantConfiguration {
    int scale_tier;               
    double operational_freq_hz;   
    double pellet_radius_mm;      
    double magnetic_flux_tesla;   
    double plant_efficiency;      
};

struct TelemetryReport {
    std::string tier_name;
    double raw_coulomb_force_newtons;
    double kinetic_energy_per_pulse_joules;
    double gross_thermal_output_watts;
    double net_electrical_output_mw;
    double monthly_homes_powered;
    double required_fuel_mass_grams_per_day;
    double cooling_overhead_kw;
};

class PowerScalingSolver {
private:
    std::unique_ptr<PlantConfiguration> config;
    std::unique_ptr<TelemetryReport> report;

    void executeGridSolver() {
        if (config->scale_tier == 1) {
            report->tier_name = "SMALL-SCALE MOBILE INFRASTRUCTURE MODULE";
        } else if (config->scale_tier == 2) {
            report->tier_name = "MEDIUM-SCALE METROPOLITAN UTILITY SUBSTATION";
        } else {
            report->tier_name = "LARGE-SCALE NATIONAL GRID SATURATION HUB";
        }

        double r_meters = (config->pellet_radius_mm / 1000.0);
        double volume_m3 = (4.0 / 3.0) * M_PI * std::pow(r_meters, 3);
        double atom_density = 5.0e28; 
        double total_atoms = atom_density * volume_m3;
        double total_charge = total_atoms * 126.0 * CDPAPhysics::PROTON_CHARGE;

        double k_coulomb = 1.0 / (4.0 * M_PI * CDPAPhysics::VACUUM_PERMITTIVITY);
        report->kinetic_energy_per_pulse_joules = 
            k_coulomb * (std::pow(total_charge, 2) / r_meters);
        
        double total_nucleons = total_atoms * 310.0;
        double mass_ballast = total_nucleons * CDPAPhysics::PROTON_MASS; 
        double max_relativistic_energy = mass_ballast * CDPAPhysics::C * CDPAPhysics::C;
        
        if (report->kinetic_energy_per_pulse_joules > max_relativistic_energy) {
            report->kinetic_energy_per_pulse_joules = max_relativistic_energy * 0.15; 
        }

        report->raw_coulomb_force_newtons = 
            report->kinetic_energy_per_pulse_joules / r_meters;

        report->gross_thermal_output_watts = 
            report->kinetic_energy_per_pulse_joules * config->operational_freq_hz;
        double net_watts = 
            report->gross_thermal_output_watts * config->plant_efficiency;
        report->net_electrical_output_mw = net_watts / 1.0e6;

        report->monthly_homes_powered = (net_watts / 1200.0);

        double grams_per_pulse = mass_ballast * 1000.0;
        report->required_fuel_mass_grams_per_day = 
            grams_per_pulse * config->operational_freq_hz * 86400.0;

        report->cooling_overhead_kw = 12.5 * std::pow(config->magnetic_flux_tesla, 2);
    }

public:
    PowerScalingSolver() {
        config = std::make_unique<PlantConfiguration>();
        report = std::make_unique<TelemetryReport>();
    }

    void configurePlantInteractively() {
        std::cout << "\n==================================================\n";
        std::cout << " CDPA DYNAMIC POWER MATRIX LAYOUT CONFIG \n";
        std::cout << "==================================================\n";
        std::cout << " Select Infrastructure Scale Tier:\n";
        std::cout << " 1. Small-Scale Mobile Module\n";
        std::cout << " 2. Medium-Scale Metropolitan Substation\n";
        std::cout << " 3. Large-Scale National Grid Saturation Hub\n";
        std::cout << " Enter selection (1-3): ";
        std::cin >> config->scale_tier;

        std::cout << " Enter cycle frequency in Hz (100 - 1000): ";
        std::cin >> config->operational_freq_hz;

        std::cout << " Enter pellet radius in mm (0.1 - 2.5): ";
        std::cin >> config->pellet_radius_mm;

        std::cout << " Enter REBCO coil flux in Tesla (10 - 25): ";
        std::cin >> config->magnetic_flux_tesla;

        std::cout << " Enter MHD harvesting efficiency (e.g. 0.92): ";
        std::cin >> config->plant_efficiency;

        executeGridSolver();
        displayGridTelemetry();
    }

    void displayGridTelemetry() const {
        std::cout << "\n--------------------------------------------------\n";
        std::cout << " INFRASTRUCTURE UTILITY REPORT \n";
        std::cout << "--------------------------------------------------\n";
        std::cout << " OPERATIONAL TIER : " 
                  << report->tier_name << "\n";
        std::cout << " Alternator Frequency : " 
                  << config->operational_freq_hz << " Hz\n";
        std::cout << " Confinement Field : " 
                  << config->magnetic_flux_tesla << " Tesla\n";
        std::cout << " Net MHD Efficiency : " 
                  << config->plant_efficiency * 100.0 << " %\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << " Coulomb Force Vector : " 
                  << report->raw_coulomb_force_newtons << " N\n";
        std::cout << " Energy Release Per Pulse : " 
                  << report->kinetic_energy_per_pulse_joules << " J\n";
        std::cout << " Gross Subatomic Thermal Energy : " 
                  << report->gross_thermal_output_watts << " W\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << " NET CLEAN ELECTRICAL POWER OUTPUT : " 
                  << report->net_electrical_output_mw << " MW\n";
        std::cout << " CRITICAL BASELOAD PROJECTION : " 
                  << report->monthly_homes_powered << " Homes\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << " Fuel Consumption Profile : " 
                  << report->required_fuel_mass_grams_per_day << " g/Day\n";
        std::cout << " Cryogenic Circuit Overhead : " 
                  << report->cooling_overhead_kw << " kW\n";
        std::cout << "==================================================\n\n";
    }
};

int main() {
    std::cout << std::fixed << std::setprecision(6);
    auto grid_engine = std::make_unique<PowerScalingSolver>();
    char loop_flag = 'y';
    while (loop_flag == 'y' || loop_flag == 'Y') {
        grid_engine->configurePlantInteractively();
        std::cout << " Adjust parameters for alternative run? (y/n): ";
        std::cin >> loop_flag;
    }
    return 0;
}