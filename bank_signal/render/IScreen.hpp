#pragma once

class IScreen
{
public:
    virtual ~IScreen() = default;

    virtual void render() = 0;
};