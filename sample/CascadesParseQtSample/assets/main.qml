/*
 * ParseError.hpp
 * 
 * Copyright (c) 2013, Framework Labs
 * 
 */

import bb.cascades 1.0
import com.frameworklabs.parseqt 1.0

NavigationPane {
    id: navigationPane

    Page {
        id: masterPage

        attachedObjects: [
            // Create a Parse object with the credentials from the Parse.com dashboard
            // If you created at least one Parse object with credentials, you can
            // create subsequent Parse objects (e.g. on other QML files) without those
            // credentials
            Parse {
                id: parse
                applicationId: "GdU958QagMGpO8Z9PuW4miCWu22bFsVnUjqQ4lE5"
                apiKey: "EbeFvVHANTbtIWeIfxbpp4w1Q343H8lfsxsJn857"
                trace: true
            },

            // Create a ParseQuery object to retrieve the list of items stored in the
            // cloud.
            ParseQuery {
                id: query
                className: "Item"
                limit: 100
                onFindObjectsCompleted: {

                    // Copy retrieved items from the cloud into the data-model. The
                    // list will then update from that.
                    itemsDataModel.clear()
                    if (results) {
                        itemsDataModel.append(results)
                    } else {
                        itemsDataModel.append({
                                type: "error"
                            })
                    }
                }
            },
            ArrayDataModel {
                id: itemsDataModel
            },
            ComponentDefinition {
                id: itemViewPageDefinition
                source: "asset:///ItemViewPage.qml"
            },
            Sheet {
                id: addItemSheet

                ItemViewPage {
                    id: itemViewPage

                    titleBar: TitleBar {
                        title: "Add Item"
                        dismissAction: ActionItem {
                            title: "Cancel"
                            onTriggered: {
                                addItemSheet.close()
                            }
                        }
                        acceptAction: ActionItem {
                            title: "Save"
                            onTriggered: {
                                itemViewPage.save()
                            }
                        }
                    }
                    onDidSave: {
                        addItemSheet.close()
                        masterPage.reload()
                    }
                }
            }
        ]

        function reload() {

            // Retrive items from cloud sorted by date
            query.orderByDescending("date")
            query.findObjects()
        }

        onCreationCompleted: {
            reload()
        }

        actions: [
            ActionItem {
                title: "Add Item"
                ActionBar.placement: ActionBarPlacement.OnBar
                onTriggered: {
                    // Create a new ParseObject with className "Item" and assign it to
                    // the itemViewPage.item property before opening the sheet
                    var item = parse.createObject()
                    item.className = "Item"
                    itemViewPage.item = item
                    addItemSheet.open()
                }
            },
            ActionItem {
                title: "Reload"
                onTriggered: {
                    masterPage.reload()
                }
            }
        ]

        /// layout

        titleBar: TitleBar {
            title: "CascadesParseQtSample"
        }
        Container {
            id: container

            layout: DockLayout {
            }

            ListView {
                dataModel: itemsDataModel
                enabled: ! query.busy
                function reload() {
                    masterPage.reload()
                }
                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        StandardListItem {
                            id: component
                            title: ListItemData.data.name
                            description: Qt.formatDateTime(ListItemData.data.date, "yyyy-MM-dd")
                            status: ListItemData.data.count
                            contextActions: [
                                ActionSet {
                                    DeleteActionItem {
                                        onTriggered: {
                                            ListItemData.eraseCompleted.connect(function(succ, error) {
                                                    component.ListItem.view.reload()
                                                })
                                            ListItemData.erase()
                                        }
                                    }
                                }
                            ]
                        }
                    },
                    ListItemComponent {
                        type: "error"
                        Label {
                            text: "Failed to load list"
                        }
                    }
                ]
                function itemType(data, indexPath) {
                    if (data.type && data.type == "error") {
                        return "error"
                    }
                    return "item"
                }
                onTriggered: {
                    var data = dataModel.data(indexPath)
                    var type = itemType(data, indexPath)
                    if (type != "error") {
                        var itemViewPage = itemViewPageDefinition.createObject()
                        itemViewPage.item = data
                        navigationPane.push(itemViewPage)
                    }
                }
            }

            ActivityIndicator {
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                preferredWidth: 100.0
                preferredHeight: 100.0
                running: query.busy
                visible: running
            }
        }
    }
}
