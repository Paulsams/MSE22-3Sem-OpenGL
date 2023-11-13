#pragma once

#include <QStandardItemModel>
#include <QTreeView>

class InspectorView;
class Scene;
class SceneNode;

class SceneHierarchyView final : public QWidget {
public:
    explicit SceneHierarchyView(InspectorView& inspector, QWidget *parent = nullptr);

    void fill(Scene& scene);

private:
    void fillInternal(SceneNode& sceneNode, QStandardItem* parentItem);

signals:
    void clicked(const QModelIndex &index) const;

    InspectorView& inspector_;
    QLayout *container_;
    QStandardItemModel model_;
    QTreeView tree_;
};
