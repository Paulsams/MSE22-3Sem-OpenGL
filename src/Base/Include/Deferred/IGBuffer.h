#pragma once

// TODO: полностью переписать, потому что пока тупо два дублирования класса в разных тасках
struct IGBuffer {
    virtual void bindForLightPass() = 0;
    virtual void bindForStencilPass() = 0;
};
