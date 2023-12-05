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

signals:
	void clicked(const QModelIndex &index) const;

private:
    void fillInternal(SceneNode& sceneNode, QStandardItem* parentItem);

    InspectorView& inspector_;
    QLayout *container_;
    QStandardItemModel model_;
    QTreeView tree_;
};
