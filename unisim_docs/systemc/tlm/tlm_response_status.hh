namespace tlm {

enum tlm_response_status {
	TLM_OK_RESPONSE = 1,
	TLM_INCOMPLETE_RESPONSE = 0,
	TLM_GENERIC_ERROR_RESPONSE = -1,
	TLM_ADDRESS_ERROR_RESPONSE = -2,
	TLM_COMMAND_ERROR_RESPONSE = -3,
	TLM_BURST_ERROR_RESPONSE = -4,
	TLM_BYTE_ENABLE_ERROR_RESPONSE = -5
};

} // namespace tlm