Scop

Scop is a lightweight C++ application that renders 3D .obj models using OpenGL. It includes a custom parser for .obj and .mtl files, and uses ImGui for an interactive legend/UI to control rendering modes and textures.

🧠 Features

Load and display 3D .obj models

Support for default and user-specified textures

Multiple rendering modes:

Faces (textured)

Wireframe

Points

Shaded colors

Interactive legend + controls via ImGui

Can be launched either from the terminal or by double-clicking .obj files (on Linux)

🛠️ Dependencies

This project uses several external libraries to provide graphics and UI capabilities. You do not need to modify them — they are included as dependencies and used as-is.

Included libraries:

GLFW — window creation & input handling

glad — OpenGL function loader

ImGui — Immediate mode GUI for the UI legend

stb_image (std_image.*) — texture loading

These folders and source files are bundled in Includes/... and compiled alongside the project so that you don’t need to install them separately.

📁 Folder Structure
Includes/           → External libs (GLFW, ImGui, glad, stb_image) and Custom libs(vml, a vector, matrix library) and other headers
Resources/          → Default textures & models + Test Models
ShadersFiles/       → Vertex/fragment shader sources
Textures/           → Texture images
*.cpp / *.hpp       → Application & Parser code
Makefile

🚀 Build & Compile

From the project root, simply run:

make


This uses the included Makefile to compile and link the application with the necessary dependencies.

▶️ How to Run

There are two ways to launch the program after compilation.

1. Terminal (Cross-platform)
./scop path/to/model.obj [optional/path/to/texture.png]


path/to/model.obj — required .obj file to render

[optional/path/to/texture.png] — texture to apply (if provided)

Example:

./scop Resources/teapot.obj Textures/wood.png

2. Double-click Opening (Linux only)

Run first:

make exec

This create the .desktop file and the MIME file necessary for linux to understand how to open the software with the object

After building, the executable is registered with the .obj file type on Linux.
You can simply double-click a .obj file in your file manager to open it in Scop (if the “make exec” step has been run and file associations are set appropriately).

🖱 Controls & UI

Once the model is loaded, you can interact with it using:

Mouse drag — rotate camera

Scroll — zoom

ImGui legend — switch rendering modes (faces, lines, points)

Light Controls — 0 - 9

Model Control — Arrows(Rotation), keyPad(translation), reset(r)

Toggle default vs custom texture

The UI panel displays options and current settings.

📦 Notes on External Code

You will see files like glad.c and stb_image.* in the source tree. These are third-party sources included to provide functionality such as OpenGL loading and image decoding. They are not authored by this project, but are compiled and linked into the application so that everything works out-of-the-box.

🧩 Contributions

Feel free to open issues or pull requests if you’d like to improve the renderer or add features!

📝 License & Credits

learnopengl.com : for the Tutorial on OpenGL
