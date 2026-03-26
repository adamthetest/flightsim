#pragma once
#include <vector>
#include "entities/Entity.h"

struct CollisionPair {
    Entity* a;
    Entity* b;
};

class CollisionSystem {
public:
    // O(n²) sphere-sphere broadphase + exact check.
    // Returns all overlapping pairs for the caller to resolve.
    static std::vector<CollisionPair> Detect(
        const std::vector<Entity*>& entities);

    static bool SpheresOverlap(Vector3 posA, float rA,
                               Vector3 posB, float rB);
};
