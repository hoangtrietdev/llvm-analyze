// Object Detection - YOLO (You Only Look Once)
#include <vector>
#include <cmath>
#include <algorithm>

struct BoundingBox {
    double x, y, width, height;
    double confidence;
    int class_id;
};

void yoloConvLayer(double* input, double* kernel, double* output,
                  int in_h, int in_w, int in_c, int out_c, int k_size, int stride) {
    int out_h = (in_h - k_size) / stride + 1;
    int out_w = (in_w - k_size) / stride + 1;
    
    for (int oc = 0; oc < out_c; oc++) {
        for (int h = 0; h < out_h; h++) {
            for (int w = 0; w < out_w; w++) {
                double sum = 0.0;
                
                for (int ic = 0; ic < in_c; ic++) {
                    for (int kh = 0; kh < k_size; kh++) {
                        for (int kw = 0; kw < k_size; kw++) {
                            int in_h_idx = h * stride + kh;
                            int in_w_idx = w * stride + kw;
                            
                            int in_idx = ic*in_h*in_w + in_h_idx*in_w + in_w_idx;
                            int k_idx = oc*in_c*k_size*k_size + ic*k_size*k_size + kh*k_size + kw;
                            
                            sum += input[in_idx] * kernel[k_idx];
                        }
                    }
                }
                
                int out_idx = oc*out_h*out_w + h*out_w + w;
                output[out_idx] = std::max(0.0, sum); // Leaky ReLU
            }
        }
    }
}

void decodeYOLOOutput(double* predictions, std::vector<BoundingBox>& boxes,
                     int grid_h, int grid_w, int n_anchors, int n_classes,
                     double** anchor_boxes, double conf_threshold) {
    int output_per_anchor = 5 + n_classes; // x, y, w, h, conf, classes
    
    for (int h = 0; h < grid_h; h++) {
        for (int w = 0; w < grid_w; w++) {
            for (int a = 0; a < n_anchors; a++) {
                int base_idx = (h*grid_w*n_anchors + w*n_anchors + a) * output_per_anchor;
                
                // Decode bounding box
                double tx = predictions[base_idx + 0];
                double ty = predictions[base_idx + 1];
                double tw = predictions[base_idx + 2];
                double th = predictions[base_idx + 3];
                double conf_raw = predictions[base_idx + 4];
                
                // Sigmoid activation
                double bx = 1.0 / (1.0 + exp(-tx)) + w;
                double by = 1.0 / (1.0 + exp(-ty)) + h;
                double bw = anchor_boxes[a][0] * exp(tw);
                double bh = anchor_boxes[a][1] * exp(th);
                double confidence = 1.0 / (1.0 + exp(-conf_raw));
                
                if (confidence < conf_threshold) continue;
                
                // Find best class
                int best_class = 0;
                double best_class_prob = -1e9;
                
                for (int c = 0; c < n_classes; c++) {
                    double class_prob = predictions[base_idx + 5 + c];
                    if (class_prob > best_class_prob) {
                        best_class_prob = class_prob;
                        best_class = c;
                    }
                }
                
                double final_conf = confidence * (1.0 / (1.0 + exp(-best_class_prob)));
                
                if (final_conf > conf_threshold) {
                    BoundingBox box;
                    box.x = bx / grid_w;
                    box.y = by / grid_h;
                    box.width = bw;
                    box.height = bh;
                    box.confidence = final_conf;
                    box.class_id = best_class;
                    boxes.push_back(box);
                }
            }
        }
    }
}

double iou(const BoundingBox& a, const BoundingBox& b) {
    double x1 = std::max(a.x - a.width/2, b.x - b.width/2);
    double y1 = std::max(a.y - a.height/2, b.y - b.height/2);
    double x2 = std::min(a.x + a.width/2, b.x + b.width/2);
    double y2 = std::min(a.y + a.height/2, b.y + b.height/2);
    
    if (x2 < x1 || y2 < y1) return 0.0;
    
    double intersection = (x2 - x1) * (y2 - y1);
    double union_area = a.width * a.height + b.width * b.height - intersection;
    
    return intersection / union_area;
}

void nonMaxSuppression(std::vector<BoundingBox>& boxes, double iou_threshold) {
    std::sort(boxes.begin(), boxes.end(),
             [](const BoundingBox& a, const BoundingBox& b) {
                 return a.confidence > b.confidence;
             });
    
    std::vector<bool> suppressed(boxes.size(), false);
    
    for (size_t i = 0; i < boxes.size(); i++) {
        if (suppressed[i]) continue;
        
        for (size_t j = i + 1; j < boxes.size(); j++) {
            if (suppressed[j]) continue;
            
            if (boxes[i].class_id == boxes[j].class_id) {
                double overlap = iou(boxes[i], boxes[j]);
                
                if (overlap > iou_threshold) {
                    suppressed[j] = true;
                }
            }
        }
    }
    
    std::vector<BoundingBox> filtered;
    for (size_t i = 0; i < boxes.size(); i++) {
        if (!suppressed[i]) {
            filtered.push_back(boxes[i]);
        }
    }
    
    boxes = filtered;
}

int main() {
    const int img_h = 416, img_w = 416;
    const int grid_h = 13, grid_w = 13;
    const int n_anchors = 5, n_classes = 80;
    
    std::vector<double> image(img_h * img_w * 3, 128.0);
    std::vector<double> features(grid_h * grid_w * 512);
    std::vector<double> predictions(grid_h * grid_w * n_anchors * (5 + n_classes));
    std::vector<BoundingBox> detections;
    
    std::vector<std::vector<double>> anchors = {
        {1.08, 1.19}, {3.42, 4.41}, {6.63, 11.38}, {9.42, 5.11}, {16.62, 10.52}
    };
    
    std::vector<double*> anchor_ptrs(n_anchors);
    for (int i = 0; i < n_anchors; i++) {
        anchor_ptrs[i] = anchors[i].data();
    }
    
    decodeYOLOOutput(predictions.data(), detections, grid_h, grid_w, n_anchors,
                    n_classes, anchor_ptrs.data(), 0.5);
    
    nonMaxSuppression(detections, 0.45);
    
    return 0;
}
