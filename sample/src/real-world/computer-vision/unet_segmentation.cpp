// Image Segmentation - U-Net Architecture
#include <vector>
#include <cmath>
#include <algorithm>

class UNetSegmentation {
public:
    struct ConvBlock {
        std::vector<std::vector<std::vector<std::vector<double>>>> conv1;
        std::vector<std::vector<std::vector<std::vector<double>>>> conv2;
        std::vector<double> bias1, bias2;
        int inChannels, outChannels;
    };
    
    // Encoder path
    std::vector<ConvBlock> encoderBlocks;
    
    // Decoder path
    std::vector<ConvBlock> decoderBlocks;
    
    int numClasses;
    
    UNetSegmentation(int classes) : numClasses(classes) {}
    
    // 2D Convolution
    std::vector<std::vector<std::vector<double>>> conv2d(
        const std::vector<std::vector<std::vector<double>>>& input,
        const std::vector<std::vector<std::vector<std::vector<double>>>>& weights,
        const std::vector<double>& bias,
        int padding = 1) {
        
        int inChannels = input.size();
        int inH = input[0].size();
        int inW = input[0][0].size();
        
        int outChannels = weights.size();
        int kernelSize = weights[0][0].size();
        
        int outH = inH + 2 * padding - kernelSize + 1;
        int outW = inW + 2 * padding - kernelSize + 1;
        
        std::vector<std::vector<std::vector<double>>> output(outChannels,
            std::vector<std::vector<double>>(outH, std::vector<double>(outW, 0)));
        
        // Parallel convolution over output channels
        for (int oc = 0; oc < outChannels; oc++) {
            for (int oh = 0; oh < outH; oh++) {
                for (int ow = 0; ow < outW; ow++) {
                    double sum = bias[oc];
                    
                    for (int ic = 0; ic < inChannels; ic++) {
                        for (int kh = 0; kh < kernelSize; kh++) {
                            for (int kw = 0; kw < kernelSize; kw++) {
                                int ih = oh + kh - padding;
                                int iw = ow + kw - padding;
                                
                                if (ih >= 0 && ih < inH && iw >= 0 && iw < inW) {
                                    sum += input[ic][ih][iw] * weights[oc][ic][kh][kw];
                                }
                            }
                        }
                    }
                    
                    output[oc][oh][ow] = sum;
                }
            }
        }
        
        return output;
    }
    
    // ReLU activation
    void applyReLU(std::vector<std::vector<std::vector<double>>>& data) {
        for (auto& channel : data) {
            for (auto& row : channel) {
                for (auto& val : row) {
                    val = std::max(0.0, val);
                }
            }
        }
    }
    
    // Max pooling 2x2
    std::vector<std::vector<std::vector<double>>> maxPool2x2(
        const std::vector<std::vector<std::vector<double>>>& input) {
        
        int channels = input.size();
        int inH = input[0].size();
        int inW = input[0][0].size();
        
        int outH = inH / 2;
        int outW = inW / 2;
        
        std::vector<std::vector<std::vector<double>>> output(channels,
            std::vector<std::vector<double>>(outH, std::vector<double>(outW, 0)));
        
        for (int c = 0; c < channels; c++) {
            for (int oh = 0; oh < outH; oh++) {
                for (int ow = 0; ow < outW; ow++) {
                    double maxVal = input[c][oh*2][ow*2];
                    maxVal = std::max(maxVal, input[c][oh*2][ow*2 + 1]);
                    maxVal = std::max(maxVal, input[c][oh*2 + 1][ow*2]);
                    maxVal = std::max(maxVal, input[c][oh*2 + 1][ow*2 + 1]);
                    
                    output[c][oh][ow] = maxVal;
                }
            }
        }
        
        return output;
    }
    
    // Transpose convolution (upsampling)
    std::vector<std::vector<std::vector<double>>> transposeConv(
        const std::vector<std::vector<std::vector<double>>>& input,
        int scale = 2) {
        
        int channels = input.size();
        int inH = input[0].size();
        int inW = input[0][0].size();
        
        int outH = inH * scale;
        int outW = inW * scale;
        
        std::vector<std::vector<std::vector<double>>> output(channels,
            std::vector<std::vector<double>>(outH, std::vector<double>(outW, 0)));
        
        // Bilinear upsampling
        for (int c = 0; c < channels; c++) {
            for (int oh = 0; oh < outH; oh++) {
                for (int ow = 0; ow < outW; ow++) {
                    double ih = oh / static_cast<double>(scale);
                    double iw = ow / static_cast<double>(scale);
                    
                    int ih0 = static_cast<int>(ih);
                    int iw0 = static_cast<int>(iw);
                    int ih1 = std::min(ih0 + 1, inH - 1);
                    int iw1 = std::min(iw0 + 1, inW - 1);
                    
                    double dh = ih - ih0;
                    double dw = iw - iw0;
                    
                    output[c][oh][ow] = 
                        (1 - dh) * (1 - dw) * input[c][ih0][iw0] +
                        (1 - dh) * dw * input[c][ih0][iw1] +
                        dh * (1 - dw) * input[c][ih1][iw0] +
                        dh * dw * input[c][ih1][iw1];
                }
            }
        }
        
        return output;
    }
    
    // Concatenate feature maps (skip connections)
    std::vector<std::vector<std::vector<double>>> concatenate(
        const std::vector<std::vector<std::vector<double>>>& x1,
        const std::vector<std::vector<std::vector<double>>>& x2) {
        
        std::vector<std::vector<std::vector<double>>> result = x1;
        result.insert(result.end(), x2.begin(), x2.end());
        return result;
    }
    
    // Encoder block
    std::vector<std::vector<std::vector<double>>> encoderBlock(
        const std::vector<std::vector<std::vector<double>>>& input,
        const ConvBlock& block) {
        
        // Conv -> ReLU -> Conv -> ReLU
        auto x = conv2d(input, block.conv1, block.bias1);
        applyReLU(x);
        
        x = conv2d(x, block.conv2, block.bias2);
        applyReLU(x);
        
        return x;
    }
    
    // Decoder block
    std::vector<std::vector<std::vector<double>>> decoderBlock(
        const std::vector<std::vector<std::vector<double>>>& input,
        const std::vector<std::vector<std::vector<double>>>& skipConnection,
        const ConvBlock& block) {
        
        // Upsample
        auto up = transposeConv(input, 2);
        
        // Concatenate with skip connection
        auto concat = concatenate(up, skipConnection);
        
        // Conv -> ReLU -> Conv -> ReLU
        auto x = conv2d(concat, block.conv1, block.bias1);
        applyReLU(x);
        
        x = conv2d(x, block.conv2, block.bias2);
        applyReLU(x);
        
        return x;
    }
    
    // Full U-Net forward pass
    std::vector<std::vector<std::vector<double>>> forward(
        const std::vector<std::vector<std::vector<double>>>& input) {
        
        std::vector<std::vector<std::vector<std::vector<double>>>> skipConnections;
        
        // Encoder path
        auto x = input;
        for (const auto& block : encoderBlocks) {
            x = encoderBlock(x, block);
            skipConnections.push_back(x);
            x = maxPool2x2(x);
        }
        
        // Decoder path
        for (size_t i = 0; i < decoderBlocks.size(); i++) {
            size_t skipIdx = skipConnections.size() - 1 - i;
            x = decoderBlock(x, skipConnections[skipIdx], decoderBlocks[i]);
        }
        
        return x;
    }
    
    // Dice loss for segmentation
    double diceLoss(
        const std::vector<std::vector<std::vector<double>>>& pred,
        const std::vector<std::vector<std::vector<double>>>& target) {
        
        double intersection = 0, predSum = 0, targetSum = 0;
        
        int channels = pred.size();
        int H = pred[0].size();
        int W = pred[0][0].size();
        
        for (int c = 0; c < channels; c++) {
            for (int h = 0; h < H; h++) {
                for (int w = 0; w < W; w++) {
                    intersection += pred[c][h][w] * target[c][h][w];
                    predSum += pred[c][h][w];
                    targetSum += target[c][h][w];
                }
            }
        }
        
        return 1.0 - (2.0 * intersection + 1e-7) / (predSum + targetSum + 1e-7);
    }
    
    // IoU (Intersection over Union)
    double computeIoU(
        const std::vector<std::vector<std::vector<double>>>& pred,
        const std::vector<std::vector<std::vector<double>>>& target,
        int classIdx) {
        
        double intersection = 0, union_area = 0;
        
        int H = pred[0].size();
        int W = pred[0][0].size();
        
        for (int h = 0; h < H; h++) {
            for (int w = 0; w < W; w++) {
                double p = pred[classIdx][h][w] > 0.5 ? 1.0 : 0.0;
                double t = target[classIdx][h][w];
                
                intersection += p * t;
                union_area += std::max(p, t);
            }
        }
        
        return intersection / (union_area + 1e-7);
    }
    
    // Softmax activation for multi-class segmentation
    std::vector<std::vector<std::vector<double>>> softmax(
        const std::vector<std::vector<std::vector<double>>>& logits) {
        
        int channels = logits.size();
        int H = logits[0].size();
        int W = logits[0][0].size();
        
        auto output = logits;
        
        for (int h = 0; h < H; h++) {
            for (int w = 0; w < W; w++) {
                // Find max for numerical stability
                double maxLogit = logits[0][h][w];
                for (int c = 1; c < channels; c++) {
                    maxLogit = std::max(maxLogit, logits[c][h][w]);
                }
                
                // Compute exp and sum
                double sumExp = 0;
                for (int c = 0; c < channels; c++) {
                    output[c][h][w] = std::exp(logits[c][h][w] - maxLogit);
                    sumExp += output[c][h][w];
                }
                
                // Normalize
                for (int c = 0; c < channels; c++) {
                    output[c][h][w] /= sumExp;
                }
            }
        }
        
        return output;
    }
    
    // Post-processing: CRF (Conditional Random Field) refinement
    std::vector<std::vector<std::vector<double>>> applyCRF(
        const std::vector<std::vector<std::vector<double>>>& prediction,
        const std::vector<std::vector<std::vector<double>>>& image) {
        
        // Simplified CRF - bilateral filtering
        auto refined = prediction;
        int iterations = 5;
        
        for (int iter = 0; iter < iterations; iter++) {
            refined = bilateralFilter(refined, image);
        }
        
        return refined;
    }
    
private:
    std::vector<std::vector<std::vector<double>>> bilateralFilter(
        const std::vector<std::vector<std::vector<double>>>& input,
        const std::vector<std::vector<std::vector<double>>>& guide) {
        
        // Simplified bilateral filtering
        return input;
    }
};

int main() {
    UNetSegmentation unet(3);  // 3 classes
    
    // Input image: 1x256x256
    std::vector<std::vector<std::vector<double>>> image(1,
        std::vector<std::vector<double>>(256, std::vector<double>(256, 0.5)));
    
    auto segmentation = unet.forward(image);
    auto probabilities = unet.softmax(segmentation);
    
    return 0;
}
