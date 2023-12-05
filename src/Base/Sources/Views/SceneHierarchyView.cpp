#include "Views/SceneHierarchyView.h"
#include "Views/InspectorView.h"
#include "Views/ItemInHierarchy.h"
#include "SceneLogic/Scene.h"
#include "SceneLogic/SceneNode.h"

#include <QVBoxLayout>
#include <QSizePolicy>
#include <QHeaderView>

SceneHierarchyView::SceneHierarchyView(InspectorView& inspector, QWidget* parent)
        : QWidget(parent), inspector_(inspector), container_(new QVBoxLayout(this)) {
    // TODO: хардкод цвета
    setStyleSheet("background-color: rgba(155, 155, 155, 200);");
    tree_.setModel(&model_);

    container_->setSizeConstraint(QLayout::SetMaximumSize);
    container_->setAlignment(Qt::AlignTop);

    container_->addWidget(&tree_);
    tree_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(&tree_, &QTreeView::clicked,
            this, &SceneHierarchyView::clicked);
}

void SceneHierarchyView::fill(Scene& scene) {
    QStandardItem* sceneItem = model_.invisibleRootItem();
    model_.setHorizontalHeaderLabels(QStringList("Test Scene"));
    fillInternal(scene.getRootNode(), sceneItem);
}

void SceneHierarchyView::fillInternal(SceneNode& sceneNode, QStandardItem* parentItem) {
	QObject::connect(&sceneNode, &SceneNode::addedChild, this, [this, parentItem](SceneNode&, SceneNode& child) {
		auto* childItem = new ItemInHierarchy(child);
		parentItem->appendRow(childItem);

		fillInternal(child, childItem);
	});

    sceneNode.iterateChildren([this, parentItem](SceneNode& child) {
        auto* childItem = new ItemInHierarchy(child);
        parentItem->appendRow(childItem);

        fillInternal(child, childItem);
    });
}

void SceneHierarchyView::clicked(const QModelIndex& index) const {
    auto* item = dynamic_cast<ItemInHierarchy *>(model_.itemFromIndex(index));
    inspector_.clear();
    inspector_.fillComponents(item->getNode());
}
