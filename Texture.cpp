#include "Texture.hpp"

// Default constructor
Texture::Texture() : _ID(0), _width(0), _height(0), _nrChannels(0) {}

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
}

void Texture::deleteTex() {
	if (_ID)
		glDeleteTextures(1, &_ID);
}


/**
*	@brief Simple function to load Texture in already existing class with stb_image library with a filePath and a TextureConfig struct for the configuration (optional as a default value is set)
*
*	@param filePath a string/char * with the relative or absolute path for the Texture
*	@param config optional parameter as a default value is set. A custom structur TextureConfig with the default value of true for flipVert(load the texture flipped back to normal) and
*		params: GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR as default values, that will be used to set the behavior of the texture in the program with glTexParameteri.
	@exception throw an exception in case the Image could not be loaded properly.
*/
void Texture::loadTexture(std::string filePath, TextureConfig config) {
    stbi_set_flip_vertically_on_load(config.flipVert);

    int reqChannels = 4;
    unsigned char* data = stbi_load(filePath.c_str(), &_width, &_height, &_nrChannels, reqChannels);

    if (!data) {
        std::cerr << "Failed to load texture" << std::endl;
        throw std::runtime_error("Failed to load Texture: " + filePath);
    }

	_path = filePath;
    glGenTextures(1, &_ID);
    glBindTexture(GL_TEXTURE_2D, _ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, config.params[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, config.params[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.params[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.params[3]);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _width,
        _height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

// unsigned char* Texture::content() {return _data;}
int Texture::width() {return _width;}
int Texture::height() {return _height;}
int Texture::nrChannels() {return _nrChannels;}
int Texture::id() {return _ID;}
std::string Texture::path () {return _path;}
