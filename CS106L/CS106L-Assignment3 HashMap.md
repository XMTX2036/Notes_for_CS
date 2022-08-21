# Assignment3	STL HashMap

## Milestone 1: Const-Correctness

### Review the Lec(Template Classes and Const-correctness)

`const`: keyword indicating a variable, function or parameter can’t be modified

```C++
// e.g.1
std::vector<int> vec{1, 2, 3};
const std::vector<int> c_vec{7, 8};
std::vector<int>& ref = vec;
const std::vector<int>& c_ref = vec;

auto copy = c_ref; // a non-const copy
const auto copy = c_ref; // a const copy
auto& a_ref = ref; // a non-const reference
const auto& c_aref = ref; // a const reference

// e.g.2
std::string stringify(const Student& s){
  return s.getName() + " is " + std::to_string(s.getAge) + " years old." ;
}
```

- The compiler doesn’t know getName and getAge don’t modify s! 
- We need to promise that it doesn’t by defining them as const functions 
- Add const to the end of function signatures!

`const-interface`: All member functions marked const in a class definition. Objects of type const ClassName may only use the const-interface.