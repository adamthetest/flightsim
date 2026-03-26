#include "entities/Entity.h"

EntityId Entity::s_nextId = 1;

Entity::Entity(EntityType t) : type(t) {
    id = s_nextId++;
    rotation = MatrixIdentity();
}
