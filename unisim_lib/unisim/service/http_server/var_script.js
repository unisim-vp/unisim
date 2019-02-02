// needs unisim/service/http_server/embedded_script.js

var window_resize_refresh_period = 500;
var window_inner_width = 0;
var window_inner_height = 0;

resize_var_table = function(var_table_width)
{
	var var_table = document.querySelector('table.var-table');
	if(var_table)
	{
		var_table.style.width = var_table_width + 'px';
		var_table.style.maxWidth = var_table_width + 'px';
	}
}

resize = function()
{
	var new_window_inner_width = window.innerWidth;
	var new_window_inner_height = window.innerHeight;
	if((new_window_inner_width != window_inner_width) || (new_window_inner_height != window_inner_height))
	{
		window_inner_width = new_window_inner_width;
		window_inner_height = new_window_inner_height;
		resize_var_table(window_inner_width - 32);
	}
}

document.addEventListener('DOMContentLoaded', function(event) { resize(); restore_window_scroll_top(); setInterval(resize, window_resize_refresh_period); });
