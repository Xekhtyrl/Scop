NAME = Scop

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

SRCS =	main.cpp \
		Controls.cpp \
		utils.cpp \
		Shader.cpp \
		Texture.cpp \
		stb_image.cpp \
		window.cpp \
		modelMatrices.cpp \
		Model.cpp \
		ModelLoadObj.cpp \
		Mesh.cpp \
		$(IMGUI_SRCS)
SRCC = glad.c

OBJ = $(addprefix $(DIR_OBJ), $(SRCS:.cpp=.o))
OBJ += $(addprefix $(DIR_OBJ), $(SRCC:.c=.o))

CXX       := c++
CC        := gcc

CXXFLAGS  = -std=c++20 -Wall -Wextra -Werror -g3
CFLAGS    = -Wall -Wextra -Werror -g

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
	rm -f imgui.ini
	rm -f err.log

cclean: fclean closeGL

closeGL:
	rm -rf $(INC)/glfw-3.4/build

rebuild: closeGL
	make openGL

re: fclean all

exec: $(NAME)
	# make the program executable
	chmod +x $(CURDIR)/Scop

	# create applications dir
	mkdir -p $(HOME)/.local/share/applications

	# write the .desktop file
	printf '%s\n' \
	  '[Desktop Entry]' \
	  'Type=Application' \
	  'Name=Scop' \
	  'Comment=Open .obj files with Scop' \
	  'Exec=$(CURDIR)/Scop %F' \
	  'Path=$(CURDIR)' \
	  'Terminal=false' \
	  'MimeType=application/x-myobj;' \
	  'NoDisplay=false' \
	  > $(HOME)/.local/share/applications/scop.desktop

	# mark the desktop file executable/trusted
	chmod +x $(HOME)/.local/share/applications/scop.desktop

	# create mime packages dir
	mkdir -p $(HOME)/.local/share/mime/packages

	# write the MIME xml
	printf '%s\n' \
	  '<?xml version="1.0"?>' \
	  '<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">' \
	  '  <mime-type type="application/x-myobj">' \
	  '    <glob pattern="*.obj"/>' \
	  '  </mime-type>' \
	  '</mime-info>' \
	  > $(HOME)/.local/share/mime/packages/myobj.xml

	# update databases (some systems/tools may not exist: tolerate failures)
	update-mime-database $(HOME)/.local/share/mime || true
	update-desktop-database $(HOME)/.local/share/applications || true

	# make it the default for the mime-type (optional)
	xdg-mime default scop.desktop application/x-myobj || true

	# validate (harmless if tool missing)
	desktop-file-validate $(HOME)/.local/share/applications/scop.desktop || true

rmexec:
	rm -f ~/.local/share/applications/scop.desktop
	rm -f ~/.local/share/mime/packages/myobj.xml

.PHONY: all openGL clean fclean cclean closeGL rebuild re exec rmexec