#include <iostream>
#include "entry.hpp"

using params = MTreeParams<double, std::string, 5>;
using space = metric_space<params>;
auto manhattan = space::manhattan;
auto euclidean = space::euclidean;
space manhattan_space(manhattan);
space euclidean_space(euclidean);

int main() {
    std::vector<double> features1 = {1.0, 2.0};
    std::vector<double> features2 = {0.0, 0.0};
    std::string xd = "xd";

    std::unique_ptr<entry<params>> e1 = std::make_unique<entry<params>>(features1, xd);
    std::unique_ptr<entry<params>> e2 = std::make_unique<entry<params>>(features2, xd);
    std::cout << e1->distance_to(*e2, manhattan_space) << std:: endl;
    return 0;
}
