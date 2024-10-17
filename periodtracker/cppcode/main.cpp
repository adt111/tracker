#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <random>

// Define period tracker class
class PeriodTracker {
private:
    struct Cycle {
        std::string startDate;              // Start date of the cycle
        std::string endDate;                // End date of the cycle
        std::vector<std::string> symptoms;  // Symptoms associated with the cycle
    };

    std::vector<Cycle> cycles;            // Store cycles
    int averageCycleLength = 28;          // Default average cycle length (set for initialization)

public:
    void addCycle(const std::string& startDate, const std::string& endDate, const std::vector<std::string>& symptomList);
    void predictFuturePeriods();
    void displayLog();
    void checkIrregularCycles();
    void healthReminders(const std::vector<std::string>& symptoms);
    static int calculateDaysDifference(const std::string& date1, const std::string& date2);
    static void printTableHeader();
    static void calculateOvulationAndFertility(const std::string& nextPeriodStartDate);
    void showMenu();
};

// Add new cycle to the tracker
void PeriodTracker::addCycle(const std::string& startDate, const std::string& endDate, const std::vector<std::string>& symptomList) {
    cycles.push_back({startDate, endDate, symptomList});

    // Calculate cycle length from start and end dates
    int cycleLength = calculateDaysDifference(startDate, endDate);
    averageCycleLength = (averageCycleLength + cycleLength) / 2;  // Update average cycle length

    // Provide health reminders based on symptoms, only if there are symptoms
    if (!symptomList.empty()) {
        healthReminders(symptomList);
    }
}

// Predict next two periods based on the end date of the last cycle
void PeriodTracker::predictFuturePeriods() {
    if (cycles.empty()) {
        std::cout << "No period data available to predict future periods.\n";
        return;
    }

    std::string lastEndDate = cycles.back().endDate;  // Use end date for predictions

    std::tm lastPeriodTm = {};
    std::istringstream lastDateStream(lastEndDate);
    lastDateStream >> std::get_time(&lastPeriodTm, "%d-%m-%Y");

    std::cout << "\n----- Predicted Future Periods -----\n";

    // Use random number generation to add variability (within 28-30 days)
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(28, 30);

    // Predict periods for the next two cycles
    for (int i = 1; i <= 2; ++i) {
        // Increment the lastPeriodTm by a random cycle length
        int predictedCycleLength = distribution(generator);
        lastPeriodTm.tm_mday += predictedCycleLength;

        // Normalize the structure to adjust month/year if necessary
        std::mktime(&lastPeriodTm);

        // Format the predicted date into a string
        std::ostringstream predictedPeriodStream;
        predictedPeriodStream << std::put_time(&lastPeriodTm, "%d-%m-%Y");
        std::string predictedPeriodStart = predictedPeriodStream.str();

        // Output the predicted date
        std::cout << "Predicted Period " << i << ": " << predictedPeriodStart << "\n";

        // Calculate and display ovulation and fertile window
        calculateOvulationAndFertility(predictedPeriodStart);
    }
}

// Display log of all cycles
void PeriodTracker::displayLog() {
    printTableHeader();

    for (const auto& cycle : cycles) {
        std::cout << "| " << std::setw(10) << cycle.startDate 
                  << " | " << std::setw(10) << cycle.endDate 
                  << " | ";

        if (cycle.symptoms.empty()) {
            std::cout << std::setw(20) << "None" << " |\n";
        } else {
            // Display each symptom in a new line for better readability
            std::cout << "\n| " << std::setw(10) << "" 
                      << " | " << std::setw(10) << "" 
                      << " | ";

            for (const auto& symptom : cycle.symptoms) {
                std::cout << std::setw(20) << symptom << " | ";
            }
            std::cout << "\n";  // Move to the next line after listing symptoms
        }
    }
    std::cout << "-------------------------------------------------------------------------\n";
}

// Check for irregular cycles
void PeriodTracker::checkIrregularCycles() {
    if (cycles.size() < 2) {
        return; // Not enough data to determine irregularity
    }

    for (size_t i = 1; i < cycles.size(); ++i) {
        int currentCycleLength = calculateDaysDifference(cycles[i - 1].startDate, cycles[i].startDate);
        if (std::abs(currentCycleLength - averageCycleLength) > 5) { // Arbitrary threshold
            std::cout << "Warning: Cycle from " << cycles[i - 1].startDate 
                      << " to " << cycles[i].startDate 
                      << " is irregular.\n";
            std::cout << "Consider tracking your symptoms or consulting a healthcare professional.\n";
        }
    }
}

// Calculate difference between two dates
int PeriodTracker::calculateDaysDifference(const std::string& date1, const std::string& date2) {
    std::tm tm1 = {};
    std::tm tm2 = {};

    std::istringstream ss1(date1);
    std::istringstream ss2(date2);

    ss1 >> std::get_time(&tm1, "%d-%m-%Y");
    ss2 >> std::get_time(&tm2, "%d-%m-%Y");

    auto time1 = std::chrono::system_clock::from_time_t(std::mktime(&tm1));
    auto time2 = std::chrono::system_clock::from_time_t(std::mktime(&tm2));

    return std::chrono::duration_cast<std::chrono::seconds>(time2 - time1).count() / (60 * 60 * 24);
}

// Print table header for the display log
void PeriodTracker::printTableHeader() {
    std::cout << "-------------------------------------------------------------------------\n";
    std::cout << "| " << std::setw(10) << "Start Date" 
              << " | " << std::setw(10) << "End Date" 
              << " | " << std::setw(20) << "Symptoms" << "                            |\n";
    std::cout << "-------------------------------------------------------------------------\n";
}

// Calculate ovulation and fertile window
void PeriodTracker::calculateOvulationAndFertility(const std::string& nextPeriodStartDate) {
    std::tm nextPeriodTm = {};
    std::istringstream dateStream(nextPeriodStartDate);
    dateStream >> std::get_time(&nextPeriodTm, "%d-%m-%Y");

    // Calculate ovulation date (14 days before the next period starts)
    nextPeriodTm.tm_mday -= 14;
    std::mktime(&nextPeriodTm); // Normalize

    // Format ovulation date into a string
    std::ostringstream ovulationStream;
    ovulationStream << std::put_time(&nextPeriodTm, "%d-%m-%Y");
    std::string ovulationDate = ovulationStream.str();

    std::cout << "Ovulation Date: " << ovulationDate << "\n";

    // Calculate fertile window
    nextPeriodTm.tm_mday -= 2; // Start of fertile window (2 days before ovulation)
    std::mktime(&nextPeriodTm); // Normalize

    // Format fertile start date into a string
    std::ostringstream fertileStartStream;
    fertileStartStream << std::put_time(&nextPeriodTm, "%d-%m-%Y");
    std::string fertileStartDate = fertileStartStream.str();

    nextPeriodTm.tm_mday += 3; // End of fertile window (1 day after ovulation)
    std::mktime(&nextPeriodTm); // Normalize

    // Format fertile end date into a string
    std::ostringstream fertileEndStream;
    fertileEndStream << std::put_time(&nextPeriodTm, "%d-%m-%Y");
    std::string fertileEndDate = fertileEndStream.str();

    std::cout << "Fertile Window: " << fertileStartDate << " to " << fertileEndDate << "\n";
}

// Health reminders based on symptoms
void PeriodTracker::healthReminders(const std::vector<std::string>& symptoms) {
    std::cout << "----- Health Reminders -----\n";
    for (const auto& symptom : symptoms) {
        if (symptom == "cramps") {
            std::cout << "Tip: Try heat therapy or light exercise to relieve cramps.\n";
        } else if (symptom == "headache") {
            std::cout << "Tip: Stay hydrated and consider a small dose of over-the-counter pain relief.\n";
        } else if (symptom == "moodswings") {
            std::cout << "Tip: Engage in activities you enjoy or practice mindfulness to help stabilize your mood.\n";
        } else if (symptom == "nausea") {
            std::cout << "Tip: Ginger tea may help soothe nausea.\n";
        }
        // Add more symptoms and corresponding reminders as needed
    }
}

// Display the main menu for user interaction
void PeriodTracker::showMenu() {
    int choice;
    do {
        std::cout << "\n1. Add Period Cycle\n2. Predict Future Periods\n3. Display Cycle Log\n4. Check Irregular Cycles\n5. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            std::string startDate, endDate;
            int numSymptoms;
            std::vector<std::string> symptomList;

            std::cout << "Enter start date (dd-mm-yyyy): ";
            std::cin >> startDate;
            std::cout << "Enter end date (dd-mm-yyyy): ";
            std::cin >> endDate;

            std::cout << "Enter the number of symptoms experienced (0 if none): ";
            std::cin >> numSymptoms;

            if (numSymptoms > 0) {
                std::cout << "Enter symptoms (separated by spaces): ";
                symptomList.resize(numSymptoms);
                for (int i = 0; i < numSymptoms; ++i) {
                    std::cin >> symptomList[i];
                }
            }

            // Add the cycle to the tracker
            addCycle(startDate, endDate, symptomList);
        } else if (choice == 2) {
            predictFuturePeriods();
        } else if (choice == 3) {
            displayLog();
        } else if (choice == 4) {
            checkIrregularCycles();
        }
    } while (choice != 5);
}

// Main function
int main() {
    PeriodTracker tracker;
    tracker.showMenu();
    return 0;
}
