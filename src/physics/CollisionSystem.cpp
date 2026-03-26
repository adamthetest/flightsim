#include "physics/CollisionSystem.h"
#include "raymath.h"

bool CollisionSystem::SpheresOverlap(Vector3 posA, float rA,
                                     Vector3 posB, float rB) {
    float dist = Vector3Distance(posA, posB);
    return dist < (rA + rB);
}

std::vector<CollisionPair> CollisionSystem::Detect(
    const std::vector<Entity*>& entities) {

    std::vector<CollisionPair> pairs;
    pairs.reserve(16);

    for (size_t i = 0; i < entities.size(); ++i) {
        Entity* a = entities[i];
        if (!a->active) continue;
        for (size_t j = i + 1; j < entities.size(); ++j) {
            Entity* b = entities[j];
            if (!b->active) continue;
            if (SpheresOverlap(a->position, a->boundRadius,
                               b->position, b->boundRadius)) {
                pairs.push_back({a, b});
            }
        }
    }
    return pairs;
}
