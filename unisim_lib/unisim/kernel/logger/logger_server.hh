#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include <string>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#ifndef __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__
#define __UNISIM_KERNEL_LOGGER_LOGGER_SERVER_HH__

namespace unisim {
namespace kernel {
namespace logger {

class LoggerServer : 
	public unisim::kernel::service::Object {
private:
	typedef unisim::kernel::service::Parameter<bool> bool_parameter_t;
	typedef unisim::kernel::service::Parameter<std::string> string_parameter_t;

public:
	/** Constructor */
	LoggerServer();
	/** Destructor */
	~LoggerServer();

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
	static LoggerServer *GetInstance();

	/** Free the handle to the unique logger server of the system
	 * This method should be called by the different loggers when being destroyed.
	 */
	static void RemoveInstance();
	
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
