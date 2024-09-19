# Tracing

Trace the performance of Vulkan backend operations

# Install build dependencies

## Windows

- cmake<br>
https://cmake.org/download/
- vulkan-sdk<br>
https://vulkan.lunarg.com/

# Get the code

```shell
git clone -b b3720-profiling https://github.com/daijh/llama.cpp.git
```

# Build llama.cpp

```shell
cd llama.cpp

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DLLAMA_BUILD_TESTS=1 -DGGML_BUILD_TESTS=0 -DGGML_VULKAN=ON

cmake --build build -j4
```

# Capture trace

```shell
llama-simple -m Phi-3-mini-4k-instruct-Q4_0.gguf -p "who are you?" -ngl 100 -n 42 -s 42 -v
```

A trace file named `xtrace_{pid}.json` will be generated after each run. You can analyze it using the `perfetto` tool.
