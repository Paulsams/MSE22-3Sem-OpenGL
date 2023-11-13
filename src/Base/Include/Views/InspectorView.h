#pragma once

#include <QTabWidget>

class FieldsDrawer;
class SceneNode;

class InspectorView final : public QWidget {
public:
    explicit InspectorView(QWidget *parent = nullptr);

    void clear();
    void fillComponents(SceneNode& sceneNode);

private:
    QLayout *container_;
    QTabWidget tabs_;
};
