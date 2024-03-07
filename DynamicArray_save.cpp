#include <iostream>
#include <vector>
#include <algorithm> // For std::find_if
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <sstream>

namespace py = pybind11;

class DynamicArray {
private:
    std::vector<std::vector<double>> data; // 2D array for storing rows of data
    size_t cols; // Number of columns, fixed

public:
    DynamicArray(size_t columns) : cols(columns) {}

    void addRow(const std::vector<double>& newRow) {
        if (newRow.size() != cols) {
            std::cerr << "Error: newRow must have the correct number of columns." << std::endl;
            return;
        }
        auto it = std::lower_bound(data.begin(), data.end(), newRow,
            [](const std::vector<double>& a, const std::vector<double>& b) {
                return a.front() < b.front();
            });
        data.insert(it, newRow); // Insert newRow at the correct position
    }

    void deleteRows(int startIdx, size_t numRows) {
        if (startIdx == -1) {
            if (!data.empty()) {
                data.pop_back();
            } else {
                std::cerr << "Error: No rows to delete." << std::endl;
                return;
            }
        } else if (startIdx < 0 || static_cast<size_t>(startIdx) >= data.size() || startIdx + numRows > data.size()) {
            std::cerr << "Error: Index out of bounds." << std::endl;
            return;
        } else if (startIdx == 0 && numRows == data.size()) {
            data.clear();
        } else if (startIdx == 0 && numRows == 1) {
            data.erase(data.begin());
        }  else {
            data.erase(data.begin() + startIdx, data.begin() + std::min(data.size(), static_cast<size_t>(startIdx) + numRows));
        }
    }

    void update(double timestamp, size_t col, double value) {
        auto it = std::find_if(data.begin(), data.end(),
            [timestamp](const std::vector<double>& row) {
                return row[0] == timestamp;
            });

        if (it == data.end()) {
            std::cerr << "Error: No row with the specified timestamp found." << std::endl;
            return;
        }

        if (col >= cols) {
            std::cerr << "Error: Column index out of bounds." << std::endl;
            return;
        }

        size_t rowIndex = std::distance(data.begin(), it);
        data[rowIndex][col] = value;
    }

    bool timestampExists(double timestamp) const {
        auto it = std::lower_bound(data.begin(), data.end(), std::vector<double>{timestamp},
            [](const std::vector<double>& a, const std::vector<double>& b) {
                return a.front() < b.front();
            });
        // Check if the iterator points to an element that matches the timestamp
        return (it != data.end() && (*it).front() == timestamp);
    }


    size_t getNumRows() const {
        return data.size();
    }

    void print() const {
        for (const auto& row : data) {
            for (double val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }

    int minKey() const {
        if (data.empty()) {
            std::cerr << "Error: No rows in the array." << std::endl;
            return -1;
        }
        return data.front().front();
    }

    int maxKey() const {
        if (data.empty()) {
            std::cerr << "Error: No rows in the array." << std::endl;
            return -1;
        }
        return data.back().front();
    }

    void addOrUpdateRow(double timestamp, int column_index, double value) {

        // Check if the timestamp exists
        auto rowIt = std::lower_bound(data.begin(), data.end(), std::vector<double>{timestamp},
            [](const std::vector<double>& a, const std::vector<double>& b) {
                return a.front() < b.front();
            });

        if (rowIt != data.end() && (*rowIt).front() == timestamp) {
            // Timestamp exists, update the existing row
            (*rowIt)[++column_index] = value; // +1 because the first column is the timestamp
        } else {
            // Timestamp does not exist, create a new row with NaNs
            std::vector<double> newRow(cols, std::nan("")); // Fill with NaN
            newRow[0] = timestamp; // Set timestamp
            newRow[++column_index] = value; // Set the value for the variable_name
            data.insert(rowIt, newRow); // Insert the new row at the correct position
        }
    }

    // extract row based on timestamp
    std::vector<double> getRow(double timestamp) {
        auto it = std::lower_bound(data.begin(), data.end(), std::vector<double>{timestamp},
            [](const std::vector<double>& a, const std::vector<double>& b) {
                return a.front() < b.front();
            });

        if (it == data.end() || (*it).front() != timestamp) {
            // std::cerr << "Error: No row with the specified timestamp found." << std::endl;
            return {};
        }

        return *it;
    }

    // extract row based on index
    std::vector<double> getRowByIndex(int index) {
        if (index < 0 || static_cast<size_t>(index) >= data.size()) {
            std::cerr << "Error: Index out of bounds." << std::endl;
            return {};
        }
        return data[index];
    }

    // Get a slice of the array based on the start and end indices
    py::array_t<double> getSlice(int startIdx, int endIdx) {
        if (startIdx < 0 || endIdx < 0 || startIdx >= data.size() || endIdx >= data.size() || startIdx > endIdx) {
            std::cerr << "Error: Invalid start and end indices." << std::endl;
            return {};
        }

        auto slice = std::vector<std::vector<double>>(data.begin() + startIdx, data.begin() + endIdx + 1);
        // auto capsule = py::capsule(slice.data(), [](void* p) { /* no deleter needed */ });
        return to_matrix(slice);
    }

    // Get row index based on timestamp
    int getRowIndex(double timestamp) {
        auto it = std::lower_bound(data.begin(), data.end(), std::vector<double>{timestamp},
            [](const std::vector<double>& a, const std::vector<double>& b) {
                return a.front() < b.front();
            });

        if (it == data.end() || (*it).front() != timestamp) {
            std::cerr << "Error: No row with the specified timestamp found." << std::endl;
            return -1;
        }

        return std::distance(data.begin(), it);
    }

    py::array_t<double> to_matrix(std::vector<std::vector<double>> vals) {
        size_t N = vals.size();
        size_t M = vals[0].size();

        py::array_t<double, py::array::c_style> arr({N, M});

        auto ra = arr.mutable_unchecked();

        for (size_t i = 0; i < N; i++)
        {
            for (size_t j = 0; j < M; j++)
            {
                ra(i, j) = vals[i][j];
            };
        };

        return arr;
    };

    // Get all timestamps (first column) in the array
    std::vector<double> getTimestamps() const {
        std::vector<double> timestamps;
        for (const auto& row : data) {
            timestamps.push_back(row.front());
        }
        return timestamps;
    }

    std::string getDebugInfo() const {
        std::ostringstream oss;
        if (!data.empty() && !data.front().empty()) {
            const double* beginAddress = data.front().data();

            const double* endAddress = data.back().data() + data.back().size();
            
            oss << "Begin Address: " << static_cast<const void*>(beginAddress)
                << ", End Address: " << static_cast<const void*>(endAddress);
        } else {
            oss << "Data is empty";
        }
        return oss.str();
    }      
};

