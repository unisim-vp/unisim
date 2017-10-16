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

#ifndef __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__
#define __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__

#include <unisim/kernel/service/service.hh>
#include <string>
#include <fstream>
#include <set>

// Forward declaration to hide libxml2 data types
typedef struct _xmlTextWriter xmlTextWriter;
typedef xmlTextWriter *xmlTextWriterPtr;

namespace unisim {
namespace kernel {
namespace logger {

struct Logger;

struct LoggerServer : public unisim::kernel::service::Object
{
	/** Constructor */
	LoggerServer(const char *name, unisim::kernel::service::Object *parent = 0);
	/** Destructor */
	~LoggerServer();

	/** Setup the logger server according to options value
	 * 
	 * Returns true if the initialization of the logger could be
	 * successfully done based on the parameters options, or false
	 * otherwise.
	 *
	 * @return true if initialization succedded, false otherwise
	 */
	bool Setup();

	/** Register a client logger.
	 * 
	 * This method should be called by the different loggers when
	 * being constructed.
	 */
	void AddClient( Logger const* client );

	/** Unregister a client logger and close server if needed
	 *  
	 *  This method should be called by the different client
	 *  loggers when being destroyed.
	 */
	void RemoveClient( Logger const* client );

	/** Message debug info log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug info message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug info message
	 * @param buffer the debug info message
	 */
	void DebugInfo( std::string name, const char *buffer );
	/** Message debug warning log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug warning message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug warning message
	 * @param buffer the debug warning message
	 */
	void DebugWarning( std::string name, const char *buffer );
	/** Message debug error log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug error message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug error message
	 * @param buffer the debug error message
	 */
	void DebugError( std::string name, const char *buffer );

private:
	/** Pointer set to the client loggers */
	std::set<Logger const*> clients;

	/** XML file handler
	 * The type of this file handler is provided by libxml2.
	 */
	xmlTextWriterPtr xml_writer_;

	/** XML output method for debug messages
	 * XML output method for debug messages of the different types.
	 *
	 * @param type   the type of the message
	 * @param obj    the object source of the message
	 * @param buffer the message buffer
	 */
	void XmlDebug( const char *type, std::string name, const char *buffer );

	/** Server internal closing method
         * 
	 * Invoked when no client are connected anymore
	 */
	void Close();

	/** Text file handler
	 */
	std::ofstream text_file_;

	/***************************************************************************
	 * Parameters                                                        START *
	 ***************************************************************************/
public:
	bool opt_std_err_;
	bool opt_std_out_;
	bool opt_std_err_color_;
	bool opt_std_out_color_;
	bool opt_file_;
	std::string opt_filename_;
	bool opt_xml_file_;
	std::string opt_xml_filename_;
	bool opt_xml_file_gzipped_;

	unisim::kernel::service::Parameter<bool>        param_std_err;
	unisim::kernel::service::Parameter<bool>        param_std_out;
	unisim::kernel::service::Parameter<bool>        param_std_err_color;
	unisim::kernel::service::Parameter<bool>        param_std_out_color;
	unisim::kernel::service::Parameter<bool>        param_file;
	unisim::kernel::service::Parameter<std::string> param_filename;
	unisim::kernel::service::Parameter<bool>        param_xml_file;
	unisim::kernel::service::Parameter<std::string> param_xml_filename;
	unisim::kernel::service::Parameter<bool>        param_xml_file_gzipped;
	
	/***************************************************************************
	 * Parameters                                                          END *
	 ***************************************************************************/
};

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__
