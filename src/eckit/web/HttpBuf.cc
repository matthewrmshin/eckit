/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/thread/Mutex.h"
#include "eckit/web/HttpBuf.h"
#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static int xindex = std::ios::xalloc();

//-----------------------------------------------------------------------------

typedef std::vector<char> VC;

class back_encoder_iterator : public std::iterator<std::output_iterator_tag, char>
{
	VC& container;
	void push(const char* p) { while (*p) container.push_back(*p++); }
public:
	back_encoder_iterator(VC& v) : container(v) {}
	back_encoder_iterator& operator=(char);
	back_encoder_iterator& operator*() { return *this; }
	back_encoder_iterator& operator++() { return *this; }
	back_encoder_iterator& operator++(int) { return *this; }

};


back_encoder_iterator&
back_encoder_iterator::operator=(char c)
{
	switch (c)
	{
	case '<' : push("&lt;") ; break;
	case '>' : push("&gt;") ; break;
	case '&' : push("&amp;"); break;
	case '\n': push("<BR>\n"); break;
	default  : container.push_back(c); break;
	}

	return *this;
}

inline back_encoder_iterator back_encoder(VC& x) {
	return back_encoder_iterator(x);
}

//-----------------------------------------------------------------------------

HttpBuf::HttpBuf(HttpStream& owner):
	owner_(owner)
{
	setp(out_, out_ + sizeof(out_));
}

HttpBuf::~HttpBuf()
{
	sync();
}

int HttpBuf::sync()
{

	if (owner_.iword(xindex))
		std::copy(pbase(), pptr(), back_encoder(buffer_));
	else
		std::copy(pbase(), pptr(), std::back_inserter(buffer_));

	setp(pbase(), epptr());
	return 0;
}

int HttpBuf::overflow(int c)
{
	sync();
	if (c == EOF)
		return 0;

	sputc(c);
	return 0;
}

void HttpBuf::write(std::ostream& out, Url& url)
{
	HttpHeader& header = url.headerOut();
//	header.length(buffer_.size() + 6 + 7);
	header.length(buffer_.size());

	Log::debug() << "Header: " << std::endl;

	// Send header

	out << header;
	Log::debug() << header;

	// Send data

//	out << "<HTML>";
	std::ostream_iterator<char> oi(out);
	std::copy(buffer_.begin(), buffer_.end(), oi);
//	out << "</HTML>";

#if 0
	Log::debug() << "Data: " << std::endl;

	for (std::vector<char>::iterator i = buffer_.begin(); i != buffer_.end(); ++i)
		if (isprint(*i) || isspace(*i))
			Log::debug() << *i;
		else
			break;

	Log::debug() << std::endl;
#endif
}

std::ostream& HttpBuf::dontEncode(std::ostream& s)
{
	ASSERT(s.iword(xindex) == 1);
	//s.rdbuf()->sync(); // << std::flush;
	s << std::flush;
	s.iword(xindex) = 0;
	return s;
}

std::ostream& HttpBuf::doEncode(std::ostream& s)
{
	ASSERT(s.iword(xindex) == 0);
	//s.rdbuf()->sync(); // << std::flush;
	s << std::flush;
	s.iword(xindex) = 1;
	return s;
}

HttpStream::HttpStream():
	std::ostream(new HttpBuf(*this))
{
	buf_ = (HttpBuf*)rdbuf();
	iword(xindex) = 1; // encode
}

HttpStream::~HttpStream()
{
	delete buf_;
}

void HttpStream::write(std::ostream& s, Url& url, DataHandle& stream)
{
	DataHandle* handle = url.streamFrom();
	if (handle) {
		HttpHeader& header = url.headerOut();

		header.length(handle->estimate());
		header.type("application/octet-stream");

		AutoClose close(*handle);
		s << header;

		if (Log::debug()) {
			Log::debug() << "Header: " << std::endl;
			Log::debug() << header;
			Log::debug() << "Tranfer " << handle->estimate() << " bytes" << std::endl;
		}
		handle->saveInto(stream);
	}
	else {

		flush();
		buf_->write(s, url);
	}
}

//-----------------------------------------------------------------------------

} // namespace eckit

