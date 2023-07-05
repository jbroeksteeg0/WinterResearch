#include "../src/NDTree.h"
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

using namespace std;

int main() {
  std::uniform_real_distribution<double> unif(-100.0, 100.0);
  std::default_random_engine re;

  double a_random_double = unif(re);

  while (true) {
    cout << endl << endl << endl << endl << endl;
    NDTree<int, 2> tree({make_pair(-100, 100), make_pair(-100, 100)});
    NDTree<int, 2> tree2({make_pair(-100, 100), make_pair(-100, 100)});
    vector<tuple<double, double, int>> points;

    for (int iter = 0; iter < 1e5; iter++) {
      std::cout << "iter " << iter << std::endl;
      int oper = rand() % 3;

      // Add point
      if (oper == 0) {
        double x = unif(re);
        double y = unif(re);
        int value = rand() % 10000;
        cout << "Add " << x << "," << y << ": " << value << endl;

        points.push_back({x, y, value});
        tree.add({(double)x, (double)y}, value);
        tree2.add({(double)x, (double)y}, value);
      } else if (oper == 1 && false) {    // delete point
        if (points.size()) {
          int ind = rand() % points.size();
          auto del = points[ind];
          std::cout << "Del " << get<0>(del) << ", " << get<1>(del) << endl;
          tree.delete_point({(double)get<0>(del), (double)get<1>(del)}, get<2>(del));
          points.erase(points.begin() + ind);
        }
      } else if (oper == 2) {
        double xBound = unif(re), yBound = unif(re);
        std::cout << "Query " << xBound << " " << yBound << endl;
        if (points.size()) {
          vector<int> points1;
          for (auto t : points) {
            if (get<0>(t) < xBound && get<1>(t) < yBound) {
              points1.push_back(get<2>(t));
            }
          }
          vector<int> points2;
          tree.query_prefix({(double)xBound, (double)yBound}, points2);

          sort(points1.begin(), points1.end());
          std::cout << "Without tree: ";
          for (int i : points1)
            std::cout << i << " ";
          std::cout << std::endl;
          std::cout << "With tree: ";
          sort(points2.begin(), points2.end());
          for (int i : points2)
            std::cout << i << " ";
          std::cout << std::endl;

          assert(points1 == points2);
        }
      }
    }
  }
}