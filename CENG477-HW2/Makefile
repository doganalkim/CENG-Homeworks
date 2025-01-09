rasterizer:
	g++ -std=c++11 -o rasterizer ./code_template/Main.cpp ./code_template/Scene.cpp ./code_template/Camera.cpp ./code_template/Color.cpp ./code_template/Helpers.cpp ./code_template/Mesh.cpp ./code_template/Matrix4.cpp ./code_template/Rotation.cpp ./code_template/Scaling.cpp ./code_template/tinyxml2.cpp ./code_template/Translation.cpp ./code_template/Triangle.cpp ./code_template/Vec3.cpp ./code_template/Vec4.cpp
c:
	g++ -std=c++11 -o rasterizer ./code_template/Main.cpp ./code_template/Scene.cpp ./code_template/Camera.cpp ./code_template/Color.cpp ./code_template/Helpers.cpp ./code_template/Mesh.cpp ./code_template/Matrix4.cpp ./code_template/Rotation.cpp ./code_template/Scaling.cpp ./code_template/tinyxml2.cpp ./code_template/Translation.cpp ./code_template/Triangle.cpp ./code_template/Vec3.cpp ./code_template/Vec4.cpp
r: 
	./rasterizer ./input_outputs/clipping_example/empty_box_clipped.xml
d:
	clang++  -std=c++11  -g -o  rasterizer ./code_template/Main.cpp ./code_template/Scene.cpp ./code_template/Camera.cpp ./code_template/Color.cpp ./code_template/Helpers.cpp ./code_template/Mesh.cpp ./code_template/Matrix4.cpp ./code_template/Rotation.cpp ./code_template/Scaling.cpp ./code_template/tinyxml2.cpp ./code_template/Translation.cpp ./code_template/Triangle.cpp ./code_template/Vec3.cpp ./code_template/Vec4.cpp
a: 
	./rasterizer ./input_outputs/different_projection_type/horse_and_mug/horse_and_mug_perspective.xml
b: 
	./rasterizer ./input_outputs/different_projection_type/flag_turkey/flag_turkey_perspective.xml
e: 
	./rasterizer ./input_outputs/culling_enabled_inputs/empty_box.xml
f: 
	./rasterizer ./input_outputs/culling_enabled_inputs/horse_and_mug.xml
g: 
	./rasterizer ./input_outputs/culling_disabled_inputs/horse_and_mug.xml