/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLInt.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLInt_H
#define SQLInt_H

#include "eckit/sql/type/SQLType.h"

namespace eckit {
namespace sql {

class SQLOutput;

namespace type {

class SQLInt : public SQLType {
public:
	SQLInt(const std::string& );
	~SQLInt(); 

private:
// No copy allowed
	SQLInt(const SQLInt&);
	SQLInt& operator=(const SQLInt&);

	virtual size_t size() const;
	virtual void output(SQLOutput& s, double, bool) const;
	virtual int getKind() const { return integerType; }

	//friend std::ostream& operator<<(std::ostream& s,const SQLInt& p)
	//	{ p.print(s); return s; }

};

} // namespace type 
} // namespace sql
} // namespace eckit

#endif
