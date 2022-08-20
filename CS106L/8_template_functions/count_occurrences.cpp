// system and timing
#include <iostream>
#include <chrono>

// data structure imports
#include <deque>
#include <vector>
#include <iterator>
#include <utility> // pair

using std::cout;    using std::endl;

// count_occurrences
template <typename InputIt, typename DataType>
int count_occurrences(InputIt begin, InputIt end, DataType val)
{
    int cnt = 0;
    for(InputIt it=begin; it!=end; it++){
        if(*it == val) cnt++;
    }

    return cnt;
}
int main() {

    // vector of strings
    std::vector<std::string> lands = {"Xadia", "Drakewood", "Innean"};
    // how can we count the number of times "Xadia" appears in this?
    cout << count_occurrences(lands.begin(), lands.end(), "Xadia") << endl;
    // string itself (array of chars)
    std::string str = "Xadia";
    // how can we count the number of times 'a' appears in this?
    cout << count_occurrences(str.begin(), str.end(), 'a') << endl;

    return 0;
}

