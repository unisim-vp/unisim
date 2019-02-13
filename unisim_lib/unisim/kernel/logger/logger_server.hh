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
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/http_server.hh>
#include <pthread.h>
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

struct LoggerServer : public unisim::kernel::service::Service<unisim::service::interfaces::HttpServer>
{
	/** Http server export */
	unisim::kernel::service::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;
	
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

	/** Message debug log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug message.
	 *
	 * @param os output stream
	 * @param opt_color whether to color message
	 * @param mode type of debug message (info, warning or error)
	 * @param obj the unisim::kernel::service::Object that is sending the debug message
	 * @param buffer the debug message
	 */
	void Print(std::ostream& os, bool opt_color, mode_t mode, const char *name, const char *buffer);
	
	/** Message debug log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug message.
	 *
	 * @param mode type of debug message (info, warning or error)
	 * @param obj the unisim::kernel::service::Object that is sending the debug message
	 * @param buffer the debug message
	 */
	void Print( mode_t mode, const char *name, const char *buffer );

	/** Message debug null log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug null message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug info message
	 * @param buffer the debug null message
	 */
	void DebugNull( const char *name, const char *buffer );
	/** Message debug info log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug info message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug info message
	 * @param buffer the debug info message
	 */
	void DebugInfo( const char *name, const char *buffer );
	/** Message debug warning log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug warning message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug warning message
	 * @param buffer the debug warning message
	 */
	void DebugWarning( const char *name, const char *buffer );
	/** Message debug error log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug error message.
	 *
	 * @param obj the unisim::kernel::service::Object that is sending the debug error message
	 * @param buffer the debug error message
	 */
	void DebugError( const char *name, const char *buffer );

	/** Serve an HTTP request
	  * HTTP server calls that interface method to ask for logger to serve an incoming HTTP request intended for logger
	  * 
	  * @param req HTTP request
	  * @param conn Connection with HTTP client (web browser)
	  */
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);

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
	void XmlDebug( const char *type, const char *name, const char *buffer );

	/** Server internal closing method
         * 
	 * Invoked when no client are connected anymore
	 */
	void Close();
	
	/** Text file handler
	 */
	std::ofstream text_file_;

	/* HTTP logs */
	struct HttpLogEntry
	{
		HttpLogEntry()
			: data(0)
		{
		}
		
		HttpLogEntry(mode_t mode, const char *name, const char *buffer)
			: data(new HttpLogEntryData(mode, name, buffer))
		{
			if(data) data->Catch();
		}
		
		HttpLogEntry(const HttpLogEntry& o)
			: data(0)
		{
			Copy(o);
		}
		
		HttpLogEntry& operator = (const HttpLogEntry& o)
		{
			Copy(o);
			return *this;
		}
		
		~HttpLogEntry()
		{
			if(data) data->Release();
			data = 0;
		}
		
		mode_t GetMode() const { return data ? data->GetMode() : NO_MODE; }
		const std::string& GetName() const { static std::string null_str; return data ? data->GetName() : null_str; }
		const std::string& GetMessage() const { static std::string null_str; return data ? data->GetMessage() : null_str; }
		
	private:
		struct HttpLogEntryData
		{
			HttpLogEntryData(mode_t _mode, const char *_name, const char *buffer)
				: mode(_mode)
				, name(_name)
				, msg(buffer)
				, ref_count(0)
			{
			}
			
			void Catch()
			{
				ref_count++;
			}
			
			void Release()
			{
				if(ref_count && (--ref_count == 0))
				{
					delete this;
				}
			}
			
			mode_t GetMode() const { return mode; }
			const std::string& GetName() const { return name; }
			const std::string& GetMessage() const { return msg; }
			
		private:
			mode_t mode;
			std::string name;
			std::string msg;
			unsigned int ref_count;
		};
		
		HttpLogEntryData *data;
		
		
		void Copy(const HttpLogEntry& o)
		{
			if(data != o.data)
			{
				if(data) data->Release();
				data = o.data;
				if(data) data->Catch();
			}
		}
	};
	
	typedef HttpLogEntry HTTP_LOG_ENTRY;
	typedef std::list<HTTP_LOG_ENTRY> HTTP_LOG;
	typedef std::map<std::string, HTTP_LOG *> HTTP_LOGS_PER_CLIENT;

	HTTP_LOG global_http_log;
	HTTP_LOGS_PER_CLIENT http_logs_per_client;
	
	void PrintHttpLog(std::ostream& os, const HTTP_LOG& http_log, bool global);

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
	bool opt_http_;
	unsigned int opt_http_max_log_size_;
	pthread_mutex_t mutex;

	unisim::kernel::service::Parameter<bool>         param_std_err;
	unisim::kernel::service::Parameter<bool>         param_std_out;
	unisim::kernel::service::Parameter<bool>         param_std_err_color;
	unisim::kernel::service::Parameter<bool>         param_std_out_color;
	unisim::kernel::service::Parameter<bool>         param_file;
	unisim::kernel::service::Parameter<std::string>  param_filename;
	unisim::kernel::service::Parameter<bool>         param_xml_file;
	unisim::kernel::service::Parameter<std::string>  param_xml_filename;
	unisim::kernel::service::Parameter<bool>         param_xml_file_gzipped;
	unisim::kernel::service::Parameter<bool>         param_http;
	unisim::kernel::service::Parameter<unsigned int> param_http_max_log_size;
	
	/***************************************************************************
	 * Parameters                                                          END *
	 ***************************************************************************/
};

} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__
