// Recurrent Neural Network - LSTM
#include <vector>
#include <cmath>

void sigmoidActivation(double* input, double* output, int size) {
    for (int i = 0; i < size; i++) {
        output[i] = 1.0 / (1.0 + exp(-input[i]));
    }
}

void tanhActivation(double* input, double* output, int size) {
    for (int i = 0; i < size; i++) {
        output[i] = tanh(input[i]);
    }
}

void lstmCell(double* input, double* prev_hidden, double* prev_cell,
             double* Wf, double* Wi, double* Wc, double* Wo,
             double* Uf, double* Ui, double* Uc, double* Uo,
             double* bf, double* bi, double* bc, double* bo,
             double* new_hidden, double* new_cell,
             int input_size, int hidden_size) {
    std::vector<double> forget_gate(hidden_size);
    std::vector<double> input_gate(hidden_size);
    std::vector<double> candidate_cell(hidden_size);
    std::vector<double> output_gate(hidden_size);
    
    // Forget gate: f_t = σ(W_f·x_t + U_f·h_{t-1} + b_f)
    for (int h = 0; h < hidden_size; h++) {
        double sum = bf[h];
        
        for (int i = 0; i < input_size; i++) {
            sum += Wf[h * input_size + i] * input[i];
        }
        
        for (int i = 0; i < hidden_size; i++) {
            sum += Uf[h * hidden_size + i] * prev_hidden[i];
        }
        
        forget_gate[h] = 1.0 / (1.0 + exp(-sum));
    }
    
    // Input gate: i_t = σ(W_i·x_t + U_i·h_{t-1} + b_i)
    for (int h = 0; h < hidden_size; h++) {
        double sum = bi[h];
        
        for (int i = 0; i < input_size; i++) {
            sum += Wi[h * input_size + i] * input[i];
        }
        
        for (int i = 0; i < hidden_size; i++) {
            sum += Ui[h * hidden_size + i] * prev_hidden[i];
        }
        
        input_gate[h] = 1.0 / (1.0 + exp(-sum));
    }
    
    // Candidate cell: C̃_t = tanh(W_C·x_t + U_C·h_{t-1} + b_C)
    for (int h = 0; h < hidden_size; h++) {
        double sum = bc[h];
        
        for (int i = 0; i < input_size; i++) {
            sum += Wc[h * input_size + i] * input[i];
        }
        
        for (int i = 0; i < hidden_size; i++) {
            sum += Uc[h * hidden_size + i] * prev_hidden[i];
        }
        
        candidate_cell[h] = tanh(sum);
    }
    
    // Output gate: o_t = σ(W_o·x_t + U_o·h_{t-1} + b_o)
    for (int h = 0; h < hidden_size; h++) {
        double sum = bo[h];
        
        for (int i = 0; i < input_size; i++) {
            sum += Wo[h * input_size + i] * input[i];
        }
        
        for (int i = 0; i < hidden_size; i++) {
            sum += Uo[h * hidden_size + i] * prev_hidden[i];
        }
        
        output_gate[h] = 1.0 / (1.0 + exp(-sum));
    }
    
    // Update cell state: C_t = f_t ⊙ C_{t-1} + i_t ⊙ C̃_t
    for (int h = 0; h < hidden_size; h++) {
        new_cell[h] = forget_gate[h] * prev_cell[h] + input_gate[h] * candidate_cell[h];
    }
    
    // Update hidden state: h_t = o_t ⊙ tanh(C_t)
    for (int h = 0; h < hidden_size; h++) {
        new_hidden[h] = output_gate[h] * tanh(new_cell[h]);
    }
}

void lstmForward(double** inputs, int seq_len, int input_size, int hidden_size,
                double* Wf, double* Wi, double* Wc, double* Wo,
                double* Uf, double* Ui, double* Uc, double* Uo,
                double* bf, double* bi, double* bc, double* bo,
                double** hidden_states, double** cell_states) {
    std::vector<double> prev_hidden(hidden_size, 0.0);
    std::vector<double> prev_cell(hidden_size, 0.0);
    
    for (int t = 0; t < seq_len; t++) {
        lstmCell(inputs[t], prev_hidden.data(), prev_cell.data(),
                Wf, Wi, Wc, Wo, Uf, Ui, Uc, Uo,
                bf, bi, bc, bo,
                hidden_states[t], cell_states[t],
                input_size, hidden_size);
        
        for (int h = 0; h < hidden_size; h++) {
            prev_hidden[h] = hidden_states[t][h];
            prev_cell[h] = cell_states[t][h];
        }
    }
}

void biDirectionalLSTM(double** inputs, int seq_len, int input_size, int hidden_size,
                      double** forward_hidden, double** backward_hidden) {
    std::vector<double> Wf(hidden_size * input_size, 0.1);
    std::vector<double> Wi(hidden_size * input_size, 0.1);
    std::vector<double> Wc(hidden_size * input_size, 0.1);
    std::vector<double> Wo(hidden_size * input_size, 0.1);
    
    std::vector<double> Uf(hidden_size * hidden_size, 0.1);
    std::vector<double> Ui(hidden_size * hidden_size, 0.1);
    std::vector<double> Uc(hidden_size * hidden_size, 0.1);
    std::vector<double> Uo(hidden_size * hidden_size, 0.1);
    
    std::vector<double> bf(hidden_size, 0.0);
    std::vector<double> bi(hidden_size, 0.0);
    std::vector<double> bc(hidden_size, 0.0);
    std::vector<double> bo(hidden_size, 0.0);
    
    std::vector<std::vector<double>> cell_states_forward(seq_len, std::vector<double>(hidden_size));
    std::vector<std::vector<double>> cell_states_backward(seq_len, std::vector<double>(hidden_size));
    
    std::vector<double*> cell_f_ptrs(seq_len);
    std::vector<double*> cell_b_ptrs(seq_len);
    
    for (int i = 0; i < seq_len; i++) {
        cell_f_ptrs[i] = cell_states_forward[i].data();
        cell_b_ptrs[i] = cell_states_backward[i].data();
    }
    
    // Forward pass
    lstmForward(inputs, seq_len, input_size, hidden_size,
               Wf.data(), Wi.data(), Wc.data(), Wo.data(),
               Uf.data(), Ui.data(), Uc.data(), Uo.data(),
               bf.data(), bi.data(), bc.data(), bo.data(),
               forward_hidden, cell_f_ptrs.data());
    
    // Backward pass (reverse inputs)
    std::vector<double*> reversed_inputs(seq_len);
    for (int i = 0; i < seq_len; i++) {
        reversed_inputs[i] = inputs[seq_len - 1 - i];
    }
    
    lstmForward(reversed_inputs.data(), seq_len, input_size, hidden_size,
               Wf.data(), Wi.data(), Wc.data(), Wo.data(),
               Uf.data(), Ui.data(), Uc.data(), Uo.data(),
               bf.data(), bi.data(), bc.data(), bo.data(),
               backward_hidden, cell_b_ptrs.data());
}

int main() {
    const int seq_len = 100;
    const int input_size = 50;
    const int hidden_size = 128;
    
    std::vector<std::vector<double>> inputs(seq_len, std::vector<double>(input_size, 0.1));
    std::vector<std::vector<double>> forward_hidden(seq_len, std::vector<double>(hidden_size));
    std::vector<std::vector<double>> backward_hidden(seq_len, std::vector<double>(hidden_size));
    
    std::vector<double*> input_ptrs(seq_len);
    std::vector<double*> forward_ptrs(seq_len);
    std::vector<double*> backward_ptrs(seq_len);
    
    for (int i = 0; i < seq_len; i++) {
        input_ptrs[i] = inputs[i].data();
        forward_ptrs[i] = forward_hidden[i].data();
        backward_ptrs[i] = backward_hidden[i].data();
    }
    
    biDirectionalLSTM(input_ptrs.data(), seq_len, input_size, hidden_size,
                     forward_ptrs.data(), backward_ptrs.data());
    
    return 0;
}
