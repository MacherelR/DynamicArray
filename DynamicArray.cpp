#include <iostream>
#include <vector>
#include <algorithm> // For std::lower_bound
#include <cmath> // For std::nan

class DynamicArray {
private:
    std::vector<std::vector<double>> data; // 2D array for storing rows of data
    size_t cols; // Number of columns, fixed

public:
    DynamicArray(size_t columns) : cols(columns) {}

    // Simplify error messages with a generic function
    void error(const std::string& message) const {
        std::cerr << "Error: " << message << std::endl;
    }

    void addRow(const std::vector<double>& newRow) {
        if (newRow.size() != cols) {
            error("newRow must have the correct number of columns.");
            return;
        }
        auto it = std::lower_bound(data.begin(), data.end(), newRow,
            [](const std::vector<double>& a, const std::vector<double>& b) {
                return a.front() < b.front();
            });
        data.insert(it, newRow);
    }

    void deleteRows(int startIdx, size_t numRows) {
        if (startIdx == -1 && numRows == 1) {
            data.pop_back();
            return;
        }
        if (startIdx < 0 || startIdx >= data.size()) {
            error("Index out of bounds.");
            return;
        }

        // Ensure valid range for deletion
        size_t endIdx = std::min(data.size(), static_cast<size_t>(startIdx) + numRows);
        if (startIdx == 0 && numRows >= data.size()) {
            data.clear();
        } else {
            data.erase(data.begin() + startIdx, data.begin() + endIdx);
        }
    }

    void update(double timestamp, size_t col, double value) {
        if (col >= cols) {
            error("Column index out of bounds.");
            return;
        }

        for (auto& row : data) {
            if (row[0] == timestamp) {
                row[col] = value;
                return;
            }
        }

        error("No row with the specified timestamp found.");
    }

    bool timestampExists(double timestamp) const {
        auto it = std::lower_bound(data.begin(), data.end(), std::vector<double>{timestamp},
            [](const std::vector<double>& a, const std::vector<double>& b) {
                return a.front() < b.front();
            });
        // Check if the iterator points to an element that matches the timestamp
        return (it != data.end() && (*it).front() == timestamp);
    }

    void print() const {
        for (const auto& row : data) {
            for (double val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }

    std::vector<double> getRow(double timestamp) const {
        auto it = std::find_if(data.begin(), data.end(),
            [timestamp](const std::vector<double>& row) { return row[0] == timestamp; });
        return it != data.end() ? *it : std::vector<double>();
    }

    // Other methods remain largely unchanged but could benefit from error handling improvements

    size_t getNumRows() const { return data.size(); }

    int minKey() const {
        return data.empty() ? -1 : data.front().front();
    }

    int maxKey() const {
        return data.empty() ? -1 : data.back().front();
    }

    void addOrUpdateRow(double timestamp, size_t column_index, double value) {
        if (column_index >= cols) {
            error("Column index out of bounds.");
            return;
        }

        // Find the position where this row should be inserted or updated
        auto rowIt = std::find_if(data.begin(), data.end(),
                                [timestamp](const std::vector<double>& row) { return row[0] == timestamp; });

        if (rowIt != data.end()) {
            // Timestamp exists, update the existing row
            (*rowIt)[column_index] = value;
        } else {
            // Timestamp does not exist, create a new row
            std::vector<double> newRow(cols, std::nan("1"));
            newRow[0] = timestamp;
            newRow[column_index] = value;
            data.insert(std::lower_bound(data.begin(), data.end(), newRow,
                                        [](const std::vector<double>& a, const std::vector<double>& b) {
                                            return a[0] < b[0];
                                        }),
                        newRow);
        }
    }

    std::vector<std::vector<double>> getSlice(int startIdx, int endIdx) {
        if (startIdx < 0 || endIdx < 0 || startIdx >= data.size() || endIdx >= data.size() || startIdx > endIdx) {
            error("Invalid start and end indices.");
            return {};
        }

        return std::vector<std::vector<double>>(data.begin() + startIdx, data.begin() + endIdx + 1);
    }

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

    std::vector<double> getTimestamps() const {
        std::vector<double> timestamps;
        for (const auto& row : data) {
            timestamps.push_back(row[0]);
        }
        return timestamps;
    }



};

