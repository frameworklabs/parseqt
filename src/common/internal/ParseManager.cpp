/*
 * ParseManager.cpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "ParseManager.hpp"

#include "ParseError.hpp"
#include "ParseJson.hpp"

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QDateTime>
#include <QDebug>

#define PQ_DATETIME_FORMAT	"yyyy-MM-ddTHH:mm:ss.zzzZ"

namespace parseqt {

Q_GLOBAL_STATIC(ParseManager, theParseManager);

ParseManager::ParseManager() : _delegate(NULL), _trace(false)
{
}

ParseManager::~ParseManager()
{
	delete _delegate;
}

ParseManager *ParseManager::instance()
{
	return theParseManager();
}

ParseManagerDelegate *ParseManager::delegate() const
{
	return _delegate;
}

void ParseManager::setDelegate(ParseManagerDelegate *delegate)
{
	delete _delegate;
	_delegate = delegate;
}

QString ParseManager::applicationId() const
{
	return _applicationId;
}

void ParseManager::setApplicationId(const QString &applicationId)
{
	_applicationId = applicationId;
}

QString ParseManager::apiKey() const
{
	return _apiKey;
}

void ParseManager::setApiKey(const QString &apiKey)
{
	_apiKey = apiKey;
}

bool ParseManager::trace() const
{
	return _trace;
}

void ParseManager::setTrace(bool trace)
{
	_trace = trace;
}

ParseError *ParseManager::request(QNetworkAccessManager::Operation op, const QString &url, const QVariant &variant, QObject *receiver, const char *slot)
{
	Q_ASSERT(!url.isEmpty());
	Q_ASSERT(receiver);
	Q_ASSERT(slot);

	// Check that application id and api key are set
	if (_applicationId.isEmpty() || _apiKey.isEmpty()) {
		return new ParseError(ParseError::DomainParseQt, ParseError::ParseQtNotInitialized, "ApplicationId or APIKey not set");
	}

	// Create NetworkRequest
	QNetworkRequest request;
	request.setUrl(QUrl("https://api.parse.com/1/" + url));
	request.setRawHeader(QString("X-Parse-Application-Id").toUtf8(), QString(_applicationId).toUtf8());
	request.setRawHeader(QString("X-Parse-REST-API-Key").toUtf8(), QString(_apiKey).toUtf8());

	// Dispatch according to method
	QNetworkReply *reply = NULL;
	ParseError *error = NULL;
	QByteArray buffer;

	switch (op) {
	case QNetworkAccessManager::HeadOperation:
		break;

	case QNetworkAccessManager::GetOperation:
		if (!variant.isValid()) {
			return new ParseError(ParseError::DomainParseQt, ParseError::ParseQtInternal, "internal");
		}
		buffer = variant.toByteArray();
		if (!buffer.isEmpty()) {
			QUrl url = request.url();
			url.setEncodedQuery(buffer);
			request.setUrl(url);
		}
		if (_trace) {
			qDebug() << "get:" << buffer;
		}
		reply = _accessManager.get(request);
		break;

	case QNetworkAccessManager::PutOperation:
		buffer = ParseJson::write(variant, &error);
		if (buffer.isEmpty()) {
			return error;
		}
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
		if (_trace) {
			qDebug() << "put:" << buffer;
		}
		reply = _accessManager.put(request, buffer);
		break;

	case QNetworkAccessManager::PostOperation:
		buffer = ParseJson::write(variant, &error);
		if (buffer.isEmpty()) {
			return error;
		}
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
		if (_trace) {
			qDebug() << "post:" << buffer;
		}
		reply = _accessManager.post(request, buffer);
		break;

	case QNetworkAccessManager::DeleteOperation:
		if (_trace) {
			qDebug() << "delete";
		}
		reply = _accessManager.deleteResource(request);
		break;

	case QNetworkAccessManager::CustomOperation:
		break;

	default:
		break;
	}

	Q_ASSERT(reply);

	// Connect for reply to finish
	bool connected = QObject::connect(reply, SIGNAL(finished()), receiver, slot);
	Q_ASSERT(connected);
	Q_UNUSED(connected);

	return NULL;
}

QVariant ParseManager::retrieveJsonReply(QNetworkReply *reply, int expectedStatusCode, ParseError **error)
{
	Q_ASSERT(reply);
	Q_ASSERT(error);

	if (reply->error() != QNetworkReply::NoError) {
		if (_trace) {
			qDebug() << "reply: error" << reply;
		}
		*error = new ParseError(ParseError::DomainQNetwork, reply->error(), reply->errorString());
		return QVariant();
	}

	QByteArray buffer = reply->readAll();
	if (_trace) {
		qDebug() << "reply:" << buffer;
	}
	QVariant json = ParseJson::read(buffer, error);
	if (!json.isValid()) {
		return QVariant();
	}

	if (expectedStatusCode != -1 && (expectedStatusCode != reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt())) {
		QVariantMap jsonMap = json.toMap();
		*error = new ParseError(ParseError::DomainParse, jsonMap["code"].toInt(), jsonMap["error"].toString());
		return QVariant();
	}

	return json;
}

QDateTime ParseManager::dateTimeFromString(const QString &string)
{
	QDateTime dateTime(QDateTime::fromString(string, PQ_DATETIME_FORMAT));
	dateTime.setTimeSpec(Qt::UTC);
	return dateTime;
}

QString ParseManager::stringFromDateTime(const QDateTime &dateTime)
{
	QDateTime utcDateTime(dateTime.toUTC());
	return utcDateTime.toString(PQ_DATETIME_FORMAT);
}

QVariant ParseManager::jsonify(const QVariant &data, ParseError **error)
{
	Q_ASSERT(error);

	QVariant::Type dataType = data.type();

	if (dataType == QVariant::DateTime) {
		QVariantMap result;
		result.insert("__type", "Date");
		result.insert("iso", stringFromDateTime(data.toDateTime()));
		return result;
	}
	if (dataType == QVariant::ByteArray) {
		QVariantMap result;
		result.insert("__type", "Bytes");
		result.insert("base64", data.toByteArray().toBase64());
		return result;
	}
	if (dataType == QVariant::List) {
		QVariantList result;
		foreach (const QVariant &variant, data.toList()) {
			QVariant json = jsonify(variant, error);
			if (!json.isValid()) {
				return json;
			}
			result.append(json);
		}
		return result;
	}
	if (dataType == QVariant::Map) {
		QVariantMap result;
		QVariantMap map = data.toMap();
		foreach (const QString &key, map.keys()) {
			QVariant json = jsonify(map.value(key), error);
			if (!json.isValid()) {
				return json;
			}
			result.insert(key, json);
		}
		return result;
	}

	if (_delegate) {
		QVariant result = _delegate->jsonify(data, error);
		if (result.isValid()) {
			return result;
		}
	}

	return data;
}

QVariant ParseManager::objectify(const QVariant &json, ParseError **error)
{
	Q_ASSERT(error);

	QVariant::Type dataType = json.type();

	if (dataType == QVariant::List) {
		QVariantList result;
		foreach (const QVariant &variant, json.toList()) {
			QVariant object = objectify(variant, error);
			if (!object.isValid()) {
				return object;
			}
			result.append(object);
		}
		return result;
	}

	if (dataType != QVariant::Map) {
		return json;
	}

	QVariantMap map = json.toMap();
	if (!map.contains("__type")) {
		QVariantMap result;
		foreach (const QString &key, map.keys()) {
			QVariant object = objectify(map.value(key), error);
			if (!object.isValid()) {
				return object;
			}
			result.insert(key, object);
		}
		return result;
	}

	QString type = map.value("__type").toString();
	if (type == "Date") {
		return dateTimeFromString(map.value("iso").toString());
	}
	if (type == "Bytes") {
		return QByteArray::fromBase64(map.value("base64").toByteArray());
	}

	if (_delegate) {
		QVariant result = _delegate->objectify(json, error);
		if (result.isValid()) {
			return result;
		}
	}

	*error = new ParseError(ParseError::DomainParseQt, ParseError::ParseQtInvalidType, "invalid type: " + type);
	return QVariant();
}

void ParseManager::debugJson(const QString &message, const QVariant &json)
{
	if (json.isValid()) {
		ParseError *error = NULL;
		qDebug() << message << ParseJson::write(json, &error);
		delete error;
	}
	else {
		qDebug() << message << "<invalid-json>";
	}
}

} /* namespace parseqt */
