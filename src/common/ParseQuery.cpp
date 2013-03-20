/*
 * ParseQuery.cpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "ParseQuery.hpp"

#include "ParseObject.hpp"
#include "internal/ParseManager.hpp"
#include "ParseError.hpp"
#include "ParseJson.hpp"

#include <QtNetwork/QNetworkReply>

#include <QDebug>

namespace parseqt {

ParseQuery::ParseQuery(QObject *parent) : QObject(parent), _limit(-1), _skip(0) { }

ParseQuery::~ParseQuery() { }

QString ParseQuery::className() const
{
	return _className;
}

void ParseQuery::setClassName(const QString &className)
{
	_className = className;
}

bool ParseQuery::busy() const
{
	return _busy;
}

void ParseQuery::getObjectById(const QString &objectId)
{
	Q_ASSERT(!_className.isEmpty());

	if (_busy) {
		return;
	}
	setBusy(true);

	ParseError *error = ParseManager::instance()->request(QNetworkAccessManager::GetOperation,
								   	      	  	  	      "classes/" + _className + "/" + objectId,
								   	      	  	  	      QVariant(),
								   	      	  	  	      this, SLOT(getObjectByIdFinished()));

	if (error) {
		setBusy(false);

		Q_EMIT getObjectByIdCompleted(NULL, error);
		error->deleteLater();
	}
}

void ParseQuery::getObjectByIdFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	reply->deleteLater();

	setBusy(false);

	ParseError *error = NULL;
	QVariant json = ParseManager::instance()->retrieveJsonReply(reply, 200, &error);
	if (!json.isValid()) {
		Q_EMIT getObjectByIdCompleted(NULL, error);
		error->deleteLater();
		return;
	}

	ParseObject *result = new ParseObject();
	result->setClassName(_className);
	result->setData(json.toMap());

	Q_EMIT getObjectByIdCompleted(result, NULL);
}

void ParseQuery::whereLessThan(const QString &key, const QVariant &what)
{
	where("$lt", key, what);
}

void ParseQuery::whereLessThanOrEqualTo(const QString &key, const QVariant &what)
{
	where("$lte", key, what);
}

void ParseQuery::whereGreaterThan(const QString &key, const QVariant &what)
{
	where("$gt", key, what);
}

void ParseQuery::whereGreaterThanOrEqualTo(const QString &key, const QVariant &what)
{
	where("$gte", key, what);
}

void ParseQuery::whereNotEqualTo(const QString &key, const QVariant &what)
{
	where("$ne", key, what);
}

void ParseQuery::orderByAscending(const QString &key)
{
	_order.clear();
	addOrder(key, Qt::AscendingOrder);
}

void ParseQuery::addAscendingOrder(const QString &key)
{
	addOrder(key, Qt::AscendingOrder);
}

void ParseQuery::orderByDescending(const QString &key)
{
	_order.clear();
	addOrder(key, Qt::DescendingOrder);
}

void ParseQuery::addDescendingOrder(const QString &key)
{
	addOrder(key, Qt::DescendingOrder);
}

int ParseQuery::limit() const
{
	return _limit;
}

void ParseQuery::setLimit(int limit)
{
	Q_ASSERT(limit >= -1);

	_limit = limit;
}

int ParseQuery::skip() const
{
	return _skip;
}

void ParseQuery::setSkip(int skip)
{
	Q_ASSERT(skip >= 0);

	_skip = skip;
}

void ParseQuery::findObjects()
{
	Q_ASSERT(!_className.isEmpty());

	if (_busy) {
		return;
	}
	setBusy(true);

	ParseError *error = NULL;
	QVariant data(constraints(&error));

	if (data.isValid()) {
		error = ParseManager::instance()->request(QNetworkAccessManager::GetOperation,
								   	      	  	  "classes/" + _className,
								   	      	  	  data,
								   	      	  	  this, SLOT(findObjectsFinished()));
	}

	if (error) {
		setBusy(false);

		Q_EMIT findObjectsCompleted(QVariant(), error);
		error->deleteLater();
	}
}

void ParseQuery::findObjectsFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	reply->deleteLater();

	setBusy(false);

	ParseError *error = NULL;
	QVariant json = ParseManager::instance()->retrieveJsonReply(reply, 200, &error);
	if (!json.isValid()) {
		Q_EMIT findObjectsCompleted(QVariant(), error);
		error->deleteLater();
		return;
	}

	QVariantList results;
	QVariantList jsonResults = json.toMap().value("results").toList();
	foreach (const QVariant &jsonResult, jsonResults) {
		ParseObject *result = new ParseObject();
		result->setClassName(_className);
		result->setData(jsonResult.toMap());

		results.append(QVariant::fromValue(result));
	}

	Q_EMIT findObjectsCompleted(results, NULL);
}

void ParseQuery::where(const QString &op, const QString &key, const QVariant &what)
{
	Q_ASSERT(!key.isEmpty());
	Q_ASSERT(what.isValid());

	QVariantMap value = _where.value(key, QVariantMap()).toMap();
	value.insert(op, what);
	_where.insert(key, value);
}

void ParseQuery::addOrder(const QString &key, Qt::SortOrder sortOrder)
{
	Q_ASSERT(!key.isEmpty());

	QVariantMap entry;
	entry.insert("order", sortOrder);
	entry.insert("key", key);
	_order.append(entry);
}

QVariant ParseQuery::constraints(ParseError **error)
{
	QByteArray buffer;

	if (!_where.isEmpty()) {
		QByteArray json(ParseJson::write(_where, error));
		if (json.isEmpty()) {
			return QVariant();
		}
		buffer.append("where=");
		buffer.append(QUrl::toPercentEncoding(json));
	}

	if (!_order.isEmpty()) {
		if (buffer.size()) {
			buffer.append("&");
		}
		buffer.append("order=");
		foreach (const QVariant &entry, _order) {
			QVariantMap entryMap = entry.toMap();
			if (entryMap.value("order").toInt() == Qt::DescendingOrder) {
				buffer.append("-");
			}
			QString key = entryMap.value("key").toString();
			buffer.append(QUrl::toPercentEncoding(key));
		}
	}

	if (_limit > -1) {
		if (buffer.size()) {
			buffer.append("&");
		}
		buffer.append("limit=");
		buffer.append(QString().setNum(_limit));
	}

	if (_skip > 0) {
		if (buffer.size()) {
			buffer.append("&");
		}
		buffer.append("skip=");
		buffer.append(QString().setNum(_skip));
	}

	return QVariant(buffer);
}

void ParseQuery::setBusy(bool busy)
{
	if (busy != _busy) {
		_busy = busy;
		Q_EMIT busyChanged(_busy);
	}
}

} /* namespace parseqt */
