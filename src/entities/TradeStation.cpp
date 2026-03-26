#include "entities/TradeStation.h"
#include "raymath.h"

TradeStation::TradeStation() : Entity(EntityType::TRADE_STATION) {
    modelKey    = "assets/models/environment/trade_station.glb";
    boundRadius = 12.0f;
}

void TradeStation::Update(float dt) {
    spinAngle += spinSpeed * dt;
    rotation = MatrixRotateY(spinAngle);
}

bool TradeStation::IsPlayerInDockRange(Vector3 playerPos) const {
    return Vector3Distance(position, playerPos) <= dockRadius;
}
