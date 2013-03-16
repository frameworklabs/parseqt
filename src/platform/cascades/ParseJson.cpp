/*
 * ParseJson.cpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "ParseJson.hpp"

#include "ParseError.hpp"

#include <bb/data/JsonDataAccess>

using namespace bb::data;

namespace parseqt {

QByteArray ParseJson::write(const QVariant &json, ParseError **error)
{
	Q_ASSERT(error);

	JsonDataAccess dataAccess;
	QByteArray buffer;
	dataAccess.saveToBuffer(json, &buffer);
	if (dataAccess.hasError()) {
		DataAccessError dataAccessError = dataAccess.error();
		*error = new ParseError(ParseError::DomainJson, ParseError::JsonCodeFailed, dataAccessError.errorMessage());
	}
	return buffer;
}

QVariant ParseJson::read(const QByteArray &buffer, ParseError **error)
{
	Q_ASSERT(error);

	JsonDataAccess dataAccess;
	QVariant json = dataAccess.loadFromBuffer(buffer);
	if (dataAccess.hasError()) {
		DataAccessError dataAccessError = dataAccess.error();
		*error = new ParseError(ParseError::DomainJson, ParseError::JsonCodeFailed, dataAccessError.errorMessage());
		return json;
	}
	return json;
}

} /* namespace parseqt */
