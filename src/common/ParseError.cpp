/*
 * ParseError.cpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "ParseError.hpp"

#include <QDebug>

namespace parseqt {

ParseError::ParseError(QObject *parent) : QObject(parent), _domain(DomainUndefined), _code(0) { }

ParseError::ParseError(Domain domain, int code, const QString &error, QObject *parent)
	: QObject(parent), _domain(domain), _code(code), _error(error)
{
}

ParseError::~ParseError() { }

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const ParseError &parseError)
{
	debug.nospace() << "[ParseError: " << parseError.error() << "]";
	return debug.space();
}
#endif

} /* namespace parseqt */
