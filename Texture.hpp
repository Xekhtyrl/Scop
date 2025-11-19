#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <GL/glext.h>
#include "Includes/stb_image.h"
#include <array>

// Class declaration
struct TextureConfig {
    unsigned int type = GL_RGB;
    bool flipVert = true;
    std::array<unsigned int, 4> params = {GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR};
};

class Texture {
	public:
		Texture();
		// Texture(std::string filePath, TextureConfig config = TextureConfig{});
		Texture(const Texture &other);
		Texture &operator=(const Texture &rhs);
		~Texture();
		void loadTexture(std::string filePath, TextureConfig config = TextureConfig{});
		void deleteTex();
		int width();
		int height();
		int nrChannels();
		int id();

	private:
		unsigned int	_ID;
		int				_width;		//not sure if still needed
		int				_height;	//not sure if still needed
		int				_nrChannels;//not sure if still needed
};

#endif // TEXTURE_HPP_
