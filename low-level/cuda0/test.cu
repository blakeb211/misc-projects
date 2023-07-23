#include <iostream>
#include <cuda_runtime.h>

// CUDA kernel for vector addition
__global__ void vectorAdd(const float* a, const float* b, float* c, int size) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < size) {
        c[idx] = a[idx] + b[idx];
    }
}

bool verify_device_avail() {
    int deviceCount{0};
    cudaGetDeviceCount(&deviceCount);
    std::cout << "Number of working devices found: " << deviceCount << "\n";
    if (deviceCount > 0) {
        return true;
    } else {
        return false;
    }
}

int main() {
    if (verify_device_avail()) {
        cudaSetDevice(0);
    }
    const int size = 1024; // Size of the arrays
    const int threadsPerBlock = 256;
    const int blocksPerGrid = (size + threadsPerBlock - 1) / threadsPerBlock;

    float* hostA = new float[size];
    float* hostB = new float[size];
    float* hostC = new float[size];

    // Initialize arrays
    for (int i = 0; i < size; i++) {
        hostA[i] = static_cast<float>(i);
        hostB[i] = static_cast<float>(2 * i);
    }

    // Device (GPU) arrays
    float* deviceA;
    float* deviceB;
    float* deviceC;

    // Allocate memory on the GPU
    cudaMalloc((void**)&deviceA, size * sizeof(float));
    cudaMalloc((void**)&deviceB, size * sizeof(float));
    cudaMalloc((void**)&deviceC, size * sizeof(float));

    // Copy data from the host to the device
    cudaMemcpy(deviceA, hostA, size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(deviceB, hostB, size * sizeof(float), cudaMemcpyHostToDevice);

    // Launch the kernel on the GPU
    vectorAdd<<<blocksPerGrid, threadsPerBlock>>>(deviceA, deviceB, deviceC, size);

    // Copy the result from the device to the host
    cudaMemcpy(hostC, deviceC, size * sizeof(float), cudaMemcpyDeviceToHost);

    // Print the result
    for (int i = 0; i < size; i++) {
        std::cout << hostA[i] << " + " << hostB[i] << " = " << hostC[i] << std::endl;
    }

    // Free device and host memory
    cudaFree(deviceA);
    cudaFree(deviceB);
    cudaFree(deviceC);
    delete[] hostA;
    delete[] hostB;
    delete[] hostC;

    return 0;
}

