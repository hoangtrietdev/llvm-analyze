// Brain Tumor Segmentation - 3D U-Net
#include <vector>
#include <cmath>

void conv3D(double* input, double* kernel, double* output,
           int in_d, int in_h, int in_w, int in_c, int out_c, int k_size) {
    int out_d = in_d - k_size + 1;
    int out_h = in_h - k_size + 1;
    int out_w = in_w - k_size + 1;
    
    for (int oc = 0; oc < out_c; oc++) {
        for (int d = 0; d < out_d; d++) {
            for (int h = 0; h < out_h; h++) {
                for (int w = 0; w < out_w; w++) {
                    double sum = 0.0;
                    
                    for (int ic = 0; ic < in_c; ic++) {
                        for (int kd = 0; kd < k_size; kd++) {
                            for (int kh = 0; kh < k_size; kh++) {
                                for (int kw = 0; kw < k_size; kw++) {
                                    int in_idx = ic*in_d*in_h*in_w + (d+kd)*in_h*in_w + 
                                                (h+kh)*in_w + (w+kw);
                                    int k_idx = oc*in_c*k_size*k_size*k_size + 
                                               ic*k_size*k_size*k_size + 
                                               kd*k_size*k_size + kh*k_size + kw;
                                    sum += input[in_idx] * kernel[k_idx];
                                }
                            }
                        }
                    }
                    
                    int out_idx = oc*out_d*out_h*out_w + d*out_h*out_w + h*out_w + w;
                    output[out_idx] = std::max(0.0, sum); // ReLU
                }
            }
        }
    }
}

void maxPool3D(double* input, double* output, int depth, int height, int width,
              int channels, int pool_size) {
    int out_d = depth / pool_size;
    int out_h = height / pool_size;
    int out_w = width / pool_size;
    
    for (int c = 0; c < channels; c++) {
        for (int d = 0; d < out_d; d++) {
            for (int h = 0; h < out_h; h++) {
                for (int w = 0; w < out_w; w++) {
                    double max_val = -1e9;
                    
                    for (int pd = 0; pd < pool_size; pd++) {
                        for (int ph = 0; ph < pool_size; ph++) {
                            for (int pw = 0; pw < pool_size; pw++) {
                                int in_idx = c*depth*height*width + 
                                           (d*pool_size+pd)*height*width + 
                                           (h*pool_size+ph)*width + (w*pool_size+pw);
                                max_val = std::max(max_val, input[in_idx]);
                            }
                        }
                    }
                    
                    int out_idx = c*out_d*out_h*out_w + d*out_h*out_w + h*out_w + w;
                    output[out_idx] = max_val;
                }
            }
        }
    }
}

void upsample3D(double* input, double* output, int depth, int height, int width,
               int channels, int factor) {
    int out_d = depth * factor;
    int out_h = height * factor;
    int out_w = width * factor;
    
    for (int c = 0; c < channels; c++) {
        for (int d = 0; d < out_d; d++) {
            for (int h = 0; h < out_h; h++) {
                for (int w = 0; w < out_w; w++) {
                    int in_d = d / factor;
                    int in_h = h / factor;
                    int in_w = w / factor;
                    
                    int in_idx = c*depth*height*width + in_d*height*width + in_h*width + in_w;
                    int out_idx = c*out_d*out_h*out_w + d*out_h*out_w + h*out_w + w;
                    
                    output[out_idx] = input[in_idx];
                }
            }
        }
    }
}

void diceCoefficient(int* prediction, int* ground_truth, int size, double& dice) {
    int intersection = 0;
    int pred_sum = 0, gt_sum = 0;
    
    for (int i = 0; i < size; i++) {
        intersection += prediction[i] * ground_truth[i];
        pred_sum += prediction[i];
        gt_sum += ground_truth[i];
    }
    
    dice = 2.0 * intersection / (pred_sum + gt_sum + 1e-10);
}

int main() {
    const int depth = 64, height = 64, width = 64;
    const int n_classes = 4; // Background, necrosis, edema, enhancing tumor
    
    std::vector<double> mri_scan(depth * height * width, 100.0);
    std::vector<double> features1(32 * depth * height * width);
    std::vector<double> features2(64 * depth/2 * height/2 * width/2);
    std::vector<double> upsampled(32 * depth * height * width);
    std::vector<int> segmentation(depth * height * width);
    
    std::vector<double> kernel1(32 * 1 * 3 * 3 * 3, 0.1);
    std::vector<double> kernel2(64 * 32 * 3 * 3 * 3, 0.1);
    
    // Encoder
    conv3D(mri_scan.data(), kernel1.data(), features1.data(),
          depth, height, width, 1, 32, 3);
    
    maxPool3D(features1.data(), features2.data(), depth-2, height-2, width-2, 32, 2);
    
    // Decoder
    upsample3D(features2.data(), upsampled.data(), depth/2-1, height/2-1, width/2-1, 32, 2);
    
    return 0;
}
