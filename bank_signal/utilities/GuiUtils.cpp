#include "GuiUtils.hpp"
#include <exception>

const char* GuiUtils::StateToText(CashierState state)
{
    switch (state)
    {
    case CashierState::Ready:
        return reinterpret_cast<const char*>(u8"ГОТОВА");

    case CashierState::Free:
        return reinterpret_cast<const char*>(u8"СВОБОДНА");

    case CashierState::Busy:
        return reinterpret_cast<const char*>(u8"ЗАНЯТА");

    case CashierState::Offline:
        return reinterpret_cast<const char*>(u8"НЕДОСТУПНА");

    default:
        throw std::exception("unknown state");
    }
}

const char* GuiUtils::StateToDescription(CashierState state)
{
    switch (state)
    {
    case CashierState::Ready:
        return reinterpret_cast<const char*>(u8"Касса готова к работе");

    case CashierState::Free:
        return reinterpret_cast<const char*>(u8"Можно направить клиента");

    case CashierState::Busy:
        return reinterpret_cast<const char*>(u8"Идёт обслуживание клиента");

    case CashierState::Offline:
        return reinterpret_cast<const char*>(u8"Нет связи с процессом кассы");

    default:
        throw std::exception("unknown state");
    }
}

ImVec4 GuiUtils::StateToColor(CashierState state)
{
    switch (state)
    {
    case CashierState::Ready:
        return ImVec4(0.20f, 0.45f, 1.00f, 1.00f); // синий

    case CashierState::Free:
        return ImVec4(0.10f, 0.75f, 0.25f, 1.00f); // зелёный

    case CashierState::Busy:
        return ImVec4(0.90f, 0.55f, 0.10f, 1.00f); // оранжевый

    case CashierState::Offline:
        return ImVec4(0.80f, 0.10f, 0.10f, 1.00f); // красный

    default:
        return ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    }
}

CashierModel GuiUtils::CashierInfoToPresentationModel(const CashierInfo& info)
{
    CashierModel model{};
    model.id = info.cashierId;
    model.name = info.mName;
    model.currentTicket = "-";
    model.description = StateToDescription(info.mState);
    model.status = StateToText(info.mState);
    model.fColor = StateToColor(info.mState);
    const auto timeNow = std::chrono::steady_clock::now();
    const auto inactiveFor = std::chrono::duration_cast<std::chrono::milliseconds>(
        timeNow - info.lastHeartBeat
    );
    model.timeout = std::to_string(inactiveFor.count() / 1000);
    return model;
}
