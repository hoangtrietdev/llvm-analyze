// Physical ray tracing simulation
#include <vector>
#include <cmath>

const int WIDTH = 1920, HEIGHT = 1080;
const int NUM_RAYS = 1000000;

struct Ray {
    double ox, oy, oz;  // Origin
    double dx, dy, dz;  // Direction
};

struct Sphere {
    double cx, cy, cz, radius;
    double r, g, b;
};

class RayTracer {
private:
    std::vector<Sphere> spheres;
    std::vector<std::vector<double>> framebuffer;
    
public:
    RayTracer() {
        framebuffer.resize(HEIGHT, std::vector<double>(WIDTH * 3, 0.0));
        spheres.push_back({0, 0, -5, 1.0, 1.0, 0.0, 0.0});
    }
    
    bool intersect_sphere(const Ray& ray, const Sphere& sphere, double& t) {
        double dx = ray.ox - sphere.cx;
        double dy = ray.oy - sphere.cy;
        double dz = ray.oz - sphere.cz;
        
        double a = ray.dx*ray.dx + ray.dy*ray.dy + ray.dz*ray.dz;
        double b = 2.0 * (dx*ray.dx + dy*ray.dy + dz*ray.dz);
        double c = dx*dx + dy*dy + dz*dz - sphere.radius*sphere.radius;
        
        double disc = b*b - 4*a*c;
        if (disc < 0) return false;
        
        t = (-b - sqrt(disc)) / (2.0 * a);
        return t > 0;
    }
    
    void trace_rays() {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                Ray ray;
                ray.ox = (x - WIDTH/2.0) / WIDTH;
                ray.oy = (y - HEIGHT/2.0) / HEIGHT;
                ray.oz = 0;
                ray.dx = 0;
                ray.dy = 0;
                ray.dz = -1;
                
                double min_t = 1e9;
                const Sphere* hit_sphere = nullptr;
                
                for (const auto& sphere : spheres) {
                    double t;
                    if (intersect_sphere(ray, sphere, t) && t < min_t) {
                        min_t = t;
                        hit_sphere = &sphere;
                    }
                }
                
                if (hit_sphere) {
                    framebuffer[y][x*3] = hit_sphere->r;
                    framebuffer[y][x*3+1] = hit_sphere->g;
                    framebuffer[y][x*3+2] = hit_sphere->b;
                }
            }
        }
    }
};

int main() {
    RayTracer tracer;
    tracer.trace_rays();
    return 0;
}
