// Semantic Segmentation - FCN (Fully Convolutional Network)
#include <vector>
#include <cmath>

void convolution2D(double* input, double* kernel, double* output,
                  int in_width, int in_height, int in_channels,
                  int kernel_size, int out_channels) {
    int out_width = in_width - kernel_size + 1;
    int out_height = in_height - kernel_size + 1;
    
    for (int oc = 0; oc < out_channels; oc++) {
        for (int y = 0; y < out_height; y++) {
            for (int x = 0; x < out_width; x++) {
                double sum = 0.0;
                
                for (int ic = 0; ic < in_channels; ic++) {
                    for (int ky = 0; ky < kernel_size; ky++) {
                        for (int kx = 0; kx < kernel_size; kx++) {
                            int in_y = y + ky;
                            int in_x = x + kx;
                            
                            sum += input[ic*in_width*in_height + in_y*in_width + in_x] *
                                  kernel[oc*in_channels*kernel_size*kernel_size + 
                                        ic*kernel_size*kernel_size + ky*kernel_size + kx];
                        }
                    }
                }
                
                output[oc*out_width*out_height + y*out_width + x] = sum;
            }
        }
    }
}

void upsample2D(double* input, double* output, int width, int height, int channels, int factor) {
    int out_width = width * factor;
    int out_height = height * factor;
    
    for (int c = 0; c < channels; c++) {
        for (int y = 0; y < out_height; y++) {
            for (int x = 0; x < out_width; x++) {
                int in_y = y / factor;
                int in_x = x / factor;
                
                output[c*out_width*out_height + y*out_width + x] = 
                    input[c*width*height + in_y*width + in_x];
            }
        }
    }
}

void semanticSegmentation(double* image, int* segmentation_map, int width, int height,
                         int n_classes) {
    std::vector<double> features1(width*height*64);
    std::vector<double> features2(width*height/4*128);
    std::vector<double> upsampled(width*height*n_classes);
    
    std::vector<double> kernel1(64*3*3*3, 0.1);
    std::vector<double> kernel2(128*64*3*3, 0.1);
    
    // Encoder
    convolution2D(image, kernel1.data(), features1.data(), width, height, 3, 3, 64);
    convolution2D(features1.data(), kernel2.data(), features2.data(), 
                 width-2, height-2, 64, 3, 128);
    
    // Decoder
    upsample2D(features2.data(), upsampled.data(), width/2-4, height/2-4, n_classes, 2);
    
    // Argmax for final prediction
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double max_score = -1e9;
            int max_class = 0;
            
            for (int c = 0; c < n_classes; c++) {
                if (upsampled[c*width*height + y*width + x] > max_score) {
                    max_score = upsampled[c*width*height + y*width + x];
                    max_class = c;
                }
            }
            
            segmentation_map[y*width + x] = max_class;
        }
    }
}

int main() {
    const int width = 256, height = 256, n_classes = 21;
    std::vector<double> image(width*height*3, 128.0);
    std::vector<int> segmentation_map(width*height);
    
    semanticSegmentation(image.data(), segmentation_map.data(), width, height, n_classes);
    
    return 0;
}
