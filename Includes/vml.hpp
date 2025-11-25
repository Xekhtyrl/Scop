#pragma once

#include <iostream>
#include <cmath>


//stand for Vectors and Matrices Library
namespace vml {
	/**
	 * @brief Vector Template Structures with operators definitions and other operation functions
	 * 
	 * @param T data type (float, int,...)
	 * @param N size_t that define the size of the vector
	 * 
	*/
	template<typename T, size_t N >
	struct Vector {
		T data[N];

		//constructors
		/**
		 * @brief default constructor that create an empty vector of N size and T type
		 */
		constexpr Vector() : data{} {}
		/**
		 * @brief constructor that create a vector of N size and T type filled with values
		 * 
		 * @param vals a list of values to put in the Vector. It does not have to be of N size. If vals is not of N size: if 1 value it will fill the vector with it, if less than N will fill the rest with 0 and if greater than N will stop when data is filled.
		 */
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
		/**
		 * @brief constructor that create a vector of N size and T type filled with one values
		 * 
		 * @param val a value of T type that will the value to fill the vector with
		 */
		constexpr Vector(T val) {
			for (size_t i = 0; i < N; i++)
				data[i] = val;
		}
		/**
		 * @brief constructor that create a vector of N size and float type filled with an array of float values 
		 * 
		 * @param val an array of float to fill the Vector with. HAS TO BE OF N SIZE AT LEAST
		 */
		constexpr Vector(float* vals) {
			for (size_t i = 0; i < N; i++)
				data[i] = vals[i];
		}
		
		/**
		 * @brief allow to construct a vector with another vector and other arguments
		 * 
		 * @param vec an other Vector<T, ..> of different size
		 * @param values variadic arguments to fill the rest of the vector with. CAREFULL vec.size + values.size must be of N size!
		*/
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
		/**
		 * @brief allow to construct a vector with arguments and a vector of another size (such as arg + vec = N)
		 * 
		 * @param values variadic arguments to fill the rest of the vector with. CAREFULL vec.size + values.size must be of N size!
		 * @param vec an other Vector<T, ..> of different size
		*/
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

		//calculate the norm of the vector and return it
		T norm() const {return std::sqrt(dot(*this));}
		
		//method that return a new vector of the normalized version of the vector
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

	/**
	 * @brief Matrix Template Structure of row column order with operators, other operation function and graph matrices needed for 3D manipulation
	 * 
	 * @param T data type of choice (int, float,...)
	 * @param R row size
	 * @param C column size
	*/
	template<typename T , size_t R, size_t C>
	struct Matrix {
		T data[R * C];

		//constructors
		/**
		 * @brief default constructor that create a Matrix of R * C size and T type filled with no values
		 */
		constexpr Matrix() : data{} {}
		/**
		 * @brief constructor that create a Matrix of R * C size and T type filled with values
		 * 
		 * @param vals a list of values to put in the Matrix. If not of R * C size will stop when it reach one of the limit (R*C or vals size)
		 */
		constexpr Matrix(std::initializer_list<T> vals) {
			size_t i = 0;
			for (auto &x : vals){
				if (i >= R * C)
					break;
				data[i++] = x;
			}
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
		//scale the matrix by multiplying its base values by scaling value
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

	// degree to radian conversion
	inline float radians(float angle) {
		return angle * (M_PI / 180);
	}
	// radian to degree conversion
	inline float degree(float rad) {
		return rad * (180 / M_PI);
	}

	// Some Vector operations redefine as functions out of the Vector Struct
	/** @brief cross product of two vec3
	 * 
	 * @param v1 a vec3 (Vector<float, 3>)
	 * @param v2 a vec3 (Vector<float, 3>)
	 * 
	 * @return a vec3 the cross product
	*/
	inline vec3 cross(vec3 v1, vec3 v2) {
		return vec3{
			v1[1] * v2[2] - v1[2] * v2[1],
			v1[2] * v2[0] - v1[0] * v2[2],
			v1[0] * v2[1] - v1[1] * v2[0]
		};
	}
	/** @brief dot product of two Vector<float, N>
	 * 
	 * @param v1 a Vector<float, N>
	 * @param v2 a Vector<float, N>
	 * 
	 * @return the dot product of T type
	*/
	template<typename T, size_t N>
	T dot(Vector<T, N> v1, Vector<T, N> v2) {
		T res = T{};
		for (size_t i = 0; i < N; i++)
			res += v1[i] * v2[i];
		return res;
	}
	/**
	 * @brief function that return a normalized version of Vector
	 * 
	 * @param vec the vector needed for the normalize version
	 * 
	 * @return a new normalized vector from the original
	 */
	template<typename T, size_t N>
	inline Vector<T, N> normalize(Vector<T, N> vec) {
		T len = vec.norm();
		Vector<T, N> res;
		for (size_t i = 0; i < N; i++)
			res[i] = vec[i] / len;
		return res;
	}

	// Return an square identity Matrix of N size and T type
	template<typename T, size_t N>
	constexpr Matrix<T, N, N> identity() {
		Matrix<T, N, N> res{}; // value-initialize to zero
		for (size_t i = 0; i < N; i++)
			res[i][i] = T{1};
		return res;
	}
	// return a translation mat4 with vec3 values added to the 4th column of the mat4
	inline mat4 translation(vec3 vals) {
		mat4 res = identity<float, 4>();
		for (int i = 0; i < 3; i++){
			res[i][3] = vals[i];
		}
		return res;
	}
	// translate a mat4 by vec3 values by adding them to the 4th column of the mat4
	inline mat4 translation(mat4 mat, vec3 vals) {
		for (int i = 0; i < 3; i++){
			mat[i][3] += vals[i];
		}
		return mat;
	}

	/**
	 * @brief rotation matrix needed for applying the rotation on another Matrix (mat4)
	 * 
	 * @param rad	a float value that is the radian of the angle of rotation desired
	 * @param axis	a vec3 precising on which axis you want the rotation to be (e.g. {1,0,0} would be on the x axis)
	 * 
	 * @return a mat4 rotation matrix to apply on another mat4
	 */
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
	/**
	 * @brief Builds a LookAt view matrix that transforms world space into camera space.
	 * @param eye     The camera position.
	 * @param center  The point the camera looks at.
	 * @param upRaw   The world up direction.
	 * @return The view matrix.
	 */	
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
	/**
	 * @brief Builds a perspective projection matrix using a vertical field-of-view.
	 * @param rad   Field of view in radians.
	 * @param aspect Aspect ratio (width / height).
	 * @param near  Near clipping plane.
	 * @param far   Far clipping plane.
	 * @return The perspective projection matrix.
	 */
	inline mat4 perspective(float rad, float aspect, float near, float far) {
		float tR = tan(rad / 2);
		return mat4({
			1 / (aspect * tR),	0,			0,								0								,
			0,					1 / tR,		0,								0								,
			0,					0,			-(far + near) / (far - near),	-2 * far * near / (far - near)	,
			0,					0,			-1,								0								
		});
	}

	/**
	 * @brief Builds an orthographic projection matrix with given clipping bounds.
	 * @param left   Left plane.
	 * @param right  Right plane.
	 * @param bottom Bottom plane.
	 * @param top    Top plane.
	 * @param near   Near clipping plane.
	 * @param far    Far clipping plane.
	 * @return The orthographic projection matrix.
	 */
	inline mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
		return mat4({
			2 / (right - left),		0,					0,					-1 * (right + left) / (right - left),
			0,						2 / (top - bottom),	0,					-1 * (top + bottom) / (top - bottom),
			0,						0,					-2 / (far - near),	-1 * (far + near) / (far - near)	,
			0,						0,					0,					1							
			});
	}

	/**
	 * @brief Creates a uniform or non-uniform scaling matrix from a 3D scale vector.
	 * @param vec The scale factors along the x, y, and z axes.
	 * @return The scaling matrix.
	 */
	inline mat4 scale(vec3 vec) {
		return mat4({
			vec[0],	0,		0,		0,
			0,		vec[1],	0,		0,
			0,		0,		vec[2],	0,
			0,		0,		0,		1
		});
	}
}
