#include <vector>
#include <unistd.h>
#include "../include/matplotlibcpp.h"

namespace plt = matplotlibcpp;
using namespace std;


int main() {
    int n_data = 50;
    vector<int> x, y;

    for (int i = 0; i < n_data; i++) {
        x.push_back(i);
        y.push_back(rand() % 100);
    }

    auto fig = plt::plot(x, y);
    for (int i = 0; i < 50; i++) {
        usleep(1000000);
        void update_plot(n_data, y);

        plt::show();
    }

    return 0;
}
