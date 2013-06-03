/*
 * ParseError.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "applicationui.hpp"

#include "Parse.hpp"
#include "ParseFile.hpp"
#include "ParseQuery.hpp"
#include "ParseObject.hpp"
#include "ParseError.hpp"

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Application>
#include <bb/utility/ImageConverter>
#include <bb/ImageData>

using namespace bb::cascades;
using namespace bb::utility;

ApplicationUI::ApplicationUI(Application *app) : QObject(app)
{
	// register pickers
	qmlRegisterType<bb::cascades::pickers::FilePicker>("pickers", 1, 0,"FilePicker");
	qmlRegisterUncreatableType<bb::cascades::pickers::FileType>("pickers", 1, 0,"FileType", "enum");

	// register parseqt types so that they are usable in QML
	qmlRegisterType<parseqt::Parse>("com.frameworklabs.parseqt", 1, 0, "Parse");
	qmlRegisterType<parseqt::ParseFile>("com.frameworklabs.parseqt", 1, 0, "ParseFile");
	qmlRegisterType<parseqt::ParseQuery>("com.frameworklabs.parseqt", 1, 0, "ParseQuery");
	qmlRegisterType<parseqt::ParseObject>("com.frameworklabs.parseqt", 1, 0, "ParseObject");
	qmlRegisterType<parseqt::ParseError>("com.frameworklabs.parseqt", 1, 0, "ParseError");
	qmlRegisterType<QDeclarativePropertyMap>("com.frameworklabs.parseqt", 1, 0, "PropertyMap");

    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this).property("_app", this);

    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // set created root object as a scene
    app->setScene(root);
}

void ApplicationUI::setImageFromByteArray(bb::cascades::ImageView *imageView, const QByteArray &data) const
{
	imageView->setImage(Image(data));
}
