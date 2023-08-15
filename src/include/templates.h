#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "fileutils.h"

// Definition of getSelectedIndices for vector of any type
template<class T>
vector<int> getSelectedIndices(const vector<T>& options, istream& input) {
    // Get selected indices from user input and return as vector
    cout << "Enter the indices of options (comma-separated): ";
    string inputStr;
    getline(input, inputStr);
    istringstream iss(inputStr);
    string indexStr;
    vector<int> selectedIndices;
    while (getline(iss, indexStr, ',')) {
        int index = stoi(indexStr);
        if (index >= 1 && index <= static_cast<int>(options.size())) {
            selectedIndices.push_back(index - 1);
        }
    }
    return selectedIndices;
}

#endif // TEMPLATES_H
