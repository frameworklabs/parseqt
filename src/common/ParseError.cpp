/*
 * ParseError.cpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#include "ParseError.hpp"

namespace parseqt {

ParseError::ParseError(QObject *parent) : QObject(parent), _domain(DomainUndefined), _code(0) { }

ParseError::ParseError(Domain domain, int code, const QString &error, QObject *parent)
	: QObject(parent), _domain(domain), _code(code), _error(error)
{
}

ParseError::~ParseError() { }

} /* namespace parseqt */
