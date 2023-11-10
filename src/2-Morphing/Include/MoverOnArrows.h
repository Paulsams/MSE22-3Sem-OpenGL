#pragma once

#include <QWidget>
#include "SceneNode.h"
#include "BaseComponent.h"

class MoverOnArrows : public BaseComponent {
public:
    explicit MoverOnArrows(float speedMove);

protected:
    void onUpdate(float deltaTime) override;

    static QVector3D getMoveDirectionFromInput(int keyCode);

private:
    void onKeyPressed(int keyCode);

    void onKeyRelease(int keyCode);

    float speedMove_;
    QVector3D directionMove_;
};
