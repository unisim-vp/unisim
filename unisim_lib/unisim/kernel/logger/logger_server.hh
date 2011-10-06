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

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include <string>
#include <fstream>
#include <vector>

#ifndef __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__
#define __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__

// Forward declaration to hide libxml2 data types
typedef struct _xmlTextWriter xmlTextWriter;
typedef xmlTextWriter *xmlTextWriterPtr;

namespace unisim {
namespace kernel {
namespace logger {

class LoggerServer {
private:
	typedef unisim::kernel::service::Parameter<bool> bool_parameter_t;
	typedef unisim::kernel::service::Parameter<std::string> string_parameter_t;

public:
	/** Constructor */
	LoggerServer();
	/** Destructor */
	virtual ~LoggerServer();

	/** Object Setup method
	 * Implementation of the Setup method required by Object. Returns true if the initialization
	 *   of the logger could be successfully done based on the parameters options, or false
	 *   otherwise.
	 *
	 * @return true if initialization succedded, false otherwise
	 */
	virtual bool Setup();

	/** Object OnDisconnect method
	 * Implementation of the OnDisconnect method required by Object.
	 */
	virtual void OnDisconnect();

	/** Get a handle the unique logger server of the system
	 * This method should be called by the different loggers to get a handle to
	 *   the unique logger server that centralizes all the log requests.
	 *
	 * @return a pointer to the unique logger server of the system
	 */
	static LoggerServer *GetInstanceWithoutCountingReference();
	static LoggerServer *GetInstance(const unisim::kernel::service::Object &obj);

	/** Free the handle to the unique logger server of the system
	 * This method should be called by the different loggers when being destroyed.
	 */
	static void RemoveInstance(const unisim::kernel::service::Object &obj);

	/** Obtain the unique logger server object name
	 *
	 * @return the unique logger server object name
	 */
	static const char *GetObjectName();
	
	/** Message debug info log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug info message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug info message
	 * @param buffer the debug info message
	 */
	void DebugInfo(const unisim::kernel::service::Object &obj, const char *buffer);
	/** Message debug warning log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug warning message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug warning message
	 * @param buffer the debug warning message
	 */
	void DebugWarning(const unisim::kernel::service::Object &obj, const char *buffer);
	/** Message debug error log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug error message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug error message
	 * @param buffer the debug error message
	 */
	void DebugError(const unisim::kernel::service::Object &obj, const char *buffer);

private:
	/** Pointer to the unique logger server of the system
	 * This is the pointer that is returned when the GetInstance method is called. It must be
	 *   set up at the GetInstance method if it hasn't been set up (that is, the first time
	 *   it is called).
	 */
	static LoggerServer *singleton;

	/** Number of references to singleton
	 * This is a counter to the number of times the singleton is being used.
	 */
	static unsigned long long int singleton_refs;

	/** Pointer to the objects that demanded an instance of the logger
	 */
	static std::vector<const unisim::kernel::service::Object *> *obj_refs;

	/** The unique logger server name
	 */
	static const char *name;

	/** XML file handler
	 * The type of this file handler is provided by libxml2.
	 */
	xmlTextWriterPtr xml_writer;

	/** XML output method for debug messages
	 * XML output method for debug messages of the different types.
	 *
	 * @param type   the type of the message
	 * @param obj    the object source of the message
	 * @param buffer the message buffer
	 */
	void XmlDebug(const char *type, const unisim::kernel::service::Object &obj, const char *buffer);

	/** Text file handler
	 */
	std::ofstream text_file;

	/***************************************************************************
	 * Parameters                                                        START *
	 ***************************************************************************/

	bool opt_std_err;
	bool_parameter_t param_std_err;
	bool opt_std_out;
	bool_parameter_t param_std_out;
	bool opt_std_err_color;
	bool_parameter_t param_std_err_color;
	bool opt_std_out_color;
	bool_parameter_t param_std_out_color;
	bool opt_file;
	bool_parameter_t param_file;
	std::string opt_filename;
	string_parameter_t param_filename;
	bool opt_xml_file;
	bool_parameter_t param_xml_file;
	std::string opt_xml_filename;
	string_parameter_t param_xml_filename;
	bool opt_xml_file_gzipped;
	bool_parameter_t param_xml_file_gzipped;

	/***************************************************************************
	 * Parameters                                                          END *
	 ***************************************************************************/
};

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__
