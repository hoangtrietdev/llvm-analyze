// parallel_bullet_flyweight.cpp
// ------------------------------------------
// Large standalone C++ example (~550 lines)
// Flyweight + Bullet System + Parallelizable Loops
// ------------------------------------------
// - Flyweight pattern for BulletType
// - Object Pool for Bullets
// - Spatial grid collision simulation
// - Several update loops easily parallelizable
// - Pure standard C++17 (no extra libs)
// ------------------------------------------

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <chrono>

// ============================================================
// Utility math structures
// ============================================================
struct Vec2 {
    float x = 0, y = 0;
    Vec2() = default;
    Vec2(float a, float b) : x(a), y(b) {}
    Vec2 operator+(const Vec2& o) const { return Vec2{x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return Vec2{x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return Vec2{x * s, y * s}; }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    float length() const { return std::sqrt(x * x + y * y); }
    void normalize() {
        float l = length();
        if (l > 0.0001f) { x /= l; y /= l; }
    }
};

// ============================================================
// Flyweight Pattern for BulletType
// ============================================================
struct BulletType {
    std::string name;
    float speed;
    float radius;
    float damage;

    BulletType(std::string n, float s, float r, float d)
        : name(std::move(n)), speed(s), radius(r), damage(d) {}
};

class BulletTypeFactory {
    std::unordered_map<std::string, std::shared_ptr<BulletType>> types;
    std::mutex mtx;

public:
    std::shared_ptr<BulletType> get(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = types.find(key);
        if (it != types.end()) return it->second;
        // Create a new shared type
        if (key == "small")
            types[key] = std::make_shared<BulletType>(key, 300.f, 2.f, 5.f);
        else if (key == "big")
            types[key] = std::make_shared<BulletType>(key, 150.f, 5.f, 20.f);
        else
            types[key] = std::make_shared<BulletType>(key, 400.f, 1.5f, 3.f);
        return types[key];
    }
};

// ============================================================
// Bullet Object (using Flyweight for type)
// ============================================================
struct Bullet {
    bool active = false;
    Vec2 position;
    Vec2 velocity;
    float life = 0.f;
    std::shared_ptr<BulletType> type;

    void update(float dt) {
        if (!active) return;
        position += velocity * dt;
        life -= dt;
        if (life <= 0) active = false;
    }
};

// ============================================================
// Bullet Pool (Object Pool Pattern)
// ============================================================
class BulletPool {
    std::vector<Bullet> pool;
    std::mutex mtx;

public:
    BulletPool(size_t size = 10000) {
        pool.resize(size);
    }

    Bullet* create(const std::shared_ptr<BulletType>& type, const Vec2& pos, const Vec2& dir) {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& b : pool) {
            if (!b.active) {
                b.active = true;
                b.position = pos;
                b.velocity = dir * type->speed;
                b.type = type;
                b.life = 3.0f;
                return &b;
            }
        }
        return nullptr;
    }

    std::vector<Bullet*> getActive() {
        std::vector<Bullet*> active;
        for (auto& b : pool)
            if (b.active) active.push_back(&b);
        return active;
    }

    void deactivate(Bullet* b) {
        std::lock_guard<std::mutex> lock(mtx);
        b->active = false;
    }

    size_t activeCount() const {
        size_t count = 0;
        for (auto& b : pool) if (b.active) count++;
        return count;
    }
};

// ============================================================
// Target structure for collisions
// ============================================================
struct Target {
    Vec2 pos;
    float radius;
    float health;
    int id;
};

// ============================================================
// Simple Spatial Grid for collision grouping
// ============================================================
struct GridCell {
    std::vector<Bullet*> bullets;
    std::vector<Target*> targets;
};

class SpatialGrid {
    float cellSize;
    int width, height;
    std::vector<GridCell> cells;

    int idx(int x, int y) const { return y * width + x; }

public:
    SpatialGrid(float size, int w, int h)
        : cellSize(size), width(w), height(h), cells(w * h) {}

    void clear() {
        for (auto& c : cells) {
            c.bullets.clear();
            c.targets.clear();
        }
    }

    void insert(Bullet* b) {
        int cx = (int)(b->position.x / cellSize);
        int cy = (int)(b->position.y / cellSize);
        if (cx >= 0 && cy >= 0 && cx < width && cy < height)
            cells[idx(cx, cy)].bullets.push_back(b);
    }

    void insert(Target* t) {
        int cx = (int)(t->pos.x / cellSize);
        int cy = (int)(t->pos.y / cellSize);
        if (cx >= 0 && cy >= 0 && cx < width && cy < height)
            cells[idx(cx, cy)].targets.push_back(t);
    }

    // Detect collisions in each cell
    void detectCollisions(std::vector<Target>& targets) {
        // PARALLELIZABLE
        for (auto& cell : cells) {
            for (Bullet* b : cell.bullets) {
                for (Target* t : cell.targets) {
                    float dx = b->position.x - t->pos.x;
                    float dy = b->position.y - t->pos.y;
                    float dist2 = dx * dx + dy * dy;
                    float r = b->type->radius + t->radius;
                    if (dist2 <= r * r) {
                        t->health -= b->type->damage;
                        b->active = false;
                    }
                }
            }
        }
    }
};

// ============================================================
// Particle System for visual effects (simple simulation)
// ============================================================
struct Particle {
    Vec2 pos;
    Vec2 vel;
    float life;
    bool active;
    void update(float dt) {
        if (!active) return;
        pos += vel * dt;
        life -= dt;
        if (life <= 0) active = false;
    }
};

class ParticleSystem {
    std::vector<Particle> parts;
public:
    ParticleSystem(size_t n=10000) { parts.resize(n); }

    void spawn(const Vec2& p, int count) {
        for (int i=0;i<count && i<(int)parts.size();++i) {
            if (!parts[i].active) {
                parts[i].pos = p;
                parts[i].vel = Vec2(((rand()%100)-50)/50.f, ((rand()%100)-50)/50.f);
                parts[i].life = 1.0f;
                parts[i].active = true;
                return;
            }
        }
    }

    void update(float dt) {
        // PARALLELIZABLE
        for (auto& p : parts) p.update(dt);
    }

    size_t activeCount() const {
        size_t c=0;
        for (auto& p : parts) if (p.active) c++;
        return c;
    }
};

// ============================================================
// GameWorld Simulation
// ============================================================
class GameWorld {
    BulletTypeFactory typeFactory;
    BulletPool pool;
    SpatialGrid grid;
    ParticleSystem particles;
    std::vector<Target> targets;
    std::mt19937 rng;

public:
    GameWorld()
        : pool(50000), grid(50, 100, 100), particles(30000) {
        rng.seed(std::random_device{}());
        createTargets(200);
    }

    void createTargets(int n) {
        std::uniform_real_distribution<float> pos(0, 5000);
        for (int i=0;i<n;i++)
            targets.push_back(Target{Vec2(pos(rng), pos(rng)), 5.f, 100.f, i});
    }

    void spawnBullets(int n) {
        std::uniform_real_distribution<float> pos(0, 5000);
        std::uniform_real_distribution<float> angle(0, 6.28318f);
        std::uniform_int_distribution<int> typeId(0,2);
        static const std::string types[3]={"small","big","fast"};
        for (int i=0;i<n;i++) {
            Vec2 p(pos(rng), pos(rng));
            float a = angle(rng);
            Vec2 dir(std::cos(a), std::sin(a));
            pool.create(typeFactory.get(types[typeId(rng)]), p, dir);
        }
    }

    void update(float dt) {
        // 1. Move bullets
        auto activeBullets = pool.getActive();
        // PARALLELIZABLE
        for (Bullet* b : activeBullets) b->update(dt);

        // 2. Build spatial grid
        grid.clear();
        for (Bullet* b : activeBullets) if (b->active) grid.insert(b);
        for (auto& t : targets) if (t.health > 0) grid.insert(&t);

        // 3. Collisions
        grid.detectCollisions(targets);

        // 4. Spawn particles where targets are hit
        for (auto& t : targets) {
            if (t.health <= 0) particles.spawn(t.pos, 10);
        }

        // 5. Update particles
        particles.update(dt);
    }

    void debugPrint() {
        std::cout << "Active Bullets: " << pool.activeCount()
                  << " | Active Particles: " << particles.activeCount() << "\n";
        int alive = 0;
        for (auto& t : targets) if (t.health > 0) alive++;
        std::cout << "Targets Alive: " << alive << "\n";
    }
};

// ============================================================
// Main Simulation Loop
// ============================================================
int main() {
    GameWorld world;
    world.spawnBullets(10000);

    auto start = std::chrono::high_resolution_clock::now();
    for (int frame=0; frame<200; ++frame) {
        world.update(0.016f); // ~60fps step
        if (frame % 20 == 0) {
            std::cout << "--- Frame " << frame << " ---\n";
            world.debugPrint();
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(end - start).count();
    std::cout << "Simulation finished in " << elapsed << " seconds\n";
    return 0;
}
