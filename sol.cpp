#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

typedef pair<long double, long double> Point;

// Decode a string in a given base to base-10
long double decodeBase(const string& value, int base) {
    long double result = 0.0L;
    for (char ch : value) {
        int digit;
        if (isdigit(ch)) digit = ch - '0';
        else if (isalpha(ch)) digit = tolower(ch) - 'a' + 10;
        else throw invalid_argument("Invalid character in encoded number");
        if (digit >= base) throw invalid_argument("Digit out of range for base");
        result = result * base + digit;
    }
    return result;
}

// Lagrange interpolation to compute f(0)
long double lagrangeConstant(const vector<Point>& points) {
    long double result = 0.0L;
    int k = points.size();

    for (int i = 0; i < k; ++i) {
        long double xi = points[i].first;
        long double yi = points[i].second;

        long double term = yi;
        for (int j = 0; j < k; ++j) {
            if (i == j) continue;
            long double xj = points[j].first;
            term *= (0.0L - xj) / (xi - xj);
        }

        result += term;
    }

    return result;
}

// Read and decode points from the given JSON file
vector<Point> parseJSON(const string& filename, int& k_out) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "❌ Could not open file: " << filename << endl;
        exit(1);
    }

    json j;
    file >> j;

    int n = j["keys"]["n"];
    int k = j["keys"]["k"];
    k_out = k;

    vector<Point> points;

    cout << "\n📄 Reading: " << filename << "\nDecoded points (x, y):\n";
    for (auto it = j.begin(); it != j.end(); ++it) {
        string key = it.key();
        if (key == "keys") continue;

        int x = stoi(key);
        int base = stoi(it.value()["base"].get<string>());
        string y_encoded = it.value()["value"];

        try {
            long double y = decodeBase(y_encoded, base);
            cout << "  (" << x << ", " << fixed << setprecision(0) << y << ")   [base " << base << " → " << y_encoded << "]\n";
            points.emplace_back(x, y);
        } catch (...) {
            cerr << "⚠️  Failed to decode key " << key << "\n";
        }
    }

    if (points.size() < k) {
        cerr << "❌ Not enough points to interpolate.\n";
        exit(1);
    }

    // Use first k points (by key order, not sorted by x)
    vector<Point> result;
    for (int i = 1; i <= k && result.size() < k; i++) {
        for (const auto& p : points) {
            if ((int)p.first == i) {
                result.push_back(p);
                break;
            }
        }
    }
    
    cout << "Using " << result.size() << " points for interpolation:\n";
    for (const auto& p : result) {
        cout << "  (" << (int)p.first << ", " << fixed << setprecision(0) << p.second << ")\n";
    }
    
    return result;
}

int main() {
    vector<string> files = {"testcase1.json", "testcase2.json"};

    for (const auto& file : files) {
        int k = 0;
        vector<Point> points = parseJSON(file, k);
        long double c = lagrangeConstant(points);

        cout << "🔐 Constant term (secret c) from " << file << " = " << fixed << setprecision(0) << llround(c) << "\n";
    }

    return 0;
}
