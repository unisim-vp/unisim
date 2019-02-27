// needs unisim/service/http_server/embedded_script.js

GUI.prototype.on_resize = function(width, height)
{
	var var_table = document.querySelector('table.var-table');
	if(var_table)
	{
		var var_table_width = width - 32;
		var_table.style.width = var_table_width + 'px';
		var_table.style.maxWidth = var_table_width + 'px';
	}
}

var gui = new GUI();
