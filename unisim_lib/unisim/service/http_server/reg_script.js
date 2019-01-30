// needs unisim/service/http_server/embedded_script.js

var window_resize_refresh_period = 500;
var window_inner_width = 0;
var window_inner_height = 0;

resize_reg_table = function(reg_table_width)
{
	var reg_table = document.querySelector('table.reg-table');
	if(reg_table)
	{
		reg_table.style.width = reg_table_width + 'px';
		reg_table.style.maxWidth = reg_table_width + 'px';
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
		resize_reg_table(window_inner_width - 32);
	}
}

document.addEventListener('DOMContentLoaded', function(event) { resize(); restore_window_scroll_top(); setInterval(resize, window_resize_refresh_period); });
