NAME = testGL

INC = ./Includes
HOME_LIB  = $(HOME)/.local/lib
HOME_INC  = $(HOME)/.local/include
DIR_OBJ = Obj/

IMGUI_DIR = $(INC)/imgui

IMGUI_SRCS = \
    $(IMGUI_DIR)/imgui.cpp \
    $(IMGUI_DIR)/imgui_draw.cpp \
    $(IMGUI_DIR)/imgui_widgets.cpp \
    $(IMGUI_DIR)/imgui_tables.cpp \
    $(IMGUI_DIR)/imgui_impl_glfw.cpp \
    $(IMGUI_DIR)/imgui_impl_opengl3.cpp

SRCS = main.cpp Controls.cpp utils.cpp Shader.cpp Texture.cpp stb_image.cpp $(IMGUI_SRCS)
SRCC = glad.c

OBJ = $(addprefix $(DIR_OBJ), $(SRCS:.cpp=.o))
OBJ += $(addprefix $(DIR_OBJ), $(SRCC:.c=.o))

CXX       := c++
CC        := gcc

CXXFLAGS  = -std=c++20 -g3 -fsanitize=address #-Wall -Wextra -Werror
CFLAGS    = -Wall -Wextra -Werror -g3 -fsanitize=address

INCLUDES  := -I$(INC) \
			 -I$(INC)/imgui \
             -I$(INC)/glad/include \
             -I$(HOME_INC)

LIBS      := -L$(HOME_LIB) \
             -Wl,-rpath,$(HOME_LIB) \
             -lglfw3 -ldl -lGL -lpthread -lX11

all: $(NAME)

$(NAME): openGL $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $@

# Compile .cpp source files
$(DIR_OBJ)%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile .c source files
$(DIR_OBJ)%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

ifeq ($(wildcard Includes/glfw-3.4/build),)
openGL:
	$(info Creating build folder)
	cd $(INC)/glfw-3.4 && \
	mkdir -p build && \
	cd build && \
	cmake .. -DGLFW_BUILD_WAYLAND=OFF -DGLFW_BUILD_X11=ON -DCMAKE_INSTALL_PREFIX=$(HOME)/.local && \
	make -j && \
	make install
else
openGL:
	$(info build folder already exists)
endif

clean:
	rm -rf $(DIR_OBJ)

fclean: clean
	rm -f $(NAME)

rebuild:
	rm -rf $(INC)/glfw-3.4/build
	make openGL

re: fclean all