#include <iostream>
#include <vector>
#include <string>
// #include "linAlg.hpp"
// #include "Matrix.hpp"
// #include "Includes/vml.hpp"
// #include "my_glm.h"

using namespace std;
// using namespace vml;

// void strTrim(string& str, string arr = " ") {
// 	size_t start = str.find_first_not_of(arr);
// 	if (start > str.length()) {
// 		str = "";
// 		return;
// 	}
// 	size_t end = str.find_last_not_of(arr);
// 	str = str.substr(start, end - start + 1);	
// }

// template <typename T>
// T add(T a, T b) {
// 	return a + b;
// }

int main() {

	for (int i = 1; i <= 10; i++){
		int a = i << 1;
		int b = i >> 1;
		std::cout << a << std::endl;
		std::cout << b << std::endl;
	}
	// vec3 v1{1.0, 2.};
	// v1.print();
	// vec3 v2({2,3,1});
	// vec3 res1 = cross(v1, v2);
	// // vec3 res2 = v1.cross(v2);
	// vec3 res2 = cross(v1, v2);

	// res1.print();
	// res2.print();
	// v1.print();
	// mat4 rot = rotation(radians(50.), vec3({1.,1.,0.}));
	// rot.print();
	// mat4 la = lookAt(vec3{1.,0,0}, vec3{0.,1,1}, vec3{0,0,1});
	// la.print();
}