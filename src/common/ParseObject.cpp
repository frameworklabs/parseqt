/*
 * ParseObject.cpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "ParseObject.hpp"

#include "internal/ParseManager.hpp"
#include "ParseError.hpp"

#include <QtNetwork/QNetworkReply>

#include <QDebug>

namespace parseqt {

static QVariantMap filterJsonMap(const QVariantMap &map)
{
	QVariantMap result = map;

	result.remove("objectId");
	result.remove("createdAt");
	result.remove("updatedAt");
	result.remove("valueOf"); // this gets added by QDeclarativePropertyMap for some reason

	return result;
}

static QVariantMap mergeJsonMap(const QVariantMap &base, const QVariantMap &other)
{
	QVariantMap result = base;

	QMapIterator<QString, QVariant> i(other);
	while (i.hasNext()) {
		i.next();
		result.insert(i.key(), i.value());
	}

	return result;
}

static QVariantMap diffJsonMap(const QVariantMap &base, const QVariantMap &other)
{
	QVariantMap result;

	QMapIterator<QString, QVariant> i(other);
	while (i.hasNext()) {
		i.next();
		QString key = i.key();
		QVariant baseValue = base.value(key);
		QVariant otherValue = other.value(key);
		if (baseValue != otherValue) {
			result.insert(key, otherValue);
		}
	}

	return result;
}

ParseObject::ParseObject(QObject *parent) : QObject(parent), _busy(false) { }

ParseObject::~ParseObject() { }

QString ParseObject::className() const
{
	return _className;
}

void ParseObject::setClassName(const QString &className)
{
	_className = className;
}

QDeclarativePropertyMap *ParseObject::data()
{
	return &_data;
}

QString ParseObject::objectId() const
{
	return _snapshot.value("objectId").toString();
}

QDateTime ParseObject::createdAt() const
{
	return ParseManager::dateTimeFromString(_snapshot.value("createdAt").toString());
}

QDateTime ParseObject::updatedAt() const
{
	return ParseManager::dateTimeFromString(_snapshot.value("updatedAt").toString());
}

bool ParseObject::busy() const
{
	return _busy;
}

void ParseObject::save()
{
	Q_ASSERT(!_className.isEmpty());

	if (_busy) {
		return;
	}
	setBusy(true);

	if (objectId().isEmpty()) {
		createObject();
	}
	else {
		updateObject();
	}
}

void ParseObject::erase()
{
	Q_ASSERT(!_className.isEmpty());

	if (_busy) {
		return;
	}
	setBusy(true);

	ParseError *error = ParseManager::instance()->request(QNetworkAccessManager::DeleteOperation,
											"classes/" + _className + "/" + objectId(),
											QVariant(),
											this, SLOT(eraseFinished()));

	if (error) {
		setBusy(false);

		error->setParent(this);
		Q_EMIT eraseCompleted(false, error);
	}
}

void ParseObject::eraseFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	reply->deleteLater();

	setBusy(false);

	ParseError *error = NULL;
	QVariant json = ParseManager::instance()->retrieveJsonReply(reply, 200, &error);

	if (error) {
		error->setParent(this);
		Q_EMIT eraseCompleted(false, error);
		return;
	}

	setData(QVariantMap());

	Q_EMIT eraseCompleted(true, NULL);
}

ParseError *ParseObject::setData(const QVariantMap &jsonMap)
{
	bool changedData = false;
	bool changedObjectId = false;
	bool changedCreatedAt = false;
	bool changedUpdatedAt = false;

	if (_snapshot != jsonMap) {
		changedData = true;
	}
	if (_snapshot.value("objectId") != jsonMap.value("objectId")) {
		changedObjectId = true;
	}
	if (_snapshot.value("createdAt") != jsonMap.value("createdAt")) {
		changedCreatedAt = true;
	}
	if (_snapshot.value("updatedAt") != jsonMap.value("updatedAt")) {
		changedUpdatedAt = true;
	}

	ParseError *error = fromJsonMap(jsonMap);
	if (error) {
		return error;
	}
	_snapshot = jsonMap;

	if (changedData) {
		Q_EMIT dataChanged();
	}
	if (changedObjectId) {
		Q_EMIT objectIdChanged();
	}
	if (changedCreatedAt) {
		Q_EMIT createdAtChanged();
	}
	if (changedUpdatedAt) {
		Q_EMIT updatedAtChanged();
	}

	return NULL;
}

QVariant ParseObject::toJson(ParseError **error) const
{
	Q_ASSERT(error);

	QVariantMap result;
	ParseManager *manager = ParseManager::instance();

	foreach (const QString &key, _data.keys()) {
		result.insert(key, manager->jsonify(_data.value(key), error));
	}

	return result;
}

ParseError *ParseObject::fromJsonMap(const QVariantMap &jsonMap)
{
	ParseError *error = NULL;
	ParseManager *manager = ParseManager::instance();

	foreach (const QString &key, jsonMap.keys()) {
		QVariant data = manager->objectify(jsonMap.value(key), &error);
		if (!data.isValid()) {
			return error;
		}
		_data.insert(key, data);
	}

	return NULL;
}

void ParseObject::createObject()
{
	ParseError *error = NULL;

	QVariant json = toJson(&error);

	if (json.isValid()) {
		error = ParseManager::instance()->request(QNetworkAccessManager::PostOperation,
								   	      	  	  "classes/" + _className,
								   	      	  	  filterJsonMap(json.toMap()),
								   	      	  	  this, SLOT(createObjectFinished()));
	}

	if (error) {
		setBusy(false);

		error->setParent(this);
		Q_EMIT saveCompleted(false, error);
	}
}

void ParseObject::createObjectFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	reply->deleteLater();

	setBusy(false);

	ParseError *error = NULL;
	QVariant oldJson = toJson(&error);
	QVariant newJson;

	if (oldJson.isValid()) {
		newJson = ParseManager::instance()->retrieveJsonReply(reply, 201, &error);
	}

	if (!newJson.isValid()) {
		error->setParent(this);
		Q_EMIT saveCompleted(false, error);
		return;
	}

	setData(mergeJsonMap(filterJsonMap(oldJson.toMap()), newJson.toMap()));
	Q_EMIT saveCompleted(true, NULL);
}

void ParseObject::updateObject()
{
	ParseError *error = NULL;

	QVariant json = toJson(&error);

	if (json.isValid()) {
		error = ParseManager::instance()->request(QNetworkAccessManager::PutOperation,
								   	      	  	  	      "classes/" + _className + "/" + objectId(),
								   	      	  	  	      diffJsonMap(filterJsonMap(_snapshot), filterJsonMap(json.toMap())),
								   	      	  	  	      this, SLOT(updateObjectFinished()));
	}

	if (error) {
		setBusy(false);

		error->setParent(this);
		Q_EMIT saveCompleted(false, error);
	}
}

void ParseObject::updateObjectFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	reply->deleteLater();

	setBusy(false);

	ParseError *error = NULL;
	QVariant oldJson = toJson(&error);
	QVariant newJson;

	if (oldJson.isValid()) {
		 newJson = ParseManager::instance()->retrieveJsonReply(reply, 200, &error);
	}

	if (!newJson.isValid()) {
		error->setParent(this);
		Q_EMIT saveCompleted(false, error);
		return;
	}

	setData(mergeJsonMap(_snapshot, mergeJsonMap(filterJsonMap(oldJson.toMap()), newJson.toMap())));
	Q_EMIT saveCompleted(true, NULL);
}

void ParseObject::setBusy(bool busy)
{
	if (busy != _busy) {
		_busy = busy;
		Q_EMIT busyChanged(_busy);
	}
}

} /* namespace parseqt */
