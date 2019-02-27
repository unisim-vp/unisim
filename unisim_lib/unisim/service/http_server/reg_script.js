// needs unisim/service/http_server/embedded_script.js

GUI.prototype.on_resize = function(width, height)
{
	var reg_table = document.querySelector('table.reg-table');
	if(reg_table)
	{
		var reg_table_width = width - 8;
		reg_table.style.width = reg_table_width + 'px';
		reg_table.style.maxWidth = reg_table_width + 'px';
	}
}

var gui = new GUI();
