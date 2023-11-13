#include "Views/ItemInHierarchy.h"
#include "SceneLogic/SceneNode.h"

ItemInHierarchy::ItemInHierarchy(SceneNode& node)
    : QStandardItem(node.getName().c_str()), node_(node) {
}
