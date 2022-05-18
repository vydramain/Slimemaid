mkdir -p cmake-build-debug/shaders
/opt/vulkansdk/1.3.204.1/x86_64/bin/glslc ./shaders/shader.vert -o ./cmake-build-debug/shaders/vert.spv
/opt/vulkansdk/1.3.204.1/x86_64/bin/glslc ./shaders/shader.frag -o ./cmake-build-debug/shaders/frag.spv
