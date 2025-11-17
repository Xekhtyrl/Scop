#include "Texture.hpp"

// Default constructor
Texture::Texture() : _ID(0), _width(0), _height(0), _nrChannels(0) {}

Texture::Texture(std::string filePath, TextureConfig config) {
	stbi_set_flip_vertically_on_load(config.flipVert);
	unsigned char *data = stbi_load(filePath.c_str(), &_width, &_height, &_nrChannels, 0);
	if (!data) {
		std::cout << "Failed to load texture" << std::endl;
		throw std::runtime_error("Failed to load Texture: " + filePath);
	}
	// 1 is the number of texture to be generated, will it always be 1?
	glGenTextures(1, &_ID);

	// will it always be 2D textures? If not, this part is only specific for 2D, change MACRO from 2d to 1d or 3d if needed?
	glBindTexture(GL_TEXTURE_2D, _ID);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, config.params[0]);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, config.params[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.params[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.params[3]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, _nrChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	stbi_image_free(data);
}

// Copy constructor
Texture::Texture(const Texture &other) {
  *this = other;
  return;
}

// Copy assignment overload
Texture &Texture::operator=(const Texture &rhs) {
	_width = rhs._width;
	_height = rhs._height;
	_ID = rhs._ID;
	_nrChannels = rhs._nrChannels;
	return *this;
}

// Default destructor
Texture::~Texture() {
	if (_ID)
		glDeleteTextures(1, &_ID);
}

// unsigned char* Texture::content() {return _data;}
int Texture::width() {return _width;}
int Texture::height() {return _height;}
int Texture::nrChannels() {return _nrChannels;}
int Texture::id() {return _ID;}
