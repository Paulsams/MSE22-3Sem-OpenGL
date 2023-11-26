#pragma once

#include <QTabWidget>
#include <QGridLayout>

class FieldsDrawer;
class SceneNode;

// TODO: добавить скролл
class InspectorView final : public QWidget {
public:
    explicit InspectorView(QWidget *parent = nullptr);

    void clear();
    void fillComponents(SceneNode& sceneNode);

private:
    QGridLayout *container_;
    QTabWidget tabs_;
};
