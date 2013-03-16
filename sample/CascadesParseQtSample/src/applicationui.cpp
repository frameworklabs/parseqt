/*
 * ParseError.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "applicationui.hpp"

#include "Parse.hpp"
#include "ParseQuery.hpp"
#include "ParseObject.hpp"
#include "ParseError.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) : QObject(app)
{
	// register pasrseqt types so that they are usable in QML
	qmlRegisterType<QDeclarativePropertyMap>("com.frameworklabs.parseqt", 1, 0, "PropertyMap");
	qmlRegisterType<parseqt::Parse>("com.frameworklabs.parseqt", 1, 0, "Parse");
	qmlRegisterType<parseqt::ParseQuery>("com.frameworklabs.parseqt", 1, 0, "ParseQuery");
	qmlRegisterType<parseqt::ParseObject>("com.frameworklabs.parseqt", 1, 0, "ParseObject");
	qmlRegisterType<parseqt::ParseError>("com.frameworklabs.parseqt", 1, 0, "ParseError");

    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // set created root object as a scene
    app->setScene(root);
}

