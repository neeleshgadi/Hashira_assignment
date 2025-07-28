#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

typedef pair<double, double> Point;

// Converts a number string in a given base to base 10
unsigned long long decodeBase(const string& value, int base) {
    unsigned long long result = 0;
    for (char digit : value) {
        int num;
        if (isdigit(digit)) num = digit - '0';
        else if (isalpha(digit)) num = tolower(digit) - 'a' + 10;
        else throw invalid_argument("Invalid character in base value");

        if (num >= base) throw invalid_argument("Digit out of base range");

        result = result * base + num;
    }
    return result;
}

// Perform Lagrange interpolation to compute constant term (f(0))
double lagrangeInterpolation(const vector<Point>& points) {
    double result = 0.0;
    int k = points.size();

    for (int i = 0; i < k; i++) {
        double xi = points[i].first;
        double yi = points[i].second;

        double term = yi;
        for (int j = 0; j < k; j++) {
            if (i != j) {
                double xj = points[j].first;
                term *= (0 - xj) / (xi - xj);
            }
        }
        result += term;
    }
    return result;
}

vector<Point> parseJSONTestCase(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "❌ Failed to open file: " << filename << endl;
        exit(1);
    }

    json j;
    file >> j;
    int n = j["keys"]["n"];
    int k = j["keys"]["k"];

    vector<Point> points;

    for (auto& [key, value] : j.items()) {
        if (key == "keys") continue;

        int x = stoi(key);
        int base = stoi(value["base"].get<string>());
        string encoded = value["value"];

        try {
            unsigned long long y = decodeBase(encoded, base);
            points.emplace_back((double)x, (double)y);
        } catch (...) {
            cerr << "Error decoding value at key " << key << endl;
        }
    }

    sort(points.begin(), points.end()); // Ensure consistent selection
    points.resize(k); // Use only the first k valid points
    return points;
}

int main() {
    vector<string> files = {"testcase1.json", "testcase2.json"};
    for (int i = 0; i < files.size(); i++) {
        vector<Point> points = parseJSONTestCase(files[i]);
        double c = lagrangeInterpolation(points);
        cout << "🔐 Secret from " << files[i] << " (constant term c) = ";
        cout << fixed << setprecision(0) << c << endl;
    }
    return 0;
}
