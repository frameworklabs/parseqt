/*
 * Parse.cpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "Parse.hpp"

#include "ParseObject.hpp"
#include "internal/ParseManager.hpp"

namespace parseqt {

class ParseHelper : public ParseManagerDelegate {
public:
	virtual QVariant jsonify(const QVariant &data, ParseError **error);
	virtual QVariant objectify(const QVariant &json, ParseError **error);
};

QVariant ParseHelper::jsonify(const QVariant &data, ParseError **error)
{
	Q_UNUSED(data);
	Q_UNUSED(error);

	return QVariant();
}

QVariant ParseHelper::objectify(const QVariant &json, ParseError **error)
{
	Q_UNUSED(json);
	Q_UNUSED(error);

	return QVariant();
}

///

Parse::Parse(QObject *parent) : QObject(parent)
{
	ParseManager *manager = ParseManager::instance();
	if (!manager->delegate()) {
		manager->setDelegate(new ParseHelper);
	}
}

Parse::~Parse() { }

QString Parse::applicationId() const
{
	return ParseManager::instance()->applicationId();
}

void Parse::setApplicationId(const QString &applicationId)
{
	Q_ASSERT(!applicationId.isEmpty());

	ParseManager::instance()->setApplicationId(applicationId);
}

QString Parse::apiKey() const
{
	return ParseManager::instance()->apiKey();
}

void Parse::setApiKey(const QString &apiKey)
{
	Q_ASSERT(!apiKey.isEmpty());

	ParseManager::instance()->setApiKey(apiKey);
}

bool Parse::trace() const
{
	return ParseManager::instance()->trace();
}

void Parse::setTrace(bool trace)
{
	ParseManager::instance()->setTrace(trace);
}

ParseObject *Parse::createObject()
{
	return new ParseObject;
}

QDateTime Parse::dateTimeFromString(const QString &string)
{
	return ParseManager::dateTimeFromString(string);
}

QString Parse::stringFromDateTime(const QDateTime &dateTime)
{
	return ParseManager::stringFromDateTime(dateTime);
}

} /* namespace parseqt */
