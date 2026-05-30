#include "GuiUtils.hpp"
#include <exception>

const char* GuiUtils::StateToText(CashierState state)
{
    switch (state)
    {
    case CashierState::Ready:
        return "ГОТОВА";

    case CashierState::Free:
        return "СВОБОДНА";

    case CashierState::Busy:
        return "ЗАНЯТА";

    case CashierState::Offline:
        return "НЕДОСТУПНА";

    default:
        throw std::exception("unknown state");
    }
}

const char* GuiUtils::StateToDescription(CashierState state)
{
    switch (state)
    {
    case CashierState::Ready:
        return "Касса готова к работе";

    case CashierState::Free:
        return "Можно направить клиента";

    case CashierState::Busy:
        return "Идёт обслуживание клиента";

    case CashierState::Offline:
        return "Нет связи с процессом кассы";

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
    auto model =  CashierModel();
    model.id = info.cashierId.c_str();
    model.currentTicket = "1337";
    model.status = StateToText(info.mState);
    model.fColor = StateToColor(info.mState);
    return model;
}
