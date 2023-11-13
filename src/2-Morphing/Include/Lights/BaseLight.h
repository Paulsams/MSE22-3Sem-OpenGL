#pragma once

#include "LightsContainer.h"

template<typename TLight, typename TUniformData>
class BaseLight : public BaseComponent {
public:
    using UniformData = TUniformData;

protected:
    friend LightsContainer<TLight>;

    BaseLight(LightsContainer<TLight>& container, const int indexInContainer)
        : lightsContainer_(container), indexInContainer_(indexInContainer) {
    }

    void updateLight() {
        // TODO: В идеале только нужные поля, а лучше делать isDirty и обновлять один раз за кадр, если нужно
        lightsContainer_.updatePoint(indexInContainer_, uniformData_);
    }

    UniformData uniformData_;

private:
    LightsContainer<TLight>&lightsContainer_;
    int indexInContainer_;
};
