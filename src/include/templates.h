#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "fileutils.h"

// Definition of getSelectedIndices for vector of any type
template<class T>
std::vector<int> getSelectedIndices(const std::vector<T>& options, std::istream& input) {
    // Get selected indices from user input and return as vector
    std::cout << "Enter the indices of options (comma-separated): ";
    std::string inputStr;
    getline(input, inputStr);
    std::istringstream iss(inputStr);
    std::string indexStr;
    std::vector<int> selectedIndices;
    while (getline(iss, indexStr, ',')) {
        int index = stoi(indexStr);
        if (index >= 1 && index <= static_cast<int>(options.size())) {
            selectedIndices.push_back(index - 1);
        }
    }
    return selectedIndices;
}

#endif // TEMPLATES_H
