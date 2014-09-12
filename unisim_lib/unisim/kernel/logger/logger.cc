/*
 *  Copyright (c) 2008,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Daniel Gracia Perez (daniel.gracia-perez@cea.fr) 
 */

#include "unisim/kernel/logger/logger.hh"

#include <iostream>

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger_server.hh"

namespace unisim {
namespace kernel {
namespace logger {

Logger::Logger(const unisim::kernel::service::Object &obj) 
    : buffer_()
    , obj_(obj)
    , mode_(NO_MODE) {
	server_ = LoggerServer::GetInstance(obj_);
}

Logger::~Logger() {
	LoggerServer::RemoveInstance(obj_);
}

Logger& operator <<(Logger& logger, std::ostream& (*f)(std::ostream &)) {
	if (logger.mode_ == Logger::NO_MODE) return logger;
	logger.buffer_ << f;
	return logger;
}

Logger& operator <<(Logger& logger, std::ios_base& (*f)(std::ios_base &)) {
	if (logger.mode_ == Logger::NO_MODE) return logger;
	logger.buffer_ << f;
	return logger;
}

Logger& operator <<(Logger& logger, Logger& (*f)(Logger &)) {
	return f(logger);
}

std::ostream& Logger::GetStream() {
  return buffer_;
}

void Logger::PrintMode() {
  std::cerr << "Current mode (" << obj_.GetName() << "): ";
	switch (mode_) {
	case NO_MODE:
    std::cerr << "NO_MODE";
		break;
	case INFO_MODE:
    std::cerr << "INFO_MODE";
		break;
	case WARNING_MODE:
    std::cerr << "WARNING_MODE";
		break;
	case ERROR_MODE:
    std::cerr << "ERROR_MODE";
		break;
	}
  std::cerr << std::endl;
}

void Logger::DebugInfo() {
	if (mode_ != NO_MODE) {
		return;
	}
	mode_ = INFO_MODE;
	buffer_.clear();
	buffer_.str("");
}

void Logger::EndDebugInfo() {
	if (mode_ != INFO_MODE) {
		return;
	}
	mode_ = NO_MODE;
  	server_->DebugInfo(obj_, buffer_.str().c_str());
}

void Logger::DebugWarning() {
	if (mode_ != NO_MODE) return;
	mode_ = WARNING_MODE;
	buffer_.clear();
	buffer_.str("");
}

void Logger::EndDebugWarning() {
	if (mode_ != WARNING_MODE) return;
	mode_ = NO_MODE;
  	server_->DebugWarning(obj_, buffer_.str().c_str());
}

void Logger::DebugError() {
	if (mode_ != NO_MODE) return;
	mode_ = ERROR_MODE;
	buffer_.clear();
	buffer_.str("");
}

void Logger::EndDebugError() {
	if (mode_ != ERROR_MODE) return;
	mode_ = NO_MODE;
  	server_->DebugError(obj_, buffer_.str().c_str());
}

void Logger::EndDebug() {
	switch (mode_) {
	case NO_MODE:
		/* nothing to do */
		break;
	case INFO_MODE:
		EndDebugInfo();
		break;
	case WARNING_MODE:
		EndDebugWarning();
		break;
	case ERROR_MODE:
		EndDebugError();
		break;
	}
}

Logger& DebugInfo(Logger &l) {
	l.DebugInfo();
	return l;
}

Logger& EndDebugInfo(Logger &l) {
	l.EndDebugInfo();
	return l;
}

Logger& DebugWarning(Logger &l) {
	l.DebugWarning();
	return l;
}

Logger& EndDebugWarning(Logger &l) {
	l.EndDebugWarning();
	return l;
}

Logger& DebugError(Logger &l) {
	l.DebugError();
	return l;
}

Logger& EndDebugError(Logger &l) {
	l.EndDebugError();
	return l;
}

Logger& EndDebug(Logger &l) {
	l.EndDebug();
	return l;
}

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim
