#include "systems/TradingSystem.h"
#include "world/World.h"
#include "entities/TradeStation.h"
#include "raymath.h"

TradeOffer TradingSystem::GetOffer(const TradeStation& station) {
    return {station.buyNeutrinos, station.sellShields, 30};
}

bool TradingSystem::SellNeutrinos(ResourceBundle& res, const TradeStation& station, int amount) {
    if (res.neutrinos < amount || amount <= 0) return false;
    res.neutrinos -= amount;
    res.credits   += amount * station.buyNeutrinos;
    return true;
}

bool TradingSystem::BuyShieldRepair(ResourceBundle& res, const TradeStation& station,
                                    float& playerShields) {
    if (res.credits < station.sellShields) return false;
    res.credits   -= station.sellShields;
    playerShields  = 1.0f;
    return true;
}

bool TradingSystem::BuyFuelReplenish(ResourceBundle& res, const TradeStation& station,
                                     float& reactorFuel) {
    constexpr int FUEL_COST = 30;
    if (res.credits < FUEL_COST) return false;
    res.credits  -= FUEL_COST;
    reactorFuel   = 1.0f;
    return true;
}
