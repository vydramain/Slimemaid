mkdir -p ./build/shaders
/opt/vulkansdk/1.3.204.1/x86_64/bin/glslc ./shaders/shader.vert -o ./build/shaders/vert.spv
/opt/vulkansdk/1.3.204.1/x86_64/bin/glslc ./shaders/shader.frag -o ./build/shaders/frag.spv
