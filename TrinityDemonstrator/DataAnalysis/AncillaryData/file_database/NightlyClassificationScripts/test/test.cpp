
#include <map>
#include <bits/stdc++.h>

std::vector<float> fileCurrent = [2.7];
auto maxFileCurrent = std::max_element(fileCurrent.begin(), fileCurrent.end());

// std::cout << *maxFileCurrent << std::endl;
float roundFileCurrent = std::round(10 * *maxFileCurrent) / 10;
std::cout << roundFileCurrent << std::endl;