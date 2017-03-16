/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Simon Smart
/// @date   March 2017

#include <unistd.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <climits>
#include <cstdlib>

#include "eckit/system/SystemInfoFreeBSD.h"

#include "eckit/io/Buffer.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

SystemInfoFreeBSD::~SystemInfoFreeBSD() {
}

LocalPathName SystemInfoFreeBSD::executablePath() const
{
    Buffer buffer(PATH_MAX);
	ssize_t size = SYSCALL(::readlink("/proc/curproc/file", buffer, buffer.size()));
    std::string path(buffer, size);
    return LocalPathName(path).realName();
}

Mem SystemInfoFreeBSD::memoryUsage() const {
    struct rusage usage;
    SYSCALL(getrusage(RUSAGE_SELF, &usage));
    return Mem(usage.ru_maxrss * 1024, 0) ;
}

size_t SystemInfoFreeBSD::memoryAllocated() const {
    return mallinfo().uordblks;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

