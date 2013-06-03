/*
 * ParseFile.cpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "ParseFile.hpp"

#include "internal/ParseManager.hpp"
#include "ParseError.hpp"

#include <QtNetwork/QNetworkReply>

#include <QFile>
#include <QDebug>

namespace parseqt {

ParseFile::ParseFile(QObject *parent) : QObject(parent), _dirty(false), _dataAvailable(false)
{
}

ParseFile::~ParseFile()
{
}

QString ParseFile::name() const
{
	return _name;
}

void ParseFile::setName(const QString &name)
{
	if (!_name.isEmpty()) {
		qWarning() << "the ParseFile object already has a name";
		return;
	}

	_name = name;
	Q_EMIT nameChanged(name);
}

QByteArray ParseFile::data() const
{
	return _data;
}

void ParseFile::setData(const QByteArray &data)
{
	if (data != _data) {
		_data = data;
		setDirty(true);
		setDataAvailable(true);

		Q_EMIT dataChanged(data);
	}
}

bool ParseFile::setDataFromFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	setData(file.readAll());
	return true;
}

bool ParseFile::dirty() const
{
	return _dirty;
}

bool ParseFile::dataAvailable() const
{
	return _dataAvailable;
}

QUrl ParseFile::url() const
{
	return _url;
}

static QString guessContentType(const QString &name)
{
	// Use QMimeType once available - or create platform variants

	QString extension = name.section('.', -1);
	if (extension == "json") {
		return "application/json";
	}
	if (extension == "png") {
		return "application/png";
	}
	if (extension == "jpg") {
		return "application/jpg";
	}
	if (extension == "txt") {
		return "application/plain";
	}
	return "application/octet-stream";
}

void ParseFile::save()
{
	ParseError *error = ParseManager::instance()->request(
			QNetworkAccessManager::PostOperation,
			"files/" + _name,
			guessContentType(_name), _data,
			this, SLOT(saveFinished()));

	if (error) {
		Q_EMIT saveCompleted(false, error);
		error->deleteLater();
	}
}

void ParseFile::saveFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	reply->deleteLater();

	ParseError *error = NULL;
	QVariant json = ParseManager::instance()->retrieveJsonReply(reply, 201, &error);
	if (!json.isValid()) {
		Q_EMIT saveCompleted(false, error);
		error->deleteLater();
		return;
	}

	QVariantMap jsonMap = json.toMap();

	_name = jsonMap.value("name").toString();
	_url = jsonMap.value("url").toUrl();
	setDirty(false);
	Q_EMIT nameChanged(_name);
	Q_EMIT urlChanged(_url);

	Q_EMIT saveCompleted(true, NULL);
}

void ParseFile::getData()
{
	ParseError *error = ParseManager::instance()->request(
			QNetworkAccessManager::GetOperation,
			"files/" + _name,
			ContentTypeNone, QVariant(),
			this, SLOT(getDataFinished()));

	if (error) {
		Q_EMIT getDataCompleted(QByteArray(), error);
		error->deleteLater();
	}
}

void ParseFile::getDataFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	reply->deleteLater();

	if (ParseManager::instance()->redirectReply(reply, this, SLOT(getDataFinished()))) {
		return;
	}

	ParseError *error = NULL;
	QByteArray buffer = ParseManager::instance()->retrieveByteArrayReply(reply, 200, &error);
	if (buffer.isNull()) {
		Q_EMIT getDataCompleted(QByteArray(), error);
		error->deleteLater();
		return;
	}

	bool didChange = _data != buffer;
	_data = buffer;
	setDirty(false);
	setDataAvailable(true);

	if (didChange) {
		Q_EMIT dataChanged(buffer);
	}

	Q_EMIT getDataCompleted(buffer, NULL);
}

void ParseFile::setDirty(bool value)
{
	if (value != _dirty) {
		_dirty = value;
		Q_EMIT dirtyChanged(value);
	}
}

void ParseFile::setDataAvailable(bool value)
{
	if (value != _dataAvailable) {
		_dataAvailable = value;
		Q_EMIT dataAvailableChanged(value);
	}
}

} /* namespace parseqt */
