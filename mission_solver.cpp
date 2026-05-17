#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>
#include <memory>

// Global Standard Invariants
namespace CAPCPhysics {
    constexpr double C = 299792458.0; // Invariant speed of light (m/s)
    constexpr double C2 = C * C; // Speed of light squared
    constexpr double G_ACCEL = 9.80665; // Standard Earth gravity (m/s²)
    constexpr double AU_TO_METERS = 1.495978707e11; // 1 AU in meters
}

// Struct to store adjustable parameters inputted by the operator
struct MissionParameters {
    double distance_au; // One-way distance in Astronomical Units
    double acceleration_g; // Constant engine acceleration vector in Gs
    double mass_dry_tonnes; // Dry structural mass of the starship hull (t)
    double exhaust_velocity_c; // Core exhaust velocity as a fraction of c
    int crew_count; // Active crew complement
};

// Struct to hold calculated scalar outputs from the kinematic solvers
struct MissionReport {
    double distance_meters;
    double acceleration_ms2;
    double t_midpoint_seconds;
    double t_oneway_days;
    double t_roundtrip_days;
    double v_max_ms;
    double v_max_percent_c;
    double mass_ratio;
    double mass_fuel_tonnes;
    double mass_wet_tonnes;
};

// Analytical Core execution engine class
class BrachistochroneSolver {
private:
    std::unique_ptr<MissionParameters> params;
    std::unique_ptr<MissionReport> report;

    // Internal execution module running special relativistic rocket kinematics
    void executeKinematicSolver() {
        // Translate input scalars to base SI dimensions
        report->distance_meters = params->distance_au * CAPCPhysics::AU_TO_METERS;
        report->acceleration_ms2 = params->acceleration_g * CAPCPhysics::G_ACCEL;
        double exhaust_velocity_ms = params->exhaust_velocity_c * CAPCPhysics::C;

        // Midpoint calculation under straight-line constant acceleration
        double d_midpoint = report->distance_meters / 2.0;

        // Compute classical/low-velocity baseline time profile first
        report->t_midpoint_seconds = std::sqrt((2.0 * d_midpoint) / report->acceleration_ms2);
        report->v_max_ms = report->acceleration_ms2 * report->t_midpoint_seconds;

        // Relativistic compensation loop to check if velocity breaches the light cone
        if (report->v_max_ms > 0.05 * CAPCPhysics::C) {
            // Apply hyperbolic Minkowski space-time transformations
            report->t_midpoint_seconds = (CAPCPhysics::C / report->acceleration_ms2) * 
                                         std::acosh((report->acceleration_ms2 * d_midpoint / CAPCPhysics::C2) + 1.0);
            report->v_max_ms = CAPCPhysics::C * std::tanh(report->acceleration_ms2 * report->t_midpoint_seconds / CAPCPhysics::C);
        }

        // Aggregate temporal logs across all four legs (2 x one-way)
        report->t_oneway_days = (2.0 * report->t_midpoint_seconds) / 86400.0;
        report->t_roundtrip_days = report->t_oneway_days * 2.0;

        // Map velocity metrics to relational percentages of c
        report->v_max_percent_c = (report->v_max_ms / CAPCPhysics::C) * 100.0;

        // Relativistic rocket mass fraction derivation: R_m = exp( delta_V_total / V_exhaust )
        double total_delta_v = 4.0 * report->v_max_ms;
        report->mass_ratio = std::exp(total_delta_v / exhaust_velocity_ms);

        // Derive structural weight boundaries
        report->mass_wet_tonnes = params->mass_dry_tonnes * report->mass_ratio;
        report->mass_fuel_tonnes = report->mass_wet_tonnes - params->mass_dry_tonnes;
    }

public:
    BrachistochroneSolver() {
        params = std::make_unique<MissionParameters>();
        report = std::make_unique<MissionReport>();
    }

    // Interactive console command sequence allowing real-time variable tweaking
    void configureMissionInteractively() {
        std::cout << "\n====================================================================\n";
        std::cout << " CAPC PROPULSION MATRIX CONFIGURATION PROFILE USER TERMINAL \n";
        std::cout << "====================================================================\n";
        
        std::cout << " Enter active crew complement (e.g., 5): ";
        std::cin >> params->crew_count;

        std::cout << " Enter starship structural dry mass in tonnes (e.g., 800): ";
        std::cin >> params->mass_dry_tonnes;

        std::cout << " Enter one-way mission distance in Astronomical Units (Mars min ~0.36): ";
        std::cin >> params->distance_au;

        std::cout << " Enter constant target engine acceleration vector in Gs (e.g., 1.0): ";
        std::cin >> params->acceleration_g;

        std::cout << " Enter engine exhaust velocity as a fraction of c (Standard = 0.999): ";
        std::cin >> params->exhaust_velocity_c;

        // Clamp safety boundaries to prevent illegal mathematical configurations
        if (params->exhaust_velocity_c >= 1.0) params->exhaust_velocity_c = 0.999999;
        if (params->exhaust_velocity_c <= 0.0) params->exhaust_velocity_c = 0.001;

        executeKinematicSolver();
        displayMissionTelemetry();
    }

    void displayMissionTelemetry() const {
        std::cout << "\n--------------------------------------------------------------------\n";
        std::cout << " MISSION TELEMETRY REPORT SUMMARY MATRIX \n";
        std::cout << "--------------------------------------------------------------------\n";
        std::cout << " Active Crew Configuration Payload : " << params->crew_count << " Members\n";
        std::cout << " Target Destination Trajectory Range : " << params->distance_au << " AU (" 
                  << report->distance_meters << " meters)\n";
        std::cout << " Operating Engine Thrust Profile : " << params->acceleration_g << " G (" 
                  << report->acceleration_ms2 << " m/s²)\n";
        std::cout << " Calibrated Plume Exit Velocity (Ve) : " << params->exhaust_velocity_c * 100.0 << " % of c\n";
        std::cout << "--------------------------------------------------------------------\n";
        std::cout << " Peak Midpoint Flip Velocity (Vmax) : " << report->v_max_ms << " m/s\n";
        std::cout << " Light-Cone Velocity Threshold : " << report->v_max_percent_c << " % of c\n";
        std::cout << " One-Way Direct Transit Timeline : " << report->t_oneway_days << " Earth Days\n";
        std::cout << " TOTAL MISSION ROUND-TRIP TIME : " << report->t_roundtrip_days << " Earth Days\n";
        std::cout << "--------------------------------------------------------------------\n";
        std::cout << " Required Structural Rocket Mass Ratio: " << report->mass_ratio << "\n";
        std::cout << " Hull Structural Dry Mass baseline : " << params->mass_dry_tonnes << " Metric Tonnes\n";
        std::cout << " REQUIRED ELEMENT 126 FUEL PAYLOAD : " << report->mass_fuel_tonnes << " Metric Tonnes\n";
        std::cout << " TOTAL INITIAL ORBITAL LAUNCH WEIGHT : " << report->mass_wet_tonnes << " Metric Tonnes\n";
        std::cout << "====================================================================\n\n";
    }
};

int main() {
    std::cout << std::fixed << std::setprecision(6);
    auto optimization_engine = std::make_unique<BrachistochroneSolver>();
    
    char execution_loop = 'y';
    while (execution_loop == 'y' || execution_loop == 'Y') {
        optimization_engine->configureMissionInteractively();
        std::cout << " Do you wish to adjust mission variables for an alternative trajectory run? (y/n): ";
        std::cin >> execution_loop;
    }
    
    std::cout << "\n[TERMINAL] Optimization ledger saved. Mission parameters locked.\n";
    return 0;
}
