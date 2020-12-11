#include <locale>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");
    int n, tmp, idx;
    vector<int> vect;
    cin >> n;
    for (int i = 0; i < n; i++) {
        cin >> tmp;
        if (vect.size() == 0) vect.push_back(tmp);
        else {
            for (idx = 0; idx < vect.size(); idx++) {
                if (vect[idx] < tmp) { vect.emplace(vect.begin() + idx, tmp); break; }
            }
            if (idx == vect.size()) vect.push_back(tmp);
        }

        if (vect.size() > 5) vect.erase(vect.begin());
        for (int x = 0; x < vect.size(); x++)
            cout << vect[x] << " ";
        cout << "\n";
    }
    return 0;
}

