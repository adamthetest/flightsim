#pragma once

struct ResourceBundle;
class TradeStation;

struct TradeOffer {
    int neutrinoSellPrice; // credits per neutrino
    int shieldRepairCost;  // credits for full shield repair
    int fuelReplenishCost; // credits for full reactor refuel
};

class TradingSystem {
public:
    // Returns false if player cannot afford the action.
    static bool SellNeutrinos(ResourceBundle& res, const TradeStation& station, int amount);
    static bool BuyShieldRepair(ResourceBundle& res, const TradeStation& station,
                                float& playerShields);
    static bool BuyFuelReplenish(ResourceBundle& res, const TradeStation& station,
                                 float& reactorFuel);

    static TradeOffer GetOffer(const TradeStation& station);
};
