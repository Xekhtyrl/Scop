#pragma once

#include <iostream>
#include <cmath>


//stand for Vectors and Matrices Library
namespace vml {
	//Vector Template Structures with operators definitions and other operation functions
	template<typename T, size_t N >
	struct Vector {
		T data[N];

		//constructors
		constexpr Vector() : data{} {}
		constexpr Vector(std::initializer_list<T> vals) {
			size_t i = 0;

			if (vals.size() == 1) {
				T val = *vals.begin();
				for (; i < N; ++i)
					data[i] = val;
			} else {
				for (auto &x : vals) {
					if (i >= N) break;
					data[i++] = x;
				}
				// if less values than N, fill the rest with 0
				for (; i < N; ++i)
					data[i] = T{};
			}
		}
		constexpr Vector(T val) {
			for (size_t i = 0; i < N; i++)
				data[i] = val;
		}
		constexpr Vector(float* vals) {
			for (size_t i = 0; i < N; i++)
				data[i] = vals[i];
		}
		
		// allow to construct a vector with another vector and other arguments
		template<size_t N2, typename... Args>
		constexpr Vector(const Vector<T, N2>& vec, Args... values)
		requires (N == N2 + sizeof...(Args)) {
			size_t i = 0;
			for (; i < N2; i++)
				data[i] = vec[i];
			T extra[] = {static_cast<T>(values)...};
			for (size_t j = 0; j < sizeof...(Args); j++)
				data[i + j] = extra[j];
		}
		template<size_t N2, typename... Args>
		constexpr Vector(Args... values, const Vector<T, N2>& vec)
		requires (N == N2 + sizeof...(Args)) {
			size_t i = 0;
			T extra[] = {static_cast<T>(values)...};
			for (; i < sizeof...(Args); i++)
				data[i] = extra[i];
			for (size_t j = 0; j < N2; j++)
				data[i + j] = vec[j];
		}

		//operators
		constexpr T& operator[](int index) {return data[index];}
		constexpr const T& operator[](int index) const {return data[index];}

		Vector<T, N> operator+(const Vector<T, N>& vec) {
			Vector<T, N> res;
			for (size_t i = 0; i < N; i++)
				res[i] = data[i] + vec[i];
			return res;
		}
		Vector<T, N>& operator+=(const Vector<T, N>& vec) {
			for (size_t i = 0; i < N; i++)
				data[i] += vec[i];
			return *this;
		}
		Vector<T, N> operator-(const Vector<T, N>& vec) {
			Vector<T, N> res;
			for (size_t i = 0; i < N; i++)
				res[i] = data[i] - vec[i];
			return res;
		}
		Vector<T, N>& operator-=(const Vector<T, N>& vec) {
			for (size_t i = 0; i < N; i++)
				data[i] -= vec[i];
			return *this;
		}

		Vector<T, N> operator*(const T val) {
			Vector<T, N> res;
			for (size_t i = 0; i < N; i++)
				res[i] = data[i] * val;
			return res;
		}
		Vector<T, N>& operator*=(const T val) {
			for (T& x : data)
				x *= val;
			return *this;
		}
		bool operator==(const Vector<T, N> vec) const {
			for (size_t i = 0; i < N; i++){
				if (data[i] != vec[i])
					return false;
			}
			return true;
		}
				// Cross product (only for 3D)
		Vector<T, 3> operator*(const Vector<T, 3>& v) const requires (N == 3) {
			return {
				data[1] * v.data[2] - data[2] * v.data[1],
				data[2] * v.data[0] - data[0] * v.data[2],
				data[0] * v.data[1] - data[1] * v.data[0]
			};
		}

		// getter
		constexpr size_t size() {return N;}

		void print() const {
			std::cout << "[";
			for (size_t i = 0; i < N; i++){
				std::cout << data[i];
				if (i < N - 1)
					std::cout << ", ";
			}
			std::cout << "]" << std::endl;
		}

		// other operations
		T dot(Vector<T,N> vec) const {
			T res = T{};
			for (size_t i = 0; i < N; i++)
				res += data[i] * vec[i];
			return res;
		}

		T norm() const {return std::sqrt(dot(*this));}
		
		Vector<T, N> normalize() const {
			T len = norm();
			Vector res;
			for (size_t i = 0; i < N; i++)
				res[i] = data[i] / len;
			return res;
		}

		// Cross product (only for 3D)
		constexpr Vector<T, 3> cross(const Vector<T, 3>& v) const requires (N == 3) {
			return {
				data[1] * v.data[2] - data[2] * v.data[1],
				data[2] * v.data[0] - data[0] * v.data[2],
				data[0] * v.data[1] - data[1] * v.data[0]
			};
		}
	};

	//aliases for most used Vector in this project
	using vec2 = Vector<float, 2>;
	using vec3 = Vector<float, 3>;
	using vec4 = Vector<float, 4>;

	//Matrix Template Structur with operators, other operation function and graph matrices needed for 3D manipulation
	template<typename T , size_t R, size_t C>
	struct Matrix {
		T data[R * C];

		//constructors
		constexpr Matrix() : data{} {}
		constexpr Matrix(std::initializer_list<T> vals) {
			size_t i = 0;
			for (auto &x : vals)
				data[i++] = x;
		}
		// operators

		constexpr T& operator()(int r, int c) {return data[r * C + c];}
		constexpr const T& operator()(int r, int c) const {return data[r * C + c];}
		constexpr T* operator[](int r) {return &data[r * C];}
		constexpr const T* operator[](int r) const {return &data[r * C];}
		Matrix<T, R, C> operator+(Matrix<T, R, C> mat) {
			Matrix res;
			for (size_t i = 0; i < R * C; i++)
				res = data[i] + mat[i];
			return res;
		}
		Matrix<T, R, C>& operator+=(Matrix<T, R, C> mat) {
			for (size_t i = 0; i < R * C; i++)
				data[i] += mat[i];
			return *this;
		}
		Matrix<T, R, C> operator-(Matrix<T, R, C> mat) {
			Matrix res;
			for (size_t i = 0; i < R * C; i++)
				res = data[i] - mat[i];
			return res;
		}
		Matrix<T, R, C>& operator-=(Matrix<T, R, C> mat) {
			for (size_t i = 0; i < R * C; i++)
				data[i] -= mat[i];
			return *this;
		}
		template<size_t C2>
		Matrix<T, R, C2> operator*(Matrix<T, C, C2> mat) {
			Matrix<T, R, C2> res{};
			for (size_t r = 0; r < R; r++){
				for (size_t c2 = 0; c2 < C2; c2++){
					for (size_t c = 0; c < C; c++){
						res[r][c2] += (*this)[r][c] * mat[c][c2];
					}
				}
			}
			return res;
		}
		template<size_t C2>
		Matrix<T, R, C2> operator*=(Matrix<T, C, C2> mat) {
			static_assert(R == C, "For *= operation Matrix need to be square.");
			*this = (*this) * mat;
			return *this;
		}
		template<size_t C2>
		Vector<T, R> operator*(Vector<T, C2> vec) {
			static_assert(C == C2, "Matrix and vector sizes incompatible.");
			Vector<T, R> res;
			for (size_t r = 0; r < R; r++)
				for (size_t c = 0; c < C2; c++)
					res[r] += vec[c] * (*this)[r][c];
			return res;
		}
		template<size_t C2>
		Matrix<T, R, R>& scale(Vector<T, C2> v) {
			for (size_t i = 0; i < R && i < C2; i++)
				(*this)[i][i] *= v[i];
			return *this;
		}
		//getter
		void print() const {
			std::cout << "[";
			for (size_t i = 0; i < R; i++) {
				std::cout << "[";
				for (size_t j = 0; j < C; j++) {
					std::cout << data[i * C + j];
					if (j < C - 1)
					std::cout << ", ";
				}
				std::cout << "]";
				if (i < R - 1)
				std::cout<< "\n";
			}
			std::cout << "]" << std::endl;
		}
	};

	//Aliased Matrices for most used Matrices
	using mat3 = Matrix<float, 3, 3>;
	using mat4 = Matrix<float, 4, 4>;

	// radian to degree and reverse conversion function
	inline float radians(float angle) {
		return angle * (M_PI / 180);
	}
	inline float degree(float rad) {
		return rad * (180 / M_PI);
	}

	// Some Vector operations redefine as functions out of the Vector Struct
	inline vec3 cross(vec3 v1, vec3 v2) {
		return vec3{
			v1[1] * v2[2] - v1[2] * v2[1],
			v1[2] * v2[0] - v1[0] * v2[2],
			v1[0] * v2[1] - v1[1] * v2[0]
		};
	}
	template<typename T, size_t N>
	T dot(Vector<T, N> v1, Vector<T, N> v2) {
		T res = T{};
		for (size_t i = 0; i < N; i++)
			res += v1[i] * v2[i];
		return res;
	}
	template<typename T, size_t N>
	inline Vector<T, N> normalize(Vector<T, N> vec) {
		T len = vec.norm();
		Vector<T, N> res;
		for (size_t i = 0; i < N; i++)
			res[i] = vec[i] / len;
		return res;
	}

	template<typename T, size_t N>
	constexpr Matrix<T, N, N> identity() {
		Matrix<T, N, N> res{}; // value-initialize to zero
		for (size_t i = 0; i < N; i++)
			res[i][i] = T{1};
		return res;
	}

	inline mat4 translation(vec3 vals) {
		mat4 res = identity<float, 4>();
		for (int i = 0; i < 3; i++){
			res[i][3] = vals[i];
		}
		return res;
	}
	inline mat4 translation(mat4 mat, vec3 vals) {
		for (int i = 0; i < 3; i++){
			mat[i][3] += vals[i];
		}
		return mat;
	}

	inline mat4 rotation(float rad, vec3 axis) {
		vec3 aN = axis.normalize();
		float c = std::cos(rad);
		float s = std::sin(rad);
		float ic = 1.0f - c;

		float x = aN[0], y = aN[1], z = aN[2];

		return mat4({
			c + x*x*ic,	   x*y*ic - z*s,  x*z*ic + y*s,  0.f,
			y*x*ic + z*s,  c + y*y*ic,    y*z*ic - x*s,  0.f,
			z*x*ic - y*s,  z*y*ic + x*s,  c + z*z*ic,    0.f,
			0.f,           0.f,           0.f,           1.f
		});
	}
	inline mat4 lookAt(vec3 eye, vec3 center, vec3 upRaw) {
		vec3 f = normalize(center - eye);		// forward
		vec3 r = normalize(cross(f, upRaw));	// right
		vec3 u = cross(r, f);					// up

		return mat4({
			r[0],	r[1],	r[2],	-dot(r, eye),
			u[0],	u[1],	u[2],	-dot(u, eye),
			-f[0],	-f[1],	-f[2],	dot(f, eye),
			0,		0,		0,		1
		});
	}

	inline mat4 perspective(float rad, float aspect, float near, float far) {
		float tR = tan(rad / 2);
		return mat4({
			1 / (aspect * tR),	0,			0,								0								,
			0,					1 / tR,		0,								0								,
			0,					0,			-(far + near) / (far - near),	-2 * far * near / (far - near)	,
			0,					0,			-1,								0								
		});
	}

	inline mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
		return mat4({
			2 / (right - left),		0,					0,					-1 * (right + left) / (right - left),
			0,						2 / (top - bottom),	0,					-1 * (top + bottom) / (top - bottom),
			0,						0,					-2 / (far - near),	-1 * (far + near) / (far - near)	,
			0,						0,					0,					1							
			});
	}

	inline mat4 scale(vec3 vec) {
		return mat4({
			vec[0],	0,		0,		0,
			0,		vec[1],	0,		0,
			0,		0,		vec[2],	0,
			0,		0,		0,		1
		});
	}
}
