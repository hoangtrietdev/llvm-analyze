// Autonomous Vehicle Path Planning with Dynamic Obstacles
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <limits>

class AutonomousPathPlanning {
public:
    struct Point {
        double x, y;
        
        Point(double _x = 0, double _y = 0) : x(_x), y(_y) {}
        
        double distance(const Point& other) const {
            double dx = x - other.x;
            double dy = y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }
    };
    
    struct Obstacle {
        Point center;
        double radius;
        Point velocity;  // For dynamic obstacles
        
        Obstacle(Point c, double r, Point v = Point(0, 0))
            : center(c), radius(r), velocity(v) {}
        
        bool collidesWith(const Point& p, double safetyMargin = 0) const {
            return center.distance(p) < (radius + safetyMargin);
        }
        
        void update(double dt) {
            center.x += velocity.x * dt;
            center.y += velocity.y * dt;
        }
    };
    
    struct VehicleState {
        Point position;
        double heading;  // radians
        double speed;
        
        VehicleState(Point p = Point(), double h = 0, double s = 0)
            : position(p), heading(h), speed(s) {}
    };
    
    struct Path {
        std::vector<Point> waypoints;
        double totalCost;
        
        Path() : totalCost(0) {}
    };
    
    // RRT (Rapidly-exploring Random Tree)
    class RRT {
    public:
        struct Node {
            Point position;
            int parent;
            double cost;
            
            Node(Point p, int par = -1, double c = 0)
                : position(p), parent(par), cost(c) {}
        };
        
        std::vector<Node> tree;
        std::vector<Obstacle> obstacles;
        Point start, goal;
        double goalRadius;
        
        RRT(Point s, Point g, double gr = 1.0)
            : start(s), goal(g), goalRadius(gr) {
            tree.push_back(Node(start));
        }
        
        int findNearest(const Point& p) {
            int nearest = 0;
            double minDist = tree[0].position.distance(p);
            
            for (size_t i = 1; i < tree.size(); i++) {
                double dist = tree[i].position.distance(p);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = i;
                }
            }
            
            return nearest;
        }
        
        Point steer(const Point& from, const Point& to, double maxStep) {
            double dist = from.distance(to);
            
            if (dist <= maxStep) {
                return to;
            }
            
            double ratio = maxStep / dist;
            return Point(
                from.x + (to.x - from.x) * ratio,
                from.y + (to.y - from.y) * ratio
            );
        }
        
        bool isCollisionFree(const Point& from, const Point& to) {
            int steps = 20;
            
            for (int i = 0; i <= steps; i++) {
                double t = (double)i / steps;
                Point p(
                    from.x + (to.x - from.x) * t,
                    from.y + (to.y - from.y) * t
                );
                
                for (const auto& obs : obstacles) {
                    if (obs.collidesWith(p, 0.5)) {
                        return false;
                    }
                }
            }
            
            return true;
        }
        
        Path plan(int maxIterations, double stepSize) {
            for (int iter = 0; iter < maxIterations; iter++) {
                // Sample random point (90%) or goal (10%)
                Point random;
                if (rand() % 10 == 0) {
                    random = goal;
                } else {
                    random = Point(
                        (rand() % 10000) / 100.0,
                        (rand() % 10000) / 100.0
                    );
                }
                
                // Find nearest node
                int nearestIdx = findNearest(random);
                Point nearest = tree[nearestIdx].position;
                
                // Steer towards random point
                Point newPoint = steer(nearest, random, stepSize);
                
                // Check collision
                if (isCollisionFree(nearest, newPoint)) {
                    double cost = tree[nearestIdx].cost + 
                                 nearest.distance(newPoint);
                    tree.push_back(Node(newPoint, nearestIdx, cost));
                    
                    // Check if goal reached
                    if (newPoint.distance(goal) < goalRadius) {
                        return extractPath(tree.size() - 1);
                    }
                }
            }
            
            // Find closest to goal
            int closest = 0;
            double minDist = tree[0].position.distance(goal);
            
            for (size_t i = 1; i < tree.size(); i++) {
                double dist = tree[i].position.distance(goal);
                if (dist < minDist) {
                    minDist = dist;
                    closest = i;
                }
            }
            
            return extractPath(closest);
        }
        
        Path extractPath(int goalIdx) {
            Path path;
            int current = goalIdx;
            
            while (current != -1) {
                path.waypoints.push_back(tree[current].position);
                current = tree[current].parent;
            }
            
            std::reverse(path.waypoints.begin(), path.waypoints.end());
            
            // Compute total cost
            path.totalCost = tree[goalIdx].cost;
            
            return path;
        }
    };
    
    // RRT* (Optimal RRT)
    class RRTStar : public RRT {
    public:
        double searchRadius;
        
        RRTStar(Point s, Point g, double gr = 1.0, double sr = 5.0)
            : RRT(s, g, gr), searchRadius(sr) {}
        
        std::vector<int> findNear(const Point& p, double radius) {
            std::vector<int> near;
            
            for (size_t i = 0; i < tree.size(); i++) {
                if (tree[i].position.distance(p) < radius) {
                    near.push_back(i);
                }
            }
            
            return near;
        }
        
        Path plan(int maxIterations, double stepSize) {
            for (int iter = 0; iter < maxIterations; iter++) {
                Point random;
                if (rand() % 10 == 0) {
                    random = goal;
                } else {
                    random = Point(
                        (rand() % 10000) / 100.0,
                        (rand() % 10000) / 100.0
                    );
                }
                
                int nearestIdx = findNearest(random);
                Point nearest = tree[nearestIdx].position;
                Point newPoint = steer(nearest, random, stepSize);
                
                if (!isCollisionFree(nearest, newPoint)) continue;
                
                // Find near nodes
                auto nearNodes = findNear(newPoint, searchRadius);
                
                // Choose parent with minimum cost
                int bestParent = nearestIdx;
                double minCost = tree[nearestIdx].cost + 
                               nearest.distance(newPoint);
                
                for (int nearIdx : nearNodes) {
                    Point nearPos = tree[nearIdx].position;
                    
                    if (isCollisionFree(nearPos, newPoint)) {
                        double cost = tree[nearIdx].cost + 
                                     nearPos.distance(newPoint);
                        
                        if (cost < minCost) {
                            minCost = cost;
                            bestParent = nearIdx;
                        }
                    }
                }
                
                // Add new node
                tree.push_back(Node(newPoint, bestParent, minCost));
                int newIdx = tree.size() - 1;
                
                // Rewire tree
                for (int nearIdx : nearNodes) {
                    Point nearPos = tree[nearIdx].position;
                    double newCost = minCost + newPoint.distance(nearPos);
                    
                    if (newCost < tree[nearIdx].cost && 
                        isCollisionFree(newPoint, nearPos)) {
                        tree[nearIdx].parent = newIdx;
                        tree[nearIdx].cost = newCost;
                    }
                }
                
                if (newPoint.distance(goal) < goalRadius) {
                    return extractPath(newIdx);
                }
            }
            
            return extractPath(findNearest(goal));
        }
    };
    
    // Dynamic Window Approach (DWA)
    class DWA {
    public:
        struct Trajectory {
            double v, w;  // Linear and angular velocity
            std::vector<VehicleState> states;
            double cost;
            
            Trajectory(double _v = 0, double _w = 0)
                : v(_v), w(_w), cost(0) {}
        };
        
        VehicleState current;
        Point goal;
        std::vector<Obstacle> obstacles;
        
        // Vehicle constraints
        double maxSpeed = 2.0;
        double maxAngularSpeed = 1.0;
        double maxAccel = 0.5;
        double maxAngularAccel = 1.0;
        
        DWA(VehicleState init, Point g) : current(init), goal(g) {}
        
        std::vector<VehicleState> simulate(double v, double w, double dt, int steps) {
            std::vector<VehicleState> states;
            VehicleState state = current;
            
            for (int i = 0; i < steps; i++) {
                // Update state
                state.heading += w * dt;
                state.position.x += v * std::cos(state.heading) * dt;
                state.position.y += v * std::sin(state.heading) * dt;
                state.speed = v;
                
                states.push_back(state);
            }
            
            return states;
        }
        
        double evaluateTrajectory(const Trajectory& traj) {
            if (traj.states.empty()) return -1e9;
            
            // Heading cost
            VehicleState last = traj.states.back();
            double dx = goal.x - last.position.x;
            double dy = goal.y - last.position.y;
            double goalHeading = std::atan2(dy, dx);
            double headingDiff = std::abs(last.heading - goalHeading);
            while (headingDiff > M_PI) headingDiff -= 2 * M_PI;
            
            double headingCost = 1.0 - std::abs(headingDiff) / M_PI;
            
            // Distance cost
            double distCost = 1.0 / (1.0 + last.position.distance(goal));
            
            // Velocity cost (prefer higher speeds)
            double velCost = traj.v / maxSpeed;
            
            // Obstacle cost
            double obsCost = 1.0;
            
            for (const auto& state : traj.states) {
                for (const auto& obs : obstacles) {
                    double dist = state.position.distance(obs.center);
                    
                    if (dist < obs.radius + 0.5) {
                        obsCost = 0;
                        break;
                    } else if (dist < obs.radius + 2.0) {
                        obsCost = std::min(obsCost, 
                                          (dist - obs.radius - 0.5) / 1.5);
                    }
                }
                
                if (obsCost == 0) break;
            }
            
            // Combined cost
            return 2.0 * headingCost + 1.0 * distCost + 
                   1.0 * velCost + 3.0 * obsCost;
        }
        
        Trajectory selectBestTrajectory(double dt) {
            std::vector<Trajectory> trajectories;
            
            // Dynamic window
            double minV = std::max(0.0, current.speed - maxAccel * dt);
            double maxV = std::min(maxSpeed, current.speed + maxAccel * dt);
            
            double minW = -maxAngularSpeed;
            double maxW = maxAngularSpeed;
            
            // Sample velocities
            int vSamples = 10;
            int wSamples = 20;
            
            for (int i = 0; i < vSamples; i++) {
                double v = minV + (maxV - minV) * i / (vSamples - 1);
                
                for (int j = 0; j < wSamples; j++) {
                    double w = minW + (maxW - minW) * j / (wSamples - 1);
                    
                    Trajectory traj(v, w);
                    traj.states = simulate(v, w, dt, 10);
                    traj.cost = evaluateTrajectory(traj);
                    
                    trajectories.push_back(traj);
                }
            }
            
            // Find best trajectory
            auto best = std::max_element(trajectories.begin(), 
                                        trajectories.end(),
                                        [](const Trajectory& a, const Trajectory& b) {
                                            return a.cost < b.cost;
                                        });
            
            return *best;
        }
        
        VehicleState step(double dt) {
            Trajectory best = selectBestTrajectory(dt);
            
            // Apply velocities
            current.heading += best.w * dt;
            current.position.x += best.v * std::cos(current.heading) * dt;
            current.position.y += best.v * std::sin(current.heading) * dt;
            current.speed = best.v;
            
            return current;
        }
    };
    
    // Artificial Potential Field
    class PotentialField {
    public:
        Point goal;
        std::vector<Obstacle> obstacles;
        double attractiveGain = 1.0;
        double repulsiveGain = 10.0;
        double repulsiveRange = 5.0;
        
        PotentialField(Point g) : goal(g) {}
        
        Point computeForce(const Point& position) {
            // Attractive force towards goal
            double dx = goal.x - position.x;
            double dy = goal.y - position.y;
            double dist = std::sqrt(dx * dx + dy * dy);
            
            Point attractive(
                attractiveGain * dx / dist,
                attractiveGain * dy / dist
            );
            
            // Repulsive force from obstacles
            Point repulsive(0, 0);
            
            for (const auto& obs : obstacles) {
                double obsDist = position.distance(obs.center);
                
                if (obsDist < repulsiveRange) {
                    double repForce = repulsiveGain * 
                                     (1.0 / obsDist - 1.0 / repulsiveRange) / 
                                     (obsDist * obsDist);
                    
                    double dx = position.x - obs.center.x;
                    double dy = position.y - obs.center.y;
                    double len = std::sqrt(dx * dx + dy * dy);
                    
                    repulsive.x += repForce * dx / len;
                    repulsive.y += repForce * dy / len;
                }
            }
            
            return Point(attractive.x + repulsive.x,
                        attractive.y + repulsive.y);
        }
        
        Path plan(Point start, double stepSize, int maxSteps) {
            Path path;
            Point current = start;
            
            for (int step = 0; step < maxSteps; step++) {
                path.waypoints.push_back(current);
                
                // Check if goal reached
                if (current.distance(goal) < 1.0) {
                    break;
                }
                
                // Compute force
                Point force = computeForce(current);
                double magnitude = std::sqrt(force.x * force.x + 
                                            force.y * force.y);
                
                if (magnitude < 1e-6) break;
                
                // Take step
                current.x += stepSize * force.x / magnitude;
                current.y += stepSize * force.y / magnitude;
            }
            
            path.waypoints.push_back(goal);
            
            return path;
        }
    };
    
    // Hybrid A* for vehicle planning
    class HybridAStar {
    public:
        struct Node {
            VehicleState state;
            double g, h;
            int parent;
            
            double f() const { return g + h; }
            
            Node(VehicleState s, double _g, double _h, int p = -1)
                : state(s), g(_g), h(_h), parent(p) {}
            
            bool operator>(const Node& other) const {
                return f() > other.f();
            }
        };
        
        VehicleState start, goal;
        std::vector<Obstacle> obstacles;
        
        HybridAStar(VehicleState s, VehicleState g)
            : start(s), goal(g) {}
        
        double heuristic(const VehicleState& state) {
            return state.position.distance(goal.position);
        }
        
        bool isValid(const VehicleState& state) {
            for (const auto& obs : obstacles) {
                if (obs.collidesWith(state.position, 0.5)) {
                    return false;
                }
            }
            return true;
        }
        
        Path plan() {
            std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
            std::vector<Node> nodes;
            
            nodes.push_back(Node(start, 0, heuristic(start)));
            openSet.push(nodes[0]);
            
            while (!openSet.empty()) {
                Node current = openSet.top();
                openSet.pop();
                
                // Check goal
                if (current.state.position.distance(goal.position) < 1.0) {
                    return extractPath(nodes, nodes.size() - 1);
                }
                
                // Expand successors
                std::vector<double> velocities = {-0.5, 0, 0.5, 1.0};
                std::vector<double> steerAngles = {-0.5, -0.25, 0, 0.25, 0.5};
                
                for (double v : velocities) {
                    for (double w : steerAngles) {
                        VehicleState next = current.state;
                        double dt = 0.5;
                        
                        next.heading += w * dt;
                        next.position.x += v * std::cos(next.heading) * dt;
                        next.position.y += v * std::sin(next.heading) * dt;
                        next.speed = v;
                        
                        if (!isValid(next)) continue;
                        
                        double g = current.g + current.state.position.distance(next.position);
                        double h = heuristic(next);
                        
                        nodes.push_back(Node(next, g, h, nodes.size() - 1));
                        openSet.push(nodes.back());
                    }
                }
            }
            
            return Path();
        }
        
        Path extractPath(const std::vector<Node>& nodes, int goalIdx) {
            Path path;
            int current = goalIdx;
            
            while (current != -1) {
                path.waypoints.push_back(nodes[current].state.position);
                current = nodes[current].parent;
            }
            
            std::reverse(path.waypoints.begin(), path.waypoints.end());
            
            return path;
        }
    };
};

int main() {
    AutonomousPathPlanning planner;
    
    // Create environment
    AutonomousPathPlanning::Point start(0, 0);
    AutonomousPathPlanning::Point goal(90, 90);
    
    std::vector<AutonomousPathPlanning::Obstacle> obstacles;
    obstacles.push_back(AutonomousPathPlanning::Obstacle(
        AutonomousPathPlanning::Point(30, 30), 5));
    obstacles.push_back(AutonomousPathPlanning::Obstacle(
        AutonomousPathPlanning::Point(60, 60), 8));
    obstacles.push_back(AutonomousPathPlanning::Obstacle(
        AutonomousPathPlanning::Point(45, 70), 6,
        AutonomousPathPlanning::Point(0.5, -0.3)));  // Moving obstacle
    
    // RRT
    AutonomousPathPlanning::RRT rrt(start, goal, 2.0);
    rrt.obstacles = obstacles;
    auto rrtPath = rrt.plan(1000, 2.0);
    
    // RRT*
    AutonomousPathPlanning::RRTStar rrtStar(start, goal, 2.0, 5.0);
    rrtStar.obstacles = obstacles;
    auto rrtStarPath = rrtStar.plan(1000, 2.0);
    
    // DWA
    AutonomousPathPlanning::VehicleState initState(start, 0, 0);
    AutonomousPathPlanning::DWA dwa(initState, goal);
    dwa.obstacles = obstacles;
    
    for (int step = 0; step < 100; step++) {
        dwa.step(0.1);
        
        // Update dynamic obstacles
        for (auto& obs : obstacles) {
            obs.update(0.1);
        }
    }
    
    // Potential Field
    AutonomousPathPlanning::PotentialField pf(goal);
    pf.obstacles = obstacles;
    auto pfPath = pf.plan(start, 0.5, 200);
    
    // Hybrid A*
    AutonomousPathPlanning::VehicleState startState(start, 0, 0);
    AutonomousPathPlanning::VehicleState goalState(goal, 0, 0);
    AutonomousPathPlanning::HybridAStar hybridAStar(startState, goalState);
    hybridAStar.obstacles = obstacles;
    auto hybridPath = hybridAStar.plan();
    
    return 0;
}
