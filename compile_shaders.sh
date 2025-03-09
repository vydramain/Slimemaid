mkdir -p cmake-build-debug/shaders
glslc ./shaders/shader.vert -o ./cmake-build-debug/shaders/vert.spv
glslc ./shaders/shader.frag -o ./cmake-build-debug/shaders/frag.spv
