// Elliptic curve cryptography
#include <cstdint>

struct Point {
    int64_t x, y;
};

Point point_add(const Point& P, const Point& Q, int64_t p, int64_t a) {
    if (P.x == Q.x && P.y == Q.y) {
        // Point doubling
        int64_t s = (3 * P.x * P.x + a) / (2 * P.y);
        int64_t x = (s * s - 2 * P.x) % p;
        int64_t y = (s * (P.x - x) - P.y) % p;
        return {x, y};
    } else {
        // Point addition
        int64_t s = (Q.y - P.y) / (Q.x - P.x);
        int64_t x = (s * s - P.x - Q.x) % p;
        int64_t y = (s * (P.x - x) - P.y) % p;
        return {x, y};
    }
}

Point scalar_mult(const Point& P, int64_t k, int64_t p, int64_t a) {
    Point result = P;
    Point temp = P;
    k--;
    
    while (k > 0) {
        if (k % 2 == 1) {
            result = point_add(result, temp, p, a);
        }
        temp = point_add(temp, temp, p, a);
        k /= 2;
    }
    
    return result;
}

int main() {
    Point G = {15, 13};
    int64_t p = 97, a = 2;
    
    for (int i = 0; i < 10000; i++) {
        Point R = scalar_mult(G, 123456, p, a);
    }
    
    return 0;
}
